#pragma once

#include "logger.h" // IWYU pragma: keep

#include "ozerolang/config.h"

#ifdef OZERO_ASSERTIONS_ENABLED
#ifdef _MSC_VER
#define debug_break() __debugbreak()
#else
#define debug_break() __builtin_trap()
#endif // _MSC_VER

/**
 * Report an assertion failure as a fatal error message with optional variadic
 * arguments. Should not be used directly, use one of the OZ_ASSERT() macros
 * instead.
 */
#define __OZ_REPORT_ASSERTION_FAILURE(expression, message, ...)                \
    do {                                                                       \
        oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_FATAL,                \
                        "Assertion failed: %s", #expression);                  \
        oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_FATAL,                \
                        message __VA_OPT__(, ) __VA_ARGS__);                   \
    } while (0)

// assertion macros

// Assert an expression's truthfullnes and display a message.
#define OZ_ASSERT(expression, message, ...)                                    \
    do {                                                                       \
        if (expression) {                                                      \
        } else {                                                               \
            __OZ_REPORT_ASSERTION_FAILURE(expression,                          \
                                          message __VA_OPT__(, ) __VA_ARGS__); \
            debug_break();                                                     \
        }                                                                      \
    } while (0)

#ifdef OZ_DEBUG
// Assert an expression's truthfullnes and display a message. Gets disabled when
// the OZ_DEBUG flag isn't set (i.e. in release builds).
#define OZ_ASSERT_DEBUG(expression, message, ...)                              \
    OZ_ASSERT(expression, message __VA_OPT__(, ) __VA_ARGS__)
#else
// Assert an expression's truthfullnes and display a message. Gets disabled when
// the OZ_DEBUG flag isn't set (i.e. in release builds).
#define OZ_ASSERT_DEBUG(expression, message, ...)
#endif // OZ_DEBUG

// no assertion macros
#else
// Assert an expression's truthfullnes and display a message.
#define OZ_ASSERT(expression, message, ...)

// Assert an expression's truthfullnes and display a message. Gets disabled when
// the OZ_DEBUG flag isn't set (i.e. in release builds).
#define OZ_ASSERT_DEBUG(expression, message, ...)

#endif // OZERO_ASSERTIONS_ENABLED
