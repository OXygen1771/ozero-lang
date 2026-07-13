#include "terminal/terminal.h"
#include "defines.h"

#include "minunit.h"

#include <stdlib.h>

// windows doesn't like unsetenv, setenv, getenv
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
static i32 test__setenv(const char *name, const char *value) {
    return _putenv_s(name, value);
}

static i32 test__unsetenv(const char *name) { return _putenv_s(name, ""); }
#else
static i32 test__setenv(const char *name, const char *value) {
    return setenv(name, value, 1);
}

static i32 test__unsetenv(const char *name) { return unsetenv(name); }
#endif

MU_TEST(terminal_init_no_env) {
    test__unsetenv("NO_COLOR");
    test__unsetenv("FORCE_COLOR");
    test__unsetenv("TERM");

    // nothing to assert, so just assert it doesn't crash
    oz_term_init();
    mu_assert(true, "Terminal initialized without an error");
}

MU_TEST(terminal_env_no_color) {
    test__setenv("NO_COLOR", "1");
    test__unsetenv("FORCE_COLOR");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, false);
    test__unsetenv("NO_COLOR");
}

MU_TEST(terminal_env_force_color) {
    test__setenv("FORCE_COLOR", "1");
    test__unsetenv("NO_COLOR");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, true);
    test__unsetenv("FORCE_COLOR");
}

MU_TEST(terminal_env_term_dumb) {
    test__setenv("TERM", "dumb");
    test__unsetenv("NO_COLOR");
    test__unsetenv("FORCE_COLOR");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, 0);
    test__unsetenv("TERM");
}

MU_TEST(terminal_env_no_color_preference) {
    test__setenv("NO_COLOR", "1");
    test__setenv("FORCE_COLOR", "1");
    oz_term_init();
    mu_assert_int_eq(oz_global_term_cfg.color_enabled, 0);
    test__unsetenv("TERM");
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
