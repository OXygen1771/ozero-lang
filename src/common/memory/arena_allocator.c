#include "arena_allocator.h"
#include "error/error.h"
#include "memory/allocator.h"
#include "platform/memory.h"

#include <stdint.h>

void oz_alloc_arena_create(OzeroArenaAllocator *arena, u8 *buffer,
                           const size_t capacity) {
    arena->buffer = buffer;
    arena->capacity = capacity;
    arena->offset = 0;
}

void oz_alloc_arena_reset(OzeroArenaAllocator *arena) { arena->offset = 0; }

static OzeroAllocationResult arena_alloc(void *ctx, const size_t size,
                                         const size_t align) {
    OzeroArenaAllocator *arena = (OzeroArenaAllocator *)ctx;

    if (size == 0 || align == 0 || (align & (align - 1)) != 0) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_INVALID_ALLOC_ARG(size, align),
        };
    }

    // calculate alignment
    const uintptr_t current = (uintptr_t)arena->buffer + arena->offset;
    const uintptr_t aligned = (current + (align - 1)) & ~(align - 1);
    const size_t padding = (size_t)(aligned - current);

    // check for integer overflow
    if (arena->offset + padding + size < arena->offset) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_INVALID_ALLOC_ARG(size, align),
        };
    }

    if (arena->offset + padding + size > arena->capacity) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_OOM(size, align),
        };
    }

    // we could cast aligned to the resulting pointer and call it a day, but
    // clang-tidy screams about casting integers into pointers:
    // performance-no-int-to-ptr "Integer to pointer cast pessimizes
    // optimization opportunities".
    // so instead we do this, costs just another sum, but the compiler could
    // optimize it better, probably.
    void *result_ptr = arena->buffer + arena->offset + padding;
    arena->offset += padding + size;
    return (OzeroAllocationResult){
        .ptr = result_ptr,
        .error = {.code = OZ_OK},
    };
}

static void arena_free(void *ctx, void *ptr, size_t size, // NOLINT
                       size_t align) {
    // arena doesn't free individual blocks
    (void)ctx;
    (void)ptr;
    (void)size;
    (void)align;
}

static void arena_free_all(void *ctx) {
    OzeroArenaAllocator *arena = (OzeroArenaAllocator *)ctx;
    arena->offset = 0;
}

static OzeroAllocationResult arena_realloc(void *ctx, void *ptr, // NOLINT
                                           size_t old_size, size_t new_size,
                                           size_t align) {
    if (new_size == 0) {
        // free()
        return (OzeroAllocationResult){.ptr = nullptr,
                                       .error = {.code = OZ_OK}};
    }
    if (ptr == nullptr) {
        return arena_alloc(ctx, new_size, align);
    }

    OzeroArenaAllocator *arena = (OzeroArenaAllocator *)ctx;

    // if ptr points to the last allocated block, we can re-use it.
    u8 *last_block_end = (u8 *)ptr + old_size;
    if (last_block_end == arena->buffer + arena->offset) {
        size_t extra_size = new_size - old_size;
        if (arena->offset + extra_size <= arena->capacity) {
            // re-using the block
            arena->offset += extra_size;
            return (OzeroAllocationResult){.ptr = ptr,
                                           .error = {.code = OZ_OK}};
        }
    }

    // otherwise realloc inside the same arena, with the old block leaking into
    // the arena
    OzeroAllocationResult result = arena_alloc(ctx, new_size, align);
    if (result.error.code != OZ_OK) {
        return result;
    }

    const size_t copy_size = old_size < new_size ? old_size : new_size;
    oz_platform_mem_copy_memory(result.ptr, ptr, copy_size);

    return result;
}

static const OzeroAllocatorVTable arena_vtable = {
    .alloc = arena_alloc,
    .free = arena_free,
    .realloc = arena_realloc, // alloc + copy
    .free_all = arena_free_all,
};

OzeroAllocator oz_alloc_arena_allocator(OzeroArenaAllocator *arena) {
    return (OzeroAllocator){
        .ctx = arena,
        .vtable = &arena_vtable,
    };
}
