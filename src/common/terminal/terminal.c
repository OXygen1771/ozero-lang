#include "terminal.h"

#include "string/oz_string.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#define OZ_IS_TTY(fd) _isatty(_fileno(fd))
#else
#include <unistd.h>
#define OZ_IS_TTY(fd) isatty(fileno(fd))
#endif

OzeroTerminalConfig oz_global_term_cfg = {
    .color_enabled = false,
    .is_tty = false,
};

void oz_term_init(void) {
    oz_global_term_cfg.is_tty = (OZ_IS_TTY(stdout) != 0);

    if (getenv("NO_COLOR") != nullptr) {
        oz_global_term_cfg.color_enabled = false;
        return;
    }

    if (getenv("FORCE_COLOR") != nullptr) {
        oz_global_term_cfg.color_enabled = true;
        return;
    }

    OzeroStringSlice term = oz_string_slice_from_cstr(getenv("TERM"));
    if (!oz_string_slice_is_empty(term) &&
        oz_strings_equal(term, oz_string_slice_from_cstr("dumb"))) {
        oz_global_term_cfg.color_enabled = false;
        return;
    }

    // fallback
    oz_global_term_cfg.color_enabled = oz_global_term_cfg.is_tty;
}
