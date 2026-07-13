#pragma once

// size_t, u8
#include "defines.h" // IWYU pragma: keep

// Allocate uninitialized memory (like malloc()) of size `size` in bytes.
// Returns a pointer to allocated memory or nullptr on failure.
// WARNING: Performs NO checks.
void *oz_platform_mem_malloc(size_t size);

// Allocate aligned uninitialized memory (like aligned_alloc()).
// Alignment MUST be a power of 2.
// Returns a pointer to allocated and aligned memory or nullptr on failure.
// WARNING: Performs NO checks.
void *oz_platform_mem_aligned_alloc(size_t size, size_t align);

// Free memory by pointer to a block (like free()).
// Also accepts block size, which is needed for some allocators.
// WARNING: Performs NO checks.
void oz_platform_mem_free(void *ptr, size_t size);

// Free aligned memory by pointer to a block (like free()).
// Also accepts block size, which is needed for some allocators.
// WARNING: Performs NO checks.
void oz_platform_mem_aligned_free(void *ptr, size_t size);

// Re-allocate memory by pointer to a block (like realloc()) and size. The block
// may be nullptr, in this case this is just malloc().
// Returns pointer to the newly allocated block.
// WARNING: Performs NO checks.
void *oz_platform_mem_realloc(void *ptr, size_t old_size, size_t new_size);

// Re-allocate aligned memory by pointer to a block and size. `align` MUST match
// the original alignment.
// NOTE: added this, because on Windows HeapReAlloc() can't reallocate memory
// allocated by _aligned_malloc().
// WARNING: Performs NO checks.
void *oz_platform_mem_aligned_realloc(void *ptr, size_t new_size, size_t align);

// Get platform page size in bytes. Should be used for mmap().
// WARNING: Performs NO checks.
size_t oz_platform_mem_get_page_size(void);

// Allocate memory using mmap/VirtualAlloc.
// `size` in bytes, gets rounded up to page size.
// Returns a pointer to allocated memory or nullptr on failure.
// WARNING: Performs NO checks.
void *oz_platform_mem_mmap(size_t size);

// Deallocate a block of memory allocated by mmap.
// `size` in bytes, MUST be equal to that passed to mmap().
// WARNING: Performs NO checks.
void oz_platform_mem_munmap(void *ptr, size_t size);

// Zero-out allocated memory by pointer and size (in bytes).
static inline void oz_platform_mem_zero_memory(void *ptr, size_t size) {
    if (ptr != nullptr && size > 0) {
        u8 *bytes = (u8 *)ptr;
        for (size_t i = 0; i < size; i++) {
            bytes[i] = 0;
        }
    }
}

// Copy `size` bytes from `src` to `dst` (like memcpy()).
static inline void oz_platform_mem_copy_memory(void *dst,
                                               const void *src, // NOLINT
                                               size_t size) {
    if (dst != nullptr && src != nullptr && size > 0) {
        u8 *destination = (u8 *)dst;
        const u8 *source = (const u8 *)src;
        for (size_t i = 0; i < size; i++) {
            destination[i] = source[i];
        }
    }
}
