#include "report.h"

#include "styler/color.h"
#include "styler/color_shorthands.h"

#include <stdarg.h>
#include <stdio.h>

void oz__cli_report_message(OzeroCLIReportLevel level, const char *fmt, ...) {
    FILE *out_fstream = (level <= OZ_CLI_REPORT_LVL_SUCCESS) ? stdout : stderr;

    const char *label = nullptr;
    OzeroTextStyle style = OZ_STYLE_RESET;

    switch (level) {
    case OZ_CLI_REPORT_LVL_SUCCESS:
        label = "success";
        style = OZ_STYLE_BOLD_BRIGHT_GREEN;
        break;
    case OZ_CLI_REPORT_LVL_HINT:
        label = "hint";
        style = OZ_STYLE_BOLD_BRIGHT_CYAN;
        break;
    case OZ_CLI_REPORT_LVL_WARNING:
        label = "warning";
        style = OZ_STYLE_BOLD_BRIGHT_YELLOW;
        break;
    case OZ_CLI_REPORT_LVL_ERROR:
        label = "error";
        style = OZ_STYLE_BOLD_BRIGHT_RED;
        break;
    }

    OZ_STYLE(out_fstream, style, label);
    fprintf(out_fstream, ": ");

    // Выводим сообщение пользователя
    va_list args;
    va_start(args, fmt);
    vfprintf(out_fstream, fmt, args);
    va_end(args);

    fputc('\n', out_fstream);
}
