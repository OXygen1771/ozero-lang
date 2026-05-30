#include "logger.h"

#include "styler/color.h"
#include "styler/color_shorthands.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define OZ_LOG_MAX_MESSAGE_LENGTH 32768 // TEMPORARY

static OzeroTextStyle log_level_name_styles[] = {
    [OZ_LOG_LEVEL_TRACE] = OZ_STYLE_DIM,
    [OZ_LOG_LEVEL_DEBUG] = OZ_STYLE_CYAN,
    [OZ_LOG_LEVEL_INFO] = OZ_STYLE_BRIGHT_WHITE,
    [OZ_LOG_LEVEL_WARN] = OZ_STYLE_YELLOW,
    [OZ_LOG_LEVEL_ERROR] = OZ_STYLE_RED,
    [OZ_LOG_LEVEL_FATAL] = OZ_STYLE_BRIGHT_RED,
};

static const char *log_level_names[] = {
    [OZ_LOG_LEVEL_TRACE] = "TRACE", [OZ_LOG_LEVEL_DEBUG] = "DEBUG",
    [OZ_LOG_LEVEL_INFO] = "INFO",   [OZ_LOG_LEVEL_WARN] = "WARN",
    [OZ_LOG_LEVEL_ERROR] = "ERROR", [OZ_LOG_LEVEL_FATAL] = "FATAL",
};

static const size_t time_buf_size = 32; // arbitrary number

void oz__log_message(const char *filename, u32 line, OzeroLogLevel level,
                     const char *fmt, ...) {
    // time formatting
    time_t timer;
    struct tm tm_info;
    char time_buffer[time_buf_size];
    timer = time(&timer);
    localtime_r(&timer, &tm_info);
    // leave it simple for now
    strftime(time_buffer, time_buf_size, "%H:%M:%S", &tm_info);

    // form full log line
    char prefix[time_buf_size * 2]; // time + log level
    snprintf(prefix, sizeof(prefix), "%s - [%s]", time_buffer,
             log_level_names[level]);

    // colorize prefix
    FILE *out_fstream = (level > OZ_LOG_LEVEL_WARN) ? stderr : stdout;
    OZ_STYLE(out_fstream, log_level_name_styles[level], prefix);
    fprintf(out_fstream, " at (%s:%d): ", filename, line);

    // pasting variadic arguments
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    vfprintf(out_fstream, fmt, arg_ptr);
    va_end(arg_ptr);

    fputc('\n', out_fstream);
}
