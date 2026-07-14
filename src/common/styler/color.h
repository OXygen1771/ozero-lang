#pragma once

#include "terminal/terminal.h"

#include <stdio.h>

// All supported text styling options.
typedef enum OzeroTextStyle {
    OZ_STYLE_RESET = 0,

    // attributes
    OZ_STYLE_BOLD,
    OZ_STYLE_DIM,
    OZ_STYLE_ITALIC,
    OZ_STYLE_INVERTED,

    // base colors
    OZ_STYLE_RED,
    OZ_STYLE_GREEN,
    OZ_STYLE_YELLOW,
    OZ_STYLE_BLUE,
    OZ_STYLE_PURPLE,
    OZ_STYLE_CYAN,
    OZ_STYLE_WHITE,

    // bright colors
    OZ_STYLE_BRIGHT_RED,
    OZ_STYLE_BRIGHT_GREEN,
    OZ_STYLE_BRIGHT_YELLOW,
    OZ_STYLE_BRIGHT_BLUE,
    OZ_STYLE_BRIGHT_PURPLE,
    OZ_STYLE_BRIGHT_CYAN,
    OZ_STYLE_BRIGHT_WHITE,

    // bold colors
    OZ_STYLE_BOLD_RED,
    OZ_STYLE_BOLD_GREEN,
    OZ_STYLE_BOLD_YELLOW,
    OZ_STYLE_BOLD_BLUE,
    OZ_STYLE_BOLD_PURPLE,
    OZ_STYLE_BOLD_CYAN,
    OZ_STYLE_BOLD_WHITE,

    // bold + bright colors
    OZ_STYLE_BOLD_BRIGHT_RED,
    OZ_STYLE_BOLD_BRIGHT_GREEN,
    OZ_STYLE_BOLD_BRIGHT_YELLOW,
    OZ_STYLE_BOLD_BRIGHT_BLUE,
    OZ_STYLE_BOLD_BRIGHT_PURPLE,
    OZ_STYLE_BOLD_BRIGHT_CYAN,
    OZ_STYLE_BOLD_BRIGHT_WHITE,

    OZ_STYLE_COUNT,
} OzeroTextStyle;

// Internal mapping from style to ANSI literal. Do not use directly.
static const char *const oz__style_ansi[] = {
    [OZ_STYLE_RESET] = "\033[0m",

    // attributes
    [OZ_STYLE_BOLD] = "\033[1m",
    [OZ_STYLE_DIM] = "\033[2m",
    [OZ_STYLE_ITALIC] = "\033[3m",
    [OZ_STYLE_INVERTED] = "\033[7m",

    // base colors
    [OZ_STYLE_RED] = "\033[31m",
    [OZ_STYLE_GREEN] = "\033[32m",
    [OZ_STYLE_YELLOW] = "\033[33m",
    [OZ_STYLE_BLUE] = "\033[34m",
    [OZ_STYLE_PURPLE] = "\033[35m",
    [OZ_STYLE_CYAN] = "\033[36m",
    [OZ_STYLE_WHITE] = "\033[37m",

    // bright colors
    [OZ_STYLE_BRIGHT_RED] = "\033[91m",
    [OZ_STYLE_BRIGHT_GREEN] = "\033[92m",
    [OZ_STYLE_BRIGHT_YELLOW] = "\033[93m",
    [OZ_STYLE_BRIGHT_BLUE] = "\033[94m",
    [OZ_STYLE_BRIGHT_PURPLE] = "\033[95m",
    [OZ_STYLE_BRIGHT_CYAN] = "\033[96m",
    [OZ_STYLE_BRIGHT_WHITE] = "\033[97m",

    // bold colors
    [OZ_STYLE_BOLD_RED] = "\033[1;31m",
    [OZ_STYLE_BOLD_GREEN] = "\033[1;32m",
    [OZ_STYLE_BOLD_YELLOW] = "\033[1;33m",
    [OZ_STYLE_BOLD_BLUE] = "\033[1;34m",
    [OZ_STYLE_BOLD_PURPLE] = "\033[1;35m",
    [OZ_STYLE_BOLD_CYAN] = "\033[1;36m",
    [OZ_STYLE_BOLD_WHITE] = "\033[1;37m",

    // bold + bright colors
    [OZ_STYLE_BOLD_BRIGHT_RED] = "\033[1;91m",
    [OZ_STYLE_BOLD_BRIGHT_GREEN] = "\033[1;92m",
    [OZ_STYLE_BOLD_BRIGHT_YELLOW] = "\033[1;93m",
    [OZ_STYLE_BOLD_BRIGHT_BLUE] = "\033[1;94m",
    [OZ_STYLE_BOLD_BRIGHT_PURPLE] = "\033[1;95m",
    [OZ_STYLE_BOLD_BRIGHT_CYAN] = "\033[1;96m",
    [OZ_STYLE_BOLD_BRIGHT_WHITE] = "\033[1;97m",
};

// Colorizes (or styles) the text with the given style (provided by
// OzeroTextStyle). Can be used directly, though the intended use is through
// macros defined in color_shorthands.h
static inline size_t oz__put_colorized(FILE *file_stream, size_t text_style_idx,
                                       const char *text) {
    if (!oz_global_term_cfg.color_enabled || text_style_idx == 0) {
        return fputs(text, file_stream);
    }

    // fputs' return value generally shouldn't be discarded, but let's not do it
    // for now...
    fputs(oz__style_ansi[text_style_idx], file_stream); // NOLINT
    size_t text_put = fputs(text, file_stream);
    fputs(oz__style_ansi[OZ_STYLE_RESET], file_stream); // NOLINT

    return text_put;
}
