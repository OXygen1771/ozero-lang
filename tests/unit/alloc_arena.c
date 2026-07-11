#include "error/error.h"
#include "memory/allocator.h"
#include "memory/arena_allocator.h"

#include "minunit.h"

// alloc success suite

MU_TEST(arena_alloc_single_object) {
    u8 buffer[1024];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = OZERO_ALLOC(alloc, i32);
    mu_assert(res.error.code == OZ_OK, "arena alloc should succeed");
    mu_assert(res.ptr != nullptr, "ptr shouldn't be null");
    mu_assert(((uintptr_t)res.ptr % alignof(i32)) == 0, "should be aligned");

    i32 *val = (i32 *)res.ptr;
    *val = 42;
    mu_assert(*val == 42, "should be writable");

    // free() for arena is a no-op, the buffer is on the stack, no need to call
    // free(), but just for peace of mind
    OZERO_FREE(alloc, val, i32);
}

MU_TEST(arena_alloc_array) {
    u8 buffer[1024];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, i64, 100);
    mu_assert(res.error.code == OZ_OK, "alloc array should succeed");
    mu_assert(res.ptr != nullptr, "ptr shouldn't be null");
    mu_assert(((uintptr_t)res.ptr % alignof(i64)) == 0, "should be aligned");

    i64 *arr = (i64 *)res.ptr;
    for (i64 i = 0; i < 100; i++) {
        arr[i] = i * 2;
    }
    mu_assert(arr[50] == 100, "should be writable");

    OZERO_FREE_ARRAY(alloc, arr, i64, 100);
}

MU_TEST(arena_alloc_alignment_padding) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res1 = OZERO_ALLOC(alloc, u8);
    mu_assert(res1.error.code == OZ_OK, "u8 alloc should succeed");

    const OzeroAllocationResult res2 = OZERO_ALLOC(alloc, i64);
    mu_assert(res2.error.code == OZ_OK, "i64 alloc should succeed");
    mu_assert(((uintptr_t)res2.ptr % 8) == 0, "i64 should be 8-byte aligned");

    const OzeroAllocationResult res3 = OZERO_ALLOC(alloc, u8);
    mu_assert(res3.error.code == OZ_OK, "u8 alloc should succeed");

    const OzeroAllocationResult res4 = OZERO_ALLOC(alloc, i32);
    mu_assert(res4.error.code == OZ_OK, "i32 alloc should succeed");
    mu_assert(((uintptr_t)res4.ptr % 4) == 0, "i32 should be 4-byte aligned");
}

MU_TEST(arena_alloc_multiple) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    for (int i = 0; i < 10; i++) {
        const OzeroAllocationResult res = OZERO_ALLOC(alloc, i32);
        mu_assert(res.error.code == OZ_OK, "alloc should succeed");
        *(i32 *)res.ptr = i;
    }

    mu_assert(arena.offset > 0, "offset should have advanced");
}

MU_TEST(arena_free_is_noop) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = OZERO_ALLOC(alloc, i32);
    mu_assert(res.error.code == OZ_OK, "alloc should succeed");

    const size_t offset_before = arena.offset;

    OZERO_FREE(alloc, res.ptr, i32);

    mu_assert(arena.offset == offset_before,
              "offset shouldn't change after free");
}

MU_TEST(arena_free_all) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    OZERO_ALLOC(alloc, i32);
    OZERO_ALLOC(alloc, i32);
    OZERO_ALLOC(alloc, i32);
    mu_assert(arena.offset > 0, "offset should have advanced");

    OZERO_FREE_ALL(alloc);
    mu_assert(arena.offset == 0, "offset should be reset");

    // we can allocate again
    const OzeroAllocationResult res = OZERO_ALLOC(alloc, i32);
    mu_assert(res.error.code == OZ_OK, "alloc after free_all should succeed");
    mu_assert(arena.offset > 0, "offset should have advanced");
}

