#pragma once

#include "allocator.h"

// An Arena (linear) allocator. Allocates memory from a fixed buffer, the arena
// is freed all at once.
typedef struct OzeroArenaAllocator {
    u8 *buffer;      //!< The buffer of the arena (not owned by the arena).
    size_t capacity; //!< Size of the arena in bytes.
    size_t offset;   //!< Current offset, or the next free block in the arena.
} OzeroArenaAllocator;

// Create an arena using a pre-allocated buffer.
void oz_alloc_arena_create(OzeroArenaAllocator *arena, u8 *buffer,
                           size_t capacity);

// Reset the arena, zeroing the offset and not freeing the buffer.
void oz_alloc_arena_reset(OzeroArenaAllocator *arena);

// Get the Arena allocator.
OzeroAllocator oz_alloc_arena_allocator(OzeroArenaAllocator *arena);
