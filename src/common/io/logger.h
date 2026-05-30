#pragma once

#include "defines.h"
#include "ozerolang/config.h"

typedef enum OzeroLogLevel {
    OZ_LOG_LEVEL_TRACE = 0, // In-depth debugging messages.
    OZ_LOG_LEVEL_DEBUG = 1, // All-purpose debugging messages.
    OZ_LOG_LEVEL_INFO = 2,  // Basic information messages.
    OZ_LOG_LEVEL_WARN = 3,  // Info about unusual, but recoverable situations.
    OZ_LOG_LEVEL_ERROR = 4, // Critical, but *maybe* recoverable errors.
    OZ_LOG_LEVEL_FATAL = 5, // Critical and unrecoverable errors.
} OzeroLogLevel;

/**
 * Main logging function to handle all logging output.
 *
 * Writes to standard output streams (for now). Accepts a format string and
 * variadic arguments to insert into the format string.
 *
 * Should not be used directly, use the OZ_TRACE, OZ_DEBUG, OZ_INFO, OZ_WARN,
 * OZ_ERROR and OZ_FATAL macros instead.
 */
void oz__log_message(const char *filename, u32 line, OzeroLogLevel level,
                     MSVC_FMT_STRING const char *fmt, ...) OZ_ATTR_PRINTF(4, 5);

#ifdef OZERO_LOG_TRACE_ENABLED
// Log a tracing message.
#define OZ_TRACE(message, ...)                                                 \
    oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_TRACE,                    \
                    message __VA_OPT__(, ) __VA_ARGS__)
#else
// Log a tracing message.
#define OZ_TRACE(message, ...)
#endif // OZERO_LOG_TRACE_ENABLED

#ifdef OZERO_LOG_DEBUG_ENABLED
// Log a debugging message.
#define OZ_DEBUG(message, ...)                                                 \
    oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_DEBUG,                    \
                    message __VA_OPT__(, ) __VA_ARGS__)
#else
// Log a debugging message.
#define OZ_DEBUG(message, ...)
#endif // OZERO_LOG_DEBUG_ENABLED

#ifdef OZERO_LOG_INFO_ENABLED
// Log an info message.
#define OZ_INFO(message, ...)                                                  \
    oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_INFO,                     \
                    message __VA_OPT__(, ) __VA_ARGS__)
#else
// Log an info message.
#define OZ_INFO(message, ...)
#endif // OZERO_LOG_INFO_ENABLED

#ifdef OZERO_LOG_WARN_ENABLED
// Log a warning message.
#define OZ_WARN(message, ...)                                                  \
    oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_WARN,                     \
                    message __VA_OPT__(, ) __VA_ARGS__)
#else
// Log a warning message.
#define OZ_WARN(message, ...)
#endif // OZERO_LOG_WARN_ENABLED

#ifndef OZERO_LOG_ERROR_DISABLED
// Log an error message.
#define OZ_ERROR(message, ...)                                                 \
    oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_ERROR,                    \
                    message __VA_OPT__(, ) __VA_ARGS__)
#else
// Log an error message.
#define OZ_ERROR(message, ...)
#endif // OZERO_LOG_ERROR_DISABLED

#ifndef OZERO_LOG_FATAL_DISABLED
// Log a fatal error message.
#define OZ_FATAL(message, ...)                                                 \
    oz__log_message(__FILE__, __LINE__, OZ_LOG_LEVEL_FATAL,                    \
                    message __VA_OPT__(, ) __VA_ARGS__)
#else
// Log a fatal error message.
#define OZ_FATAL(message, ...)
#endif // OZERO_LOG_FATAL_DISABLED
