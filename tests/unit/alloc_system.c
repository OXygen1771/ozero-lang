#include "memory/allocator.h"
#include "memory/system_allocator.h"

#include "minunit.h"

// alloc succes suite

MU_TEST(system_alloc_single_object) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    const OzeroAllocationResult res = OZERO_ALLOC(alloc, i32);
    mu_assert(res.error.code == OZ_OK, "alloc should succeed");
    mu_assert(res.ptr != nullptr, "ptr shouldn't be null");
    mu_assert(((uintptr_t)res.ptr % alignof(i32)) == 0, "should be aligned");

    i32 *val = (i32 *)res.ptr;
    *val = 42;
    mu_assert(*val == 42, "should be writable");

    OZERO_FREE(alloc, val, i32);
}

MU_TEST(system_alloc_array) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

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

MU_TEST(system_alloc_various_alignments) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    // align = 1
    const OzeroAllocationResult res1 = alloc.vtable->alloc(alloc.ctx, 100, 1);
    mu_assert(res1.error.code == OZ_OK, "align=1 should succeed");
    alloc.vtable->free(alloc.ctx, res1.ptr, 100, 1);

    // align = 8
    const OzeroAllocationResult res8 = alloc.vtable->alloc(alloc.ctx, 100, 8);
    mu_assert(res8.error.code == OZ_OK, "align=8 should succeed");
    mu_assert(((uintptr_t)res8.ptr % 8) == 0, "should be 8-byte aligned");
    alloc.vtable->free(alloc.ctx, res8.ptr, 100, 8);

    // align = 16
    const OzeroAllocationResult res16 = alloc.vtable->alloc(alloc.ctx, 100, 16);
    mu_assert(res16.error.code == OZ_OK, "align=16 should succeed");
    mu_assert(((uintptr_t)res16.ptr % 16) == 0, "should be 16-byte aligned");
    alloc.vtable->free(alloc.ctx, res16.ptr, 100, 16);

    // align = 64
    const OzeroAllocationResult res64 = alloc.vtable->alloc(alloc.ctx, 100, 64);
    mu_assert(res64.error.code == OZ_OK, "align=64 should succeed");
    mu_assert(((uintptr_t)res64.ptr % 64) == 0, "should be 64-byte aligned");
    alloc.vtable->free(alloc.ctx, res64.ptr, 100, 64);
}

MU_TEST(system_free_null) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    // shouldn't crash
    OZERO_FREE(alloc, nullptr, i32);
}

MU_TEST_SUITE(system_alloc_success_suite) {
    MU_RUN_TEST(system_alloc_single_object);
    MU_RUN_TEST(system_alloc_array);
    MU_RUN_TEST(system_alloc_various_alignments);
    MU_RUN_TEST(system_free_null);
}

// realloc suite

MU_TEST(system_realloc_grow) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, u8, 16);
    mu_assert(res.error.code == OZ_OK, "initial alloc should succeed");

    u8 *buf = (u8 *)res.ptr;
    memset(buf, 0xAA, 16);

    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, buf, 16, 32, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK, "realloc should succeed");

    u8 *new_buf = (u8 *)realloc_res.ptr;
    for (int i = 0; i < 16; i++) {
        mu_assert(new_buf[i] == 0xAA, "old data should be preserved");
    }

    OZERO_FREE_ARRAY(alloc, new_buf, u8, 32);
}

MU_TEST(system_realloc_shrink) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, u8, 32);
    mu_assert(res.error.code == OZ_OK, "initial alloc should succeed");

    u8 *buf = (u8 *)res.ptr;
    memset(buf, 0xBB, 32);

    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, buf, 32, 16, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK, "realloc shrink should succeed");

    u8 *new_buf = (u8 *)realloc_res.ptr;
    for (int i = 0; i < 16; i++) {
        mu_assert(new_buf[i] == 0xBB,
                  "old data should be preserved"); // (up to new size)
    }

    OZERO_FREE_ARRAY(alloc, new_buf, u8, 16);
}

MU_TEST(system_realloc_null_ptr) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    // realloc(nullptr, ...) should behave like alloc
    const OzeroAllocationResult res =
        OZERO_REALLOC(alloc, nullptr, 0, 100, alignof(u8));
    mu_assert(res.error.code == OZ_OK, "realloc(nullptr) should succeed");
    mu_assert(res.ptr != nullptr, "ptr shouldn't be null");

    OZERO_FREE(alloc, res.ptr, u8);
}

MU_TEST(system_realloc_to_zero) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, u8, 100);
    mu_assert(res.error.code == OZ_OK, "initial alloc should succeed");

    u8 *buf = (u8 *)res.ptr;

    // realloc(ptr, ..., 0) should free and return nullptr
    OzeroAllocationResult realloc_res =
        OZERO_REALLOC(alloc, buf, 100, 0, alignof(u8));
    mu_assert(realloc_res.error.code == OZ_OK,
              "realloc to zero should succeed");
    mu_assert(realloc_res.ptr == nullptr,
              "ptr should be null after realloc to zero");
}

MU_TEST_SUITE(system_realloc_suite) {
    MU_RUN_TEST(system_realloc_grow);
    MU_RUN_TEST(system_realloc_shrink);
    MU_RUN_TEST(system_realloc_null_ptr);
    MU_RUN_TEST(system_realloc_to_zero);
}

// failure suite

MU_TEST(system_alloc_zero_size) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    const OzeroAllocationResult res = alloc.vtable->alloc(alloc.ctx, 0, 1);
    mu_assert(res.error.code == OZ_ERR_INVALID_ALLOCATION_ARGUMENT,
              "zero size should fail");
    mu_assert(res.ptr == nullptr, "ptr should be null on error");
}

MU_TEST(system_alloc_array_overflow) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    const OzeroAllocationResult res = OZERO_ALLOC_ARRAY(alloc, i64, SIZE_MAX);
    mu_assert(res.error.code == OZ_ERR_INVALID_ALLOCATION_ARGUMENT,
              "integer overflow should be caught");
    mu_assert(res.ptr == nullptr, "ptr should be null on overflow");
}

MU_TEST(system_free_all_noop) {
    const OzeroAllocator alloc = oz_alloc_get_system_alloc();

    // should be a no-op
    OZERO_FREE_ALL(alloc);
}

MU_TEST_SUITE(system_alloc_failure_suite) {
    MU_RUN_TEST(system_alloc_zero_size);
    MU_RUN_TEST(system_alloc_array_overflow);
    MU_RUN_TEST(system_free_all_noop);
}

int main(void) {
    MU_RUN_SUITE(system_alloc_success_suite);
    MU_RUN_SUITE(system_realloc_suite);
    MU_RUN_SUITE(system_alloc_failure_suite);

    MU_REPORT();
    return MU_EXIT_CODE;
}
