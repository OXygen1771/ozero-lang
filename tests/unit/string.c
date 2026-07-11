#include "string/oz_string.h"

#include "minunit.h"

#include <string.h>

// string suite

MU_TEST(string_from_cstr) {
    OzeroString str = oz_string_from_cstr("Hello, World!");
    mu_assert(str.length == 13, "length should be 13");
    mu_assert(str.bytes != nullptr, "bytes shouldn't be nullptr");
    mu_assert(str.bytes[13] == '\0', "should be null-terminated");
    mu_assert(memcmp(str.bytes, "Hello, World!", 13) == 0,
              "contents should match");
    mu_assert(!oz_string_is_empty(str), "string shouldn't be empty");

    oz_string_free(&str);
    mu_assert(str.bytes == nullptr, "string should have been freed");
    mu_assert(oz_string_is_empty(str), "string should be empty");
}

MU_TEST(string_empty) {
    OzeroString str = oz_string_from_cstr("");

    mu_assert(oz_string_is_empty(str), "string should be empty");
    mu_assert(str.bytes != nullptr,
              "bytes shouldn't be nullptr for empty string");
    mu_assert(str.bytes[0] == '\0', "empty string should be null-terminated");

    oz_string_free(&str);
}

MU_TEST(string_null_input) {
    OzeroString str = oz_string_from_cstr(nullptr);

    mu_assert(oz_string_is_empty(str), "string should be empty");
    mu_assert(str.bytes == nullptr, "bytes should be nullptr for null input");

    oz_string_free(&str);
}

MU_TEST(string_from_bytes) {
    const char raw[] = {'t', 'e', 's', 't', ' ', 'd', 'a', 't', 'a'};
    OzeroString str = oz_string_from_bytes(raw, 9); // NOLINT

    mu_assert(str.length == 9, "length should be 9");
    mu_assert(str.bytes != nullptr, "bytes shouldn't be nullptr");
    mu_assert(memcmp(str.bytes, raw, 9) == 0, "contents should match");

    oz_string_free(&str);
}

MU_TEST(string_from_bytes_with_nulls) {
    const char raw[] = {'h', 'i', '\0', '!', '!'};
    OzeroString str = oz_string_from_bytes(raw, 5); // NOLINT

    mu_assert(str.length == 5, "length should be 5");
    mu_assert(str.bytes != nullptr, "bytes shouldn't be nullptr");
    mu_assert(str.bytes[2] == '\0', "should contain null byte at index 2");
    mu_assert(str.bytes[5] == '\0', "should be null-terminated");
    mu_assert(memcmp(str.bytes, raw, 5) == 0, "contents should match");

    oz_string_free(&str);
}

MU_TEST_SUITE(string_suite) {
    MU_RUN_TEST(string_from_cstr);
    MU_RUN_TEST(string_empty);
    MU_RUN_TEST(string_null_input);
    MU_RUN_TEST(string_from_bytes);
    MU_RUN_TEST(string_from_bytes_with_nulls);
}

// string slice suite

MU_TEST(slice_from_cstr) {
    OzeroString str1 = oz_string_from_cstr("test");
    OzeroStringSlice slice = oz_string_as_slice(str1);

    mu_assert(slice.length == str1.length, "slice length should match");
    mu_assert(!oz_string_slice_is_empty(slice), "slice shouldn't be empty");
    mu_assert(slice.bytes == str1.bytes, "slice should point to same memory");
    mu_assert(memcmp(slice.bytes, "test", 4) == 0, "contents should match");

    OzeroString str2 = oz_string_from_cstr("");
    slice = oz_string_as_slice(str2);

    mu_assert(slice.length == str2.length, "slice length should match");
    mu_assert(oz_string_slice_is_empty(slice), "slice should be empty");
    mu_assert(slice.bytes == str2.bytes, "slice should point to same memory");
    mu_assert(memcmp(slice.bytes, "", 0) == 0, "contents should match");

    oz_string_free(&str1);
    oz_string_free(&str2);
}

MU_TEST(slice_from_bytes) {
    const char data[] = {'s', 'l', 'i', 'c', 'e'};
    OzeroStringSlice slice = oz_string_slice_from_bytes(data, 5); // NOLINT

    mu_assert(slice.length == 5, "length should be 5");
    mu_assert(!oz_string_slice_is_empty(slice), "slice shouldn't be empty");
    mu_assert(slice.bytes == data, "bytes should point to original data");
}

MU_TEST(slice_from_string) {
    OzeroString str = oz_string_from_cstr("original string");
    OzeroStringSlice slice = oz_string_as_slice(str);

    mu_assert(slice.length == str.length,
              "slice length should match string length");
    mu_assert(slice.bytes == str.bytes,
              "slice bytes should point to string bytes");

    oz_string_free(&str);
}

MU_TEST(slice_null_input) {
    OzeroStringSlice slice = oz_string_slice_from_cstr(nullptr);

    mu_assert(slice.length == 0, "length should be 0");
    mu_assert(slice.bytes == nullptr, "bytes should be nullptr");
}

MU_TEST(slice_equality_checks) {
    OzeroStringSlice sl1 = oz_string_slice_from_cstr("hello");
    OzeroStringSlice sl2 = oz_string_slice_from_cstr("hello");
    OzeroStringSlice sl3 = oz_string_slice_from_cstr("world");
    OzeroStringSlice sl4 = oz_string_slice_from_cstr("a longer string");
    OzeroStringSlice empty1 = oz_string_slice_from_cstr("");
    OzeroStringSlice empty2 = oz_string_slice_from_cstr("");

    mu_assert(oz_strings_equal(sl1, sl2), "identical slices should be equal");
    mu_assert(!oz_strings_equal(sl1, sl3),
              "different slices shouldn't be equal (contents)");
    mu_assert(!oz_strings_equal(sl1, sl4),
              "different slices shouldn't be equal (length)");
    mu_assert(oz_strings_equal(empty1, empty2), "empty slices should be equal");
    mu_assert(!oz_strings_equal(sl1, empty1),
              "non-empty and empty slices should not be equal");

    mu_assert(oz_strings_equal(sl1, sl1), "a slice should be equal to itself");
}

MU_TEST_SUITE(string_slice_suite) {
    MU_RUN_TEST(slice_from_cstr);
    MU_RUN_TEST(slice_from_bytes);
    MU_RUN_TEST(slice_from_string);
    MU_RUN_TEST(slice_null_input);
    MU_RUN_TEST(slice_equality_checks);
}

int main() {
    MU_RUN_SUITE(string_suite);
    MU_RUN_SUITE(string_slice_suite);

    MU_REPORT();
    return MU_EXIT_CODE;
}