MU_TEST(arena_reset) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    OZERO_ALLOC(alloc, i64);
    OZERO_ALLOC(alloc, i64);
    mu_assert(arena.offset > 0, "offset should have advanced");

    oz_alloc_arena_reset(&arena);
    mu_assert(arena.offset == 0, "offset should be reset");
}

MU_TEST_SUITE(arena_alloc_success_suite) {
    MU_RUN_TEST(arena_alloc_single_object);
    MU_RUN_TEST(arena_alloc_array);
    MU_RUN_TEST(arena_alloc_alignment_padding);
    MU_RUN_TEST(arena_alloc_multiple);
    MU_RUN_TEST(arena_free_is_noop);
    MU_RUN_TEST(arena_free_all);
    MU_RUN_TEST(arena_reset);
}

// realloc suite

MU_TEST(arena_realloc_inplace_success) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    // block A
    const OzeroAllocationResult res_a = OZERO_ALLOC_ARRAY(alloc, u8, 8);
    mu_assert(res_a.error.code == OZ_OK, "alloc A should succeed");
    u8 *blk_a = (u8 *)res_a.ptr;
    memset(blk_a, 0xAA, 8);

    // last block is B
    const OzeroAllocationResult res_b = OZERO_ALLOC_ARRAY(alloc, u8, 8);
    mu_assert(res_b.error.code == OZ_OK, "alloc B should succeed");
    u8 *blk_b = (u8 *)res_b.ptr;
    memset(blk_b, 0xBB, 8);

    // B realloc should be in-place
    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, blk_b, 8, 16, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK, "realloc should succeed");
    mu_assert(realloc_res.ptr == blk_b,
              "realloc should be in-place, pointer shouldn't have changed");

    for (int i = 0; i < 8; i++) {
        mu_assert(blk_b[i] == 0xBB, "old data should be preserved");
    }
}

MU_TEST(arena_realloc_inplace_shrink) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, u8, 32);
    mu_assert(res.error.code == OZ_OK, "alloc should succeed");
    u8 *buf = (u8 *)res.ptr;
    memset(buf, 0xCC, 32);

    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, buf, 32, 16, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK, "realloc shrink should succeed");
    mu_assert(realloc_res.ptr == buf, "realloc shrink should be in-place");

    for (int i = 0; i < 16; i++) {
        mu_assert(buf[i] == 0xCC, "old data should be preserved");
    }
}

MU_TEST(arena_realloc_fallback_not_last) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res_a = OZERO_ALLOC_ARRAY(alloc, u8, 8);
    mu_assert(res_a.error.code == OZ_OK, "alloc A should succeed");
    u8 *blk_a = (u8 *)res_a.ptr;
    memset(blk_a, 0xAA, 8);

    const OzeroAllocationResult res_b = OZERO_ALLOC_ARRAY(alloc, u8, 8);
    mu_assert(res_b.error.code == OZ_OK, "alloc B should succeed");

    // realloc for block in the middle of an arena should fallback to alloc +
    // copy
    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, blk_a, 8, 16, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK, "realloc should succeed");
    mu_assert(realloc_res.ptr != blk_a, "realloc should return a new ptr");

    u8 *new_a = (u8 *)realloc_res.ptr;
    for (int i = 0; i < 8; i++) {
        mu_assert(new_a[i] == 0xAA, "old data should be copied");
    }
}

MU_TEST(arena_realloc_null_ptr) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    // realloc(nullptr, ...) should behave like alloc
    const OzeroAllocationResult res =
        OZERO_REALLOC(alloc, nullptr, 0, 100, alignof(u8));
    mu_assert(res.error.code == OZ_OK, "realloc(nullptr) should succeed");
    mu_assert(res.ptr != nullptr, "ptr shouldn't be null");
}

MU_TEST(arena_realloc_to_zero) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, u8, 100);
    mu_assert(res.error.code == OZ_OK, "initial alloc should succeed");

    u8 *buf = (u8 *)res.ptr;

    // realloc(ptr, ..., 0) should return nullptr + OK (no-op for arena)
    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, buf, 100, 0, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK,
              "realloc to zero should succeed");
    mu_assert(realloc_res.ptr == nullptr,
              "ptr should be null after realloc to zero");
}

