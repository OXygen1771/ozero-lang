#pragma once

/*
 * Global terminal configuration. Populated by oz_term_init();
 */
typedef struct OzeroTerminalConfig {
    bool color_enabled;
    bool is_tty;
} OzeroTerminalConfig;

extern OzeroTerminalConfig oz_global_term_cfg;

// Initializes OZTerminalConfig based on the environment the interpreter is
// launched in.
void oz_term_init(void);
