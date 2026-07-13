#include "system_allocator.h"
#include "error/error.h"
#include "platform/memory.h"

// ctx is unused by the system allocator.

static OzeroAllocationResult system_alloc(void *ctx, size_t size,
                                          size_t align) {
    (void)ctx;

    if (size == 0 || align == 0 || (align & (align - 1)) != 0) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_INVALID_ALLOC_ARG(size, align),
        };
    }

    void *ptr = oz_platform_mem_aligned_alloc(size, align);
    if (ptr == nullptr) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_OOM(size, align),
        };
    }

    return (OzeroAllocationResult){.ptr = ptr, .error = {.code = OZ_OK}};
}

static void system_free(void *ctx, void *ptr, size_t size, // NOLINT
                        size_t align) {
    (void)ctx;
    (void)size;
    (void)align;
    if (ptr != nullptr) {
        oz_platform_mem_aligned_free(ptr, size);
    }
}

static OzeroAllocationResult system_realloc(void *ctx, void *ptr, // NOLINT
                                            size_t old_size,      // NOLINT
                                            size_t new_size, size_t align) {
    (void)ctx;
    if (new_size == 0) {
        oz_platform_mem_aligned_free(ptr, old_size);
        return (OzeroAllocationResult){.ptr = nullptr,
                                       .error = {.code = OZ_OK}};
    }

    void *new_ptr = oz_platform_mem_aligned_realloc(ptr, new_size, align);
    if (new_ptr == nullptr) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_OOM(new_size, align),
        };
    }

    return (OzeroAllocationResult){.ptr = new_ptr, .error = {.code = OZ_OK}};
}

static const OzeroAllocatorVTable system_vtable = {
    .alloc = system_alloc,
    .free = system_free,
    .realloc = system_realloc,
    .free_all = nullptr, // Heap не поддерживает free_all
};

OzeroAllocator oz_alloc_get_system_alloc(void) {
    return (OzeroAllocator){
        .ctx = nullptr,
        .vtable = &system_vtable,
    };
}
