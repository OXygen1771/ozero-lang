#include "cli/cli_parser.h"

#include "error/error.h"
#include "memory/allocator.h"
#include "memory/system_allocator.h"

#include "minunit.h"

#include <stdlib.h>
#include <string.h>

// helpers
static char **make_argv(const char **args, size_t count) {
    char **argv = (char **)malloc(count * sizeof(char *));
    for (size_t i = 0; i < count; i++) {
        argv[i] =
            (char *)args[i]; // cast away const for compatibility with argv
    }
    return argv;
}

static void free_argv(char **argv) { free((void *)argv); }

// parsing suite

MU_TEST(parse_no_args) {
    const char *args[] = {"ozero"};
    char **argv = make_argv(args, 1);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {false};

    OzeroErrorCode err = ozero_cli_parse(1, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.command == nullptr, "no command should be parsed");

    free_argv(argv);
}

MU_TEST(parse_run_command) {
    const char *args[] = {"ozero", "run"};
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {false};

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.command != nullptr, "run command should be parsed");
    mu_assert_string_eq("run", parsed.command->name);

    free_argv(argv);
}

MU_TEST(parse_check_command) {
    const char *args[] = {"ozero", "check"};
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {false};

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.command != nullptr, "check command should be parsed");
    mu_assert_string_eq("check", parsed.command->name);

    free_argv(argv);
}

MU_TEST(parse_unknown_command) {
    const char *args[] = {"ozero", "runn"}; // typo
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {false};

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);
    mu_assert(parsed.command == nullptr,
              "unknown command should not be parsed");

    free_argv(argv);
}

MU_TEST(parse_all_commands) {
    const char *command_names[] = {"run", "repl",   "check",
                                   "fmt", "create", "help"};
    size_t num_commands = sizeof(command_names) / sizeof(command_names[0]);

    for (size_t i = 0; i < num_commands; i++) {
        const char *args[] = {"ozero", command_names[i]};
        char **argv = make_argv(args, 2);

        OzeroAllocator allocator = oz_alloc_get_system_alloc();
        OzeroCLIParsedArgs parsed = {false};

        OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
        mu_assert_int_eq(OZ_OK, err);
        mu_assert(parsed.command != nullptr, "command should be parsed");
        mu_assert_string_eq(command_names[i], parsed.command->name);

        free_argv(argv);
    }
}

MU_TEST_SUITE(cli_parser_suite) {
    MU_RUN_TEST(parse_no_args);
    MU_RUN_TEST(parse_run_command);
    MU_RUN_TEST(parse_check_command);
    MU_RUN_TEST(parse_unknown_command);
    MU_RUN_TEST(parse_all_commands);
}

int main(void) {
    MU_RUN_SUITE(cli_parser_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