MU_TEST_SUITE(arena_realloc_suite) {
    MU_RUN_TEST(arena_realloc_inplace_success);
    MU_RUN_TEST(arena_realloc_inplace_shrink);
    MU_RUN_TEST(arena_realloc_fallback_not_last);
    MU_RUN_TEST(arena_realloc_null_ptr);
    MU_RUN_TEST(arena_realloc_to_zero);
}

// failure suite

MU_TEST(arena_alloc_zero_size) {
    u8 buffer[256];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = alloc.vtable->alloc(alloc.ctx, 0, 1);
    mu_assert(res.error.code == OZ_ERR_INVALID_ALLOCATION_ARGUMENT,
              "zero size should fail");
    mu_assert(res.ptr == nullptr, "ptr should be null on error");
}

MU_TEST(arena_alloc_overflow) {
    u8 buffer[12]; // small buffer
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res1 = OZERO_ALLOC(alloc, i64);
    mu_assert(res1.error.code == OZ_OK, "first alloc should succeed");

    const OzeroAllocationResult res2 = OZERO_ALLOC(alloc, i64);
    mu_assert(res2.error.code == OZ_ERR_OOM,
              "second alloc should fail with OOM");
    mu_assert(res2.ptr == nullptr, "ptr should be null on OOM");
}

MU_TEST(arena_alloc_integer_overflow) {
    u8 buffer[1024];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res =
        alloc.vtable->alloc(alloc.ctx, SIZE_MAX, 1);
    mu_assert(res.error.code == OZ_ERR_OOM, "huge allocation should fail");
    mu_assert(res.ptr == nullptr, "ptr should be null on error");
}

MU_TEST(arena_realloc_inplace_no_space) {
    u8 buffer[32]; // small buffer
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, u8, 16);
    mu_assert(res.error.code == OZ_OK, "alloc should succeed");
    u8 *buf = (u8 *)res.ptr;
    memset(buf, 0xDD, 16);

    // realloc with growth that exceeds capacity — should fail
    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, buf, 16, 64, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_ERR_OOM,
              "realloc should fail with OOM");
    mu_assert(realloc_res.ptr == nullptr, "ptr should be null on OOM");
}

MU_TEST(arena_realloc_fallback_oom) {
    u8 buffer[32]; // small buffer
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, buffer, sizeof(buffer));

    const OzeroAllocator alloc = oz_alloc_arena_allocator(&arena);

    const OzeroAllocationResult res_a = OZERO_ALLOC_ARRAY(alloc, u8, 8);
    mu_assert(res_a.error.code == OZ_OK, "alloc A should succeed");
    u8 *blk_a = (u8 *)res_a.ptr;

    // this one fills remaining space
    const OzeroAllocationResult res_b = OZERO_ALLOC_ARRAY(alloc, u8, 8);
    mu_assert(res_b.error.code == OZ_OK, "alloc B should succeed");

    // realloc not last block
    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, blk_a, 8, 32, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_ERR_OOM,
              "realloc fallback should fail with OOM");
    mu_assert(realloc_res.ptr == nullptr, "ptr should be null on OOM");
}

MU_TEST_SUITE(arena_alloc_failure_suite) {
    MU_RUN_TEST(arena_alloc_zero_size);
    MU_RUN_TEST(arena_alloc_overflow);
    MU_RUN_TEST(arena_alloc_integer_overflow);
    MU_RUN_TEST(arena_realloc_inplace_no_space);
    MU_RUN_TEST(arena_realloc_fallback_oom);
}

int main(void) {
    MU_RUN_SUITE(arena_alloc_success_suite);
    MU_RUN_SUITE(arena_realloc_suite);
    MU_RUN_SUITE(arena_alloc_failure_suite);

    MU_REPORT();
    return MU_EXIT_CODE;
}
