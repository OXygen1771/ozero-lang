#include "memory.h"

#define WIN32_LEAN_AND_MEAN
#include <corecrt_malloc.h>
#include <windows.h>

void *oz_platform_mem_malloc(size_t size) {
    return HeapAlloc(GetProcessHeap(), 0, size);
}

void *oz_platform_mem_aligned_alloc(size_t size, size_t align) {
    // align >= sizeof(void*) on Windows
    if (align < sizeof(void *)) {
        align = sizeof(void *);
    }

    return _aligned_malloc(size, align);
}

void oz_platform_mem_free(void *ptr, size_t size) {
    (void)size; // unused in HeapFree()

    HeapFree(GetProcessHeap(), 0, ptr);
}

void oz_platform_mem_aligned_free(void *ptr, size_t size) {
    (void)size;
    _aligned_free(ptr);
}

void *oz_platform_mem_realloc(void *ptr, size_t old_size, // NOLINT
                              size_t new_size) {
    (void)old_size;

    return HeapReAlloc(GetProcessHeap(), 0, ptr, new_size);
}

void *oz_platform_mem_aligned_realloc(void *ptr, size_t new_size,
                                      size_t align) {
    // align >= sizeof(void*) on Windows
    if (align < sizeof(void *)) {
        align = sizeof(void *);
    }

    return _aligned_realloc(ptr, new_size, align);
}

size_t oz_platform_mem_page_size(void) {
    static size_t cached_page_size = 0;
    if (cached_page_size == 0) {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        cached_page_size = sys_info.dwPageSize;
    }
    return cached_page_size;
}

void *oz_platform_mem_mmap(size_t size) {
    size_t page_size = oz_platform_mem_page_size();
    size_t aligned_size = (size + page_size - 1) & ~(page_size - 1);

    void *ptr = VirtualAlloc(nullptr, aligned_size, MEM_COMMIT | MEM_RESERVE,
                             PAGE_READWRITE);

    return ptr; // nullptr on failure
}

void oz_platform_mem_munmap(void *ptr, size_t size) {
    (void)size; // not needed
    VirtualFree(ptr, 0, MEM_RELEASE);
}
