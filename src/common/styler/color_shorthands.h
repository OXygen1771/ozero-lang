#pragma once

// clangd thinks it's unused even though oz__put_colorized() is used
#include "color.h" // IWYU pragma: keep

#define OZ_STYLE(fstream, style, text) oz__put_colorized(fstream, style, text)

#define OZ_BOLD(fstream, text) oz__put_colorized(fstream, OZ_STYLE_BOLD, text)
#define OZ_DIM(fstream, text) oz__put_colorized(fstream, OZ_STYLE_DIM, text)
#define OZ_ITALIC(fstream, text)                                               \
    oz__put_colorized(fstream, OZ_STYLE_ITALIC, text)
#define OZ_INVERTED(fstream, text)                                             \
    oz__put_colorized(fstream, OZ_STYLE_INVERTED, text)

#define OZ_RED(fstream, text) oz__put_colorized(fstream, OZ_STYLE_RED, text)
#define OZ_GREEN(fstream, text) oz__put_colorized(fstream, OZ_STYLE_GREEN, text)
#define OZ_YELLOW(fstream, text)                                               \
    oz__put_colorized(fstream, OZ_STYLE_YELLOW, text)
#define OZ_BLUE(fstream, text) oz__put_colorized(fstream, OZ_STYLE_BLUE, text)
#define OZ_PURPLE(fstream, text)                                               \
    oz__put_colorized(fstream, OZ_STYLE_PURPLE, text)
#define OZ_CYAN(fstream, text) oz__put_colorized(fstream, OZ_STYLE_CYAN, text)
#define OZ_WHITE(fstream, text) oz__put_colorized(fstream, OZ_STYLE_WHITE, text)

#define OZ_BRIGHT_RED(fstream, text)                                           \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_RED, text)
#define OZ_BRIGHT_GREEN(fstream, text)                                         \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_GREEN, text)
#define OZ_BRIGHT_YELLOW(fstream, text)                                        \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_YELLOW, text)
#define OZ_BRIGHT_BLUE(fstream, text)                                          \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_BLUE, text)
#define OZ_BRIGHT_PURPLE(fstream, text)                                        \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_PURPLE, text)
#define OZ_BRIGHT_CYAN(fstream, text)                                          \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_CYAN, text)
#define OZ_BRIGHT_WHITE(fstream, text)                                         \
    oz__put_colorized(fstream, OZ_STYLE_BRIGHT_WHITE, text)
