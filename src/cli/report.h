#pragma once

#include "defines.h"

// CLI message level for the user.
typedef enum OzeroCLIReportLevel {
    OZ_CLI_REPORT_LVL_SUCCESS,
    OZ_CLI_REPORT_LVL_HINT,
    OZ_CLI_REPORT_LVL_WARNING,
    OZ_CLI_REPORT_LVL_ERROR,
} OzeroCLIReportLevel;

// Main function to handle all CLI output.
//
// WARNING: should not be used directly, use OZ_CLI_SUCCES, OZ_CLI_WARN,
// OZ_CLI_ERROR macros instead.
void oz__cli_report_message(OzeroCLIReportLevel level,
                            MSVC_FMT_STRING const char *fmt, ...)
    OZ_ATTR_PRINTF(2, 3);

// Report a success message to stdout.
#define OZ_CLI_SUCCESS(fmt, ...)                                               \
    oz__cli_report_message(OZ_CLI_REPORT_LVL_SUCCESS,                          \
                           fmt __VA_OPT__(, ) __VA_ARGS__)

// Give a hint to the user to stderr.
#define OZ_CLI_HINT(fmt, ...)                                                  \
    oz__cli_report_message(OZ_CLI_REPORT_LVL_HINT,                             \
                           fmt __VA_OPT__(, ) __VA_ARGS__)

// Report a warning message to stderr.
#define OZ_CLI_WARN(fmt, ...)                                                  \
    oz__cli_report_message(OZ_CLI_REPORT_LVL_WARNING,                          \
                           fmt __VA_OPT__(, ) __VA_ARGS__)

// Report an error message to stderr.
#define OZ_CLI_ERROR(fmt, ...)                                                 \
    oz__cli_report_message(OZ_CLI_REPORT_LVL_ERROR,                            \
                           fmt __VA_OPT__(, ) __VA_ARGS__)
