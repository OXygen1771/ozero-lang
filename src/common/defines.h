#pragma once

#include <stdint.h>

// Number typedefs

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

static_assert(sizeof(u8) == 1, "u8 must be exactly 1 byte");
static_assert(sizeof(u16) == 2, "u16 must be exactly 1 bytes");
static_assert(sizeof(u32) == 4, "u32 must be exactly 1 bytes");
static_assert(sizeof(u64) == 8, "u64 must be exactly 1 bytes"); // NOLINT

static_assert(sizeof(i8) == 1, "i8 must be exactly 1 byte");
static_assert(sizeof(i16) == 2, "i16 must be exactly 1 bytes");
static_assert(sizeof(i32) == 4, "i32 must be exactly 1 bytes");
static_assert(sizeof(i64) == 8, "i64 must be exactly 1 bytes"); // NOLINT

static_assert(sizeof(f32) == 4, "f32 must be exactly 4 bytes");
static_assert(sizeof(f64) == 8, "f32 must be exactly 4 bytes"); // NOLINT

// Other typedefs

#if defined(__GNUC__) || defined(__clang__)
// Tell the compiler about our own printf-like function.
#define OZ_ATTR_PRINTF(fmt_idx, args_idx)                                      \
    __attribute__((__format__(__printf__, fmt_idx, args_idx)))
#define MSVC_FMT_STRING
#elif defined(_MSC_VER)
#include <sal.h>
// Tell the compiler about our own printf-like function.
#define OZ_ATTR_PRINTF(fmt_idx, args_idx)
#define MSVC_FMT_STRING _Printf_format_string_
#else
// Tell the compiler about our own printf-like function.
#define OZ_ATTR_PRINTF(fmt_idx, args_idx)
#define MSVC_FMT_STRING
#endif
