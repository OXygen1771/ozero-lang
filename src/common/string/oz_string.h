#pragma once

// for size_t
#include "defines.h" // IWYU pragma: keep
#include <stddef.h>

/*
 * A UTF-8 string owning its memory. May or may not be null-terminated.
 */
typedef struct {
    // Length in bytes (not in characters!).
    // WARNING: Does NOT include the null-terminator.
    size_t length;
    // Underlying character memory. Should not be used directly.
    char *bytes;
} OzeroString;

/*
 * A UTF-8 string slice, does not own its memory.
 */
typedef struct {
    // Length in bytes (not in characters!).
    size_t length;
    // Underlying character memory. Should not be used directly.
    const char *bytes;
} OzeroStringSlice;

// Create a new string from a C-string, copying the data.
// Returns a null-string (length=0, nullptr to bytes) on allocation failure or
// empty input.
OzeroString oz_string_from_cstr(const char *cstr);

// Create a new string from raw bytes, copying the data. The resulting string
// will be null-terminated.
// Returns a null-string (length=0, nullptr to bytes) on allocation failure or
// empty input or length=0.
OzeroString oz_string_from_bytes(const char *bytes, size_t length);

// Free the memory owned by a string and destroy it. The given string will
// be converted to a null-string (length=0, nullptr to bytes).
void oz_string_free(OzeroString *str);

// Create a full slice of an owning string.
static inline OzeroStringSlice oz_string_as_slice(OzeroString str) {
    return (OzeroStringSlice){.length = str.length, .bytes = str.bytes};
}

// Create a new string slice from a C-string (`cstr`).
// Returns a null-string slice (length=0, nullptr to bytes) on allocation
// failure or empty input.
OzeroStringSlice oz_string_slice_from_cstr(const char *cstr);

// Create a new string slice from raw bytes. No bounds checking.
static inline OzeroStringSlice oz_string_slice_from_bytes(const char *bytes,
                                                          size_t length) {
    return (OzeroStringSlice){.length = length, .bytes = bytes};
}

// --- encoding-agnostic ops

// Check if two strings (slices) are equal.
bool oz_strings_equal(OzeroStringSlice sl_a, OzeroStringSlice sl_b);

static inline bool oz_string_is_empty(OzeroString str) {
    return str.length == 0;
}

static inline bool oz_string_slice_is_empty(OzeroStringSlice slice) {
    return slice.length == 0;
}
