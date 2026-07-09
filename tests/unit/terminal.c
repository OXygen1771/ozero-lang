#include "terminal/terminal.h"

#include "minunit.h"

#include <stdlib.h>

MU_TEST(terminal_init_no_env) {
    unsetenv("NO_COLOR");
    unsetenv("FORCE_COLOR");
    unsetenv("TERM");

    // nothing to assert, so just assert it doesn't crash
    oz_term_init();
    mu_assert(true, "Terminal initialized without an error");
}

MU_TEST(terminal_env_no_color) {
    setenv("NO_COLOR", "1", 1);
    unsetenv("FORCE_COLOR");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, false);
    unsetenv("NO_COLOR");
}

MU_TEST(terminal_env_force_color) {
    setenv("FORCE_COLOR", "1", 1);
    unsetenv("NO_COLOR");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, true);
    unsetenv("FORCE_COLOR");
}

MU_TEST(terminal_env_term_dumb) {
    setenv("TERM", "dumb", 1);
    unsetenv("NO_COLOR");
    unsetenv("FORCE_COLOR");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, 0);
    unsetenv("TERM");
}

MU_TEST(terminal_env_no_color_preference) {
    setenv("NO_COLOR", "1", 1);
    setenv("FORCE_COLOR", "1", 1);
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, 0);
    unsetenv("TERM");
}

// Suite
MU_TEST_SUITE(terminal_env_suite) { // NOLINT
    MU_RUN_TEST(terminal_init_no_env);
    MU_RUN_TEST(terminal_env_no_color);
    MU_RUN_TEST(terminal_env_force_color);
    MU_RUN_TEST(terminal_env_term_dumb);
    MU_RUN_TEST(terminal_env_no_color_preference);
}

int main() {
    MU_RUN_SUITE(terminal_env_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
