#include "terminal.h"

#include "string/oz_string.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

// _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <windows.h>
#define OZ_IS_TTY(fd) _isatty(_fileno(fd))

static void oz__enable_win_ansi_formatting(void) {
    HANDLE out_handles[2] = {GetStdHandle(STD_OUTPUT_HANDLE),
                             GetStdHandle(STD_ERROR_HANDLE)};

    for (size_t i = 0; i < 2; i++) {
        if (out_handles[i] == INVALID_HANDLE_VALUE) {
            continue;
        }
        DWORD dwMode = 0;
        if (!GetConsoleMode(out_handles[i], &dwMode)) {
            continue;
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(out_handles[i], dwMode);
    }
}
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
#ifdef _WIN32
        if (oz_global_term_cfg.is_tty) {
            oz__enable_win_ansi_formatting();
        }
#endif
        return;
    }

    const char *term_env = getenv("TERM");
    if (term_env != nullptr) {
        const OzeroStringSlice term = oz_string_slice_from_cstr(term_env);
        if (oz_strings_equal(term, oz_string_slice_from_cstr("dumb"))) {
            oz_global_term_cfg.color_enabled = false;
            return;
        }
    }

    // fallback
    oz_global_term_cfg.color_enabled = oz_global_term_cfg.is_tty;
#ifdef _WIN32
    if (oz_global_term_cfg.color_enabled) {
        oz__enable_win_ansi_formatting();
    }

#endif
}
