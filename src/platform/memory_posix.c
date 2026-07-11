#include "platform/memory.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

void *oz_platform_mem_malloc(size_t size) { return malloc(size); }

void *oz_platform_mem_aligned_alloc(size_t size, size_t align) {
    // size must be divisible by align on POSIX
    const size_t aligned_size = (size + align - 1) & ~(align - 1);
    return aligned_alloc(align, aligned_size);
}

void oz_platform_mem_free(void *ptr, size_t size) {
    (void)size; // POSIX malloc/free doesn't use this
    free(ptr);
}

void oz_platform_mem_aligned_free(void *ptr, size_t size) {
    (void)size; // aligned_alloc() doesn't need this
    if (ptr != nullptr) {
        free(ptr);
    }
}

void *oz_platform_mem_realloc(void *ptr, size_t old_size, // NOLINT
                              size_t new_size) {
    (void)old_size; // POSIX realloc doesn't use this
    return realloc(ptr, new_size);
}

size_t oz_platform_mem_get_page_size(void) {
    static size_t cached_page_size = 0;
    if (cached_page_size == 0) {
        cached_page_size = (size_t)sysconf(_SC_PAGESIZE);
    }

    return cached_page_size;
}

void *oz_platform_mem_mmap(size_t size) {
    const size_t page_size = oz_platform_mem_get_page_size();
    const size_t aligned_size = (size + page_size - 1) & ~(page_size - 1);
    void *ptr =
        mmap(nullptr, // let the system choose the address
             aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,
             -1, // no fd needed for anonymous map
             0);
    if (ptr == MAP_FAILED) {
        return nullptr;
    }
    return ptr;
}

void oz_platform_mem_munmap(void *ptr, size_t size) {
    const size_t page_size = oz_platform_mem_get_page_size();
    const size_t aligned_size = (size + page_size - 1) & ~(page_size - 1);
    munmap(ptr, aligned_size);
}
