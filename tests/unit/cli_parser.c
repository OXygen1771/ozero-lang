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
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(1, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.command == nullptr, "no command should be parsed");

    free_argv(argv);
}

MU_TEST(parse_run_command) {
    const char *args[] = {"ozero", "run", "file.oz"};
    char **argv = make_argv(args, 3);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(3, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.command != nullptr, "run command should be parsed");
    mu_assert_string_eq("run", parsed.command->name);

    free_argv(argv);
}

MU_TEST(parse_check_command) {
    const char *args[] = {"ozero", "check", "file.oz"};
    char **argv = make_argv(args, 3);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(3, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.command != nullptr, "check command should be parsed");
    mu_assert_string_eq("check", parsed.command->name);

    free_argv(argv);
}

MU_TEST(parse_unknown_command) {
    const char *args[] = {"ozero", "runn"}; // typo
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);
    mu_assert(parsed.command == nullptr,
              "unknown command should not be parsed");

    free_argv(argv);
}

MU_TEST(parse_all_commands_with_required_args) {
    // require 1 positional arg
    const char *req_commands[] = {"run", "check", "fmt", "create"};
    size_t req_count = sizeof(req_commands) / sizeof(req_commands[0]);

    for (size_t i = 0; i < req_count; i++) {
        const char *args[] = {"ozero", req_commands[i], "dummy.oz"};
        char **argv = make_argv(args, 3);

        OzeroAllocator allocator = oz_alloc_get_system_alloc();
        OzeroCLIParsedArgs parsed = {0}; // NOLINT

        OzeroErrorCode err = ozero_cli_parse(3, argv, allocator, &parsed);
        mu_assert_int_eq(OZ_OK, err);
        mu_assert(parsed.command != nullptr, "command should be parsed");
        mu_assert_string_eq(req_commands[i], parsed.command->name);

        free_argv(argv);
    }

    // no required positional args
    const char *opt_commands[] = {"repl", "help"};
    size_t opt_count = sizeof(opt_commands) / sizeof(opt_commands[0]);

    for (size_t i = 0; i < opt_count; i++) {
        const char *args[] = {"ozero", opt_commands[i]};
        char **argv = make_argv(args, 2);

        OzeroAllocator allocator = oz_alloc_get_system_alloc();
        OzeroCLIParsedArgs parsed = {0}; // NOLINT

        OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
        mu_assert_int_eq(OZ_OK, err);
        mu_assert(parsed.command != nullptr, "command should be parsed");
        mu_assert_string_eq(opt_commands[i], parsed.command->name);

        free_argv(argv);
    }
}

MU_TEST_SUITE(parse_basic_suite) {
    MU_RUN_TEST(parse_no_args);
    MU_RUN_TEST(parse_run_command);
    MU_RUN_TEST(parse_check_command);
    MU_RUN_TEST(parse_unknown_command);
    MU_RUN_TEST(parse_all_commands_with_required_args);
}

// global flags suite

