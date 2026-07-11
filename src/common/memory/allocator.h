#pragma once

#include "error/error.h"

// Result of an allocation operation.
typedef struct OzeroAllocationResult {
    void *ptr;        //!< Pointer to allocated memory, nullptr on failure.
    OzeroError error; //!< OZ_OK if successful, error details otherwise.
} OzeroAllocationResult;

// A VTable of all allocator methods.
typedef struct OzeroAllocatorVTable {
    // --- REQUIRED METHODS
    // Allocate `size` bytes with `align` alignment.
    OzeroAllocationResult (*alloc)(void *ctx, size_t size, size_t align);

    // Free the memory by a given pointer (and size and alignment, maybe).
    void (*free)(void *ctx, void *ptr, size_t size, size_t align);

    // --- OPTIONAL, will return an error if the allocator doesn't implement it.
    // Change the size of an allocated block, potentially moving all contents.
    // If `new_size` == 0, free the block.
    // If `ptr` == nullptr, allocates a new block exactly like alloc(), ignoring
    // old_size.
    OzeroAllocationResult (*realloc)(void *ctx, void *ptr, size_t old_size,
                                     size_t new_size, size_t align);

    // Free all the memory at once (Arena/Stack). No-op if not implemented.
    void (*free_all)(void *ctx);
} OzeroAllocatorVTable;

// An allocator descriptor.
typedef struct OzeroAllocator {
    void *ctx;                          //!< Opaque allocator state and type.
    const OzeroAllocatorVTable *vtable; //!< Method table for the allocator.
} OzeroAllocator;

// (hopefully) safe macros

// Allocate memory for a single object of type `Type`.
#define OZERO_ALLOC(allocator, Type)                                           \
    ((allocator).vtable->alloc((allocator).ctx, sizeof(Type), alignof(Type)))

// Allocate memory for a `count` of objects of type `Type`. Checks for size
// overflow, may return an "Invalid argument" error.
#define OZERO_ALLOC_ARRAY(allocator, Type, count)                              \
    oz__alloc_array_safe((allocator), sizeof(Type), (size_t)(count),           \
                         alignof(Type))

// Free memory for a single object of type `Type`.
#define OZERO_FREE(allocator, ptr, Type)                                       \
    ((allocator).vtable->free((allocator).ctx, (ptr), sizeof(Type),            \
                              alignof(Type)))

// Allocate memory for a `count` of objects of type `Type`.
#define OZERO_FREE_ARRAY(allocator, ptr, Type, count)                          \
    ((allocator).vtable->free((allocator).ctx, (ptr),                          \
                              sizeof(Type) * (size_t)(count), alignof(Type)))

// Reallocate a block of memory. If the allocator doesn't support realloc,
// returns an error.
#define OZERO_REALLOC(allocator, ptr, old_size, new_size, align)               \
    oz__realloc_safe((allocator), (ptr), (old_size), (new_size), (align))

// Free all the memory allocated by an allocator. No-op if unsupported.
#define OZERO_FREE_ALL(allocator)                                              \
    do {                                                                       \
        if ((allocator).vtable->free_all != nullptr) {                         \
            (allocator).vtable->free_all((allocator).ctx);                     \
        }                                                                      \
    } while (0)

static inline OzeroAllocationResult
oz__alloc_array_safe(OzeroAllocator allocator, size_t element_size,
                     size_t count, size_t align) {
    if (count > 0 && element_size > SIZE_MAX / count) {
        return (OzeroAllocationResult){
            .ptr = nullptr,
            .error = OZ_ERROR_MAKE_INVALID_ALLOC_ARG(0, align),
        };
    }
    return allocator.vtable->alloc(allocator.ctx, element_size * count, align);
}

static inline OzeroAllocationResult oz__realloc_safe(OzeroAllocator allocator,
                                                     void *ptr, size_t old_size,
                                                     size_t new_size,
                                                     size_t align) {
    if (allocator.vtable->realloc != nullptr) {
        return allocator.vtable->realloc(allocator.ctx, ptr, old_size, new_size,
                                         align);
    }
    return (OzeroAllocationResult){
        .ptr = nullptr,
        .error = OZ_ERROR_MAKE_UNSUPPORTED_ALLOC_OP(OZ_ALLOC_OP_REALLOC),
    };
}