MU_TEST(parse_global_flags) {
    const char *args[] = {"ozero", "--version", "--no-color", "--debug"};
    char **argv = make_argv(args, 4);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(4, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert(parsed.global.show_version, "show_version should be true");
    mu_assert(parsed.global.no_color, "no_color should be true");
    mu_assert(parsed.global.debug, "debug should be true");

    free_argv(argv);
}

MU_TEST(parse_global_count_option_1) {
    // -v => verbose_level = 1
    // TODO: add -vvv support
    const char *args[] = {"ozero", "-v", "run", "file.oz"};
    char **argv = make_argv(args, 4);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(4, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_int_eq(1, parsed.global.verbose_level);
    mu_assert(parsed.command != nullptr, "run command should be parsed");

    free_argv(argv);
}

MU_TEST(parse_global_count_option_2_separate) {
    const char *args[] = {"ozero", "-v", "-v", "run", "file.oz"};
    char **argv = make_argv(args, 5);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(5, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_int_eq(2, parsed.global.verbose_level);
    mu_assert(parsed.command != nullptr, "run command should be parsed");

    free_argv(argv);
}

MU_TEST(parse_global_count_option_3_repeated) {
    const char *args[] = {"ozero", "-vvv", "run", "file.oz"};
    char **argv = make_argv(args, 4);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(4, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_int_eq(3, parsed.global.verbose_level);
    mu_assert(parsed.command != nullptr, "run command should be parsed");
}

MU_TEST(parse_global_string_option) {
    const char *args[] = {"ozero", "--log", "ozero.log", "--emit", "tokens"};
    char **argv = make_argv(args, 5);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(5, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_string_eq("ozero.log", parsed.global.log_file);
    mu_assert_string_eq("tokens", parsed.global.emit_stage);

    free_argv(argv);
}

MU_TEST_SUITE(parse_global_flags_suite) {
    MU_RUN_TEST(parse_global_flags);
    MU_RUN_TEST(parse_global_count_option_1);
    MU_RUN_TEST(parse_global_count_option_2_separate);
    MU_RUN_TEST(parse_global_count_option_3_repeated);
    MU_RUN_TEST(parse_global_string_option);
}

// options and positional args suite

MU_TEST(parse_command_options) {
    const char *args[] = {"ozero", "fmt", "-i", "file.oz"};
    char **argv = make_argv(args, 4);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(4, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_string_eq("fmt", parsed.command->name);
    mu_assert(parsed.command_opts.fmt.in_place, "in_place should be true");
    mu_assert_int_eq(1, parsed.positional_args.count);
    mu_assert_string_eq("file.oz", parsed.argv[parsed.positional_args.start]);

    free_argv(argv);
}

MU_TEST(parse_positional_args_multiple) {
    const char *args[] = {"ozero", "run", "file1.oz", "file2.oz", "file3.oz"};
    char **argv = make_argv(args, 5);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(5, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_int_eq(3, parsed.positional_args.count);
    mu_assert_int_eq(2, parsed.positional_args.start);
    mu_assert_string_eq("file1.oz", parsed.argv[parsed.positional_args.start]);
    mu_assert_string_eq("file2.oz",
                        parsed.argv[parsed.positional_args.start + 1]);
    mu_assert_string_eq("file3.oz",
                        parsed.argv[parsed.positional_args.start + 2]);

    free_argv(argv);
}

MU_TEST(parse_passthrough_args) {
    const char *args[] = {"ozero", "run",          "file.oz",
                          "--",    "--script-arg", "-x"};
    char **argv = make_argv(args, 6);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(6, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_OK, err);
    mu_assert_int_eq(1, parsed.positional_args.count);
    mu_assert_int_eq(2, parsed.script_args.count);
    mu_assert_int_eq(4, parsed.script_args.start);
    mu_assert_string_eq("--script-arg", parsed.argv[parsed.script_args.start]);
    mu_assert_string_eq("-x", parsed.argv[parsed.script_args.start + 1]);

    free_argv(argv);
}

MU_TEST_SUITE(parser_opts_and_args_suite) {
    MU_RUN_TEST(parse_command_options);
    MU_RUN_TEST(parse_positional_args_multiple);
    MU_RUN_TEST(parse_passthrough_args);
}

// error suite

MU_TEST(parse_error_missing_required_positional) {
    const char *args[] = {"ozero", "run"}; // run requires a file
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);

    free_argv(argv);
}

MU_TEST(parse_error_unknown_option) {
    const char *args[] = {"ozero", "--unknown-global"};
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);

    free_argv(argv);
}

MU_TEST(parse_error_missing_option_value) {
    const char *args[] = {"ozero", "--log"}; // expects a file path
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);

    free_argv(argv);
}

MU_TEST(parse_error_unsupported_syntax_equals) {
    const char *args[] = {"ozero", "--log=ozero.log"};
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);

    free_argv(argv);
}

MU_TEST(parse_error_unsupported_syntax_clustered) {
    const char *args[] = {"ozero",
                          "-xyz"}; // clustered short flags not supported yet
    char **argv = make_argv(args, 2);

    OzeroAllocator allocator = oz_alloc_get_system_alloc();
    OzeroCLIParsedArgs parsed = {0}; // NOLINT

    OzeroErrorCode err = ozero_cli_parse(2, argv, allocator, &parsed);
    mu_assert_int_eq(OZ_ERR_INVALID_CMD_ARGUMENT, err);

    free_argv(argv);
}

MU_TEST_SUITE(parser_error_suite) {
    MU_RUN_TEST(parse_error_missing_required_positional);
    MU_RUN_TEST(parse_error_unknown_option);
    MU_RUN_TEST(parse_error_missing_option_value);
    MU_RUN_TEST(parse_error_unsupported_syntax_equals);
    MU_RUN_TEST(parse_error_unsupported_syntax_clustered);
}

int main(void) {
    MU_RUN_SUITE(parse_basic_suite);
    MU_RUN_SUITE(parse_global_flags_suite);
    MU_RUN_SUITE(parser_opts_and_args_suite);
    MU_RUN_SUITE(parser_error_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
