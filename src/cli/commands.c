#include "commands.h"
#include "cli/cli_parser.h"

#include <stddef.h>
#include <string.h>

// emit options
static const char *EMIT_STAGES[] = {"tokens", "ast"};
static const size_t EMIT_STAGES_COUNT = 2;

static const OzeroCLIOption GLOBAL_OPTIONS[] = {
    {
        .long_name = "version",
        .short_name = '\0',
        .type = OZ_CLI_OPT_FLAG,
        .scope = OZ_CLI_SCOPE_GLOBAL,
        .target_offset = offsetof(OzeroCLIGlobalOptions, show_version),
        .help = "Show version information",
        .value_name = nullptr,
        .required = false,
    },
    {
        .long_name = "no-color",
        .short_name = '\0',
        .type = OZ_CLI_OPT_FLAG,
        .scope = OZ_CLI_SCOPE_GLOBAL,
        .target_offset = offsetof(OzeroCLIGlobalOptions, no_color),
        .help = "Disable all colored output",
        .value_name = nullptr,
        .required = false,
    },
    {
        .long_name = "log",
        .short_name = '\0',
        .type = OZ_CLI_OPT_STRING,
        .scope = OZ_CLI_SCOPE_GLOBAL,
        .target_offset = offsetof(OzeroCLIGlobalOptions, log_file),
        .help = "Write ozero logs to a file",
        .value_name = "FILE",
        .required = false,
    },
    {
        .long_name = nullptr,
        .short_name = 'v',
        .type = OZ_CLI_OPT_COUNT,
        .scope = OZ_CLI_SCOPE_GLOBAL,
        .target_offset = offsetof(OzeroCLIGlobalOptions, verbose_level),
        .help = "Increase verbosity (repeat to increase more: -v, -vv, -vvv)",
        .value_name = nullptr,
        .required = false,
    },
    {
        .long_name = "debug",
        .short_name = '\0',
        .type = OZ_CLI_OPT_FLAG,
        .scope = OZ_CLI_SCOPE_GLOBAL,
        .target_offset = offsetof(OzeroCLIGlobalOptions, debug),
        .help = "Enable debug mode (maximum verbosity)",
        .value_name = nullptr,
        .required = false,
    },
    {
        .long_name = "emit",
        .short_name = '\0',
        .type = OZ_CLI_OPT_STRING,
        .scope = OZ_CLI_SCOPE_GLOBAL,
        .target_offset = offsetof(OzeroCLIGlobalOptions, emit_stage),
        .help = "Dump various intermediate representations",
        .value_name = "STAGE",
        .required = false,
        .allowed_values = EMIT_STAGES,
        .allowed_values_count = 2,
    },
};

// run
static const OzeroCLIOption RUN_OPTIONS[] = {
    {
        .long_name = nullptr,
        .short_name = '\0',
        .type = OZ_CLI_OPT_POSITIONAL,
        .target_offset = offsetof(OzeroCLICommandArgs, run.files),
        .array_count_offset = offsetof(OzeroCLICommandArgs, run.file_count),
        .help = "Source file(s) to run",
        .value_name = "FILE(s)",
        .required = true,
    },
};

// check
static const OzeroCLIOption CHECK_OPTIONS[] = {
    {
        .long_name = nullptr,
        .short_name = '\0',
        .type = OZ_CLI_OPT_POSITIONAL,
        .scope = OZ_CLI_SCOPE_COMMAND,
        .target_offset = offsetof(OzeroCLICommandArgs, check.files),
        .array_count_offset = offsetof(OzeroCLICommandArgs, check.file_count),
        .help = "Source file(s) to check",
        .value_name = "FILE(s)",
        .required = true,
    },
};

// fmt
static const OzeroCLIOption FMT_OPTIONS[] = {
    {
        .long_name = nullptr,
        .short_name = '\0',
        .type = OZ_CLI_OPT_POSITIONAL,
        .scope = OZ_CLI_SCOPE_COMMAND,
        .target_offset = offsetof(OzeroCLICommandArgs, fmt.files),
        .array_count_offset = offsetof(OzeroCLICommandArgs, fmt.file_count),
        .help = "Source file(s) to format",
        .value_name = "FILE(s)",
        .required = true,
    },
    {
        .long_name = "in-place",
        .short_name = 'i',
        .type = OZ_CLI_OPT_FLAG,
        .scope = OZ_CLI_SCOPE_COMMAND,
        .target_offset = offsetof(OzeroCLICommandArgs, fmt.in_place),
        .help = "Edit file in place instead of printing to stdout",
        .value_name = nullptr,
        .required = false,
    },
};

// create
static const OzeroCLIOption CREATE_OPTIONS[] = {
    {
        .long_name = nullptr,
        .short_name = '\0',
        .type = OZ_CLI_OPT_POSITIONAL,
        .scope = OZ_CLI_SCOPE_COMMAND,
        .target_offset =
            offsetof(OzeroCLICommandArgs, create_project.project_name),
        .help = "Name of the project to create",
        .value_name = "NAME",
        .required = true,
    },
};

// help
static const OzeroCLIOption HELP_OPTIONS[] = {
    {
        .long_name = nullptr,
        .short_name = '\0',
        .type = OZ_CLI_OPT_POSITIONAL,
        .help = "Command to show help for (omit for general help)",
        .scope = OZ_CLI_SCOPE_COMMAND,
        .target_offset = offsetof(OzeroCLICommandArgs, help.command_name),
        .value_name = "COMMAND",
        .required = false,
    },
};

// all commands
const OzeroCLICommand OZERO_CLI_COMMANDS[] = {
    {
        .name = "run",
        .help = "Run an .oz source file (or multiple)",
        .usage =
            "[options] <file1.oz> <file2.oz> ... [-- args for fileN.oz...]",
        .options = RUN_OPTIONS,
        .option_count = sizeof(RUN_OPTIONS) / sizeof(RUN_OPTIONS[0]),
        .handler = ozero_cli_cmd_run,
    },
    {
        .name = "repl",
        .help = "Start the interactive REPL",
        .usage = "",
        .options = nullptr,
        .option_count = 0,
        .handler = ozero_cli_cmd_repl,
    },
    {
        .name = "check",
        .help = "Type-check an .oz source file (or multiple)",
        .usage = "[options] <file1.oz> <file2.oz> ...",
        .options = CHECK_OPTIONS,
        .option_count = sizeof(CHECK_OPTIONS) / sizeof(CHECK_OPTIONS[0]),
        .handler = ozero_cli_cmd_check,
    },
    {
        .name = "fmt",
        .help = "Format an .oz source file (or multiple)",
        .usage = "[options] <file.oz>",
        .options = FMT_OPTIONS,
        .option_count = sizeof(FMT_OPTIONS) / sizeof(FMT_OPTIONS[0]),
        .handler = ozero_cli_cmd_fmt,
    },
    {
        .name = "create",
        .help = "Create a new OZero project",
        .usage = "<name>",
        .options = CREATE_OPTIONS,
        .option_count = sizeof(CREATE_OPTIONS) / sizeof(CREATE_OPTIONS[0]),
        .handler = ozero_cli_cmd_create,
    },
    {
        .name = "help",
        .help = "Show help for a command",
        .usage = "[command]",
        .options = HELP_OPTIONS,
        .option_count = sizeof(HELP_OPTIONS) / sizeof(HELP_OPTIONS[0]),
        .handler = ozero_cli_cmd_help,
    },
};

const size_t OZERO_CLI_COMMAND_COUNT =
    sizeof(OZERO_CLI_COMMANDS) / sizeof(OZERO_CLI_COMMANDS[0]);

const OzeroCLICommand *ozero_cli_find_command(const char *name) {
    if (name == nullptr) {
        return nullptr;
    }

    for (size_t i = 0; i < OZERO_CLI_COMMAND_COUNT; i++) {
        if (strcmp(OZERO_CLI_COMMANDS[i].name, name) == 0) {
            return &OZERO_CLI_COMMANDS[i];
        }
    }

    return nullptr;
}

// max number of differences to not suggest an option
#define OZ_CLI_MAX_SUGGEST_DISTANCE 2
// max length of a single command name (should be fine?)
#define OZ_CLI_MAX_CMD_LEN 32

static size_t oz__fuzzy_match(const char *str_a, const char *str_b) {
    size_t len_a = strlen(str_a);
    size_t len_b = strlen(str_b);

    // quick reject if diff is too big
    if (len_a > len_b + OZ_CLI_MAX_SUGGEST_DISTANCE ||
        len_b > len_a + OZ_CLI_MAX_SUGGEST_DISTANCE) {
        return OZ_CLI_MAX_SUGGEST_DISTANCE + 1;
    }

    // clamp to max cmd len
    if (len_a >= OZ_CLI_MAX_CMD_LEN) {
        len_a = OZ_CLI_MAX_CMD_LEN - 1;
    }
    if (len_b >= OZ_CLI_MAX_CMD_LEN) {
        len_b = OZ_CLI_MAX_CMD_LEN - 1;
    }

    // Levenshtein distance algorithm
    size_t matrix[OZ_CLI_MAX_CMD_LEN][OZ_CLI_MAX_CMD_LEN];

    for (size_t i = 0; i <= len_a; i++) {
        matrix[i][0] = i;
    }
    for (size_t j = 0; j <= len_b; j++) {
        matrix[0][j] = j;
    }

    for (size_t i = 1; i <= len_a; i++) {
        for (size_t j = 1; j <= len_b; j++) {
            size_t cost = (str_a[i - 1] == str_b[j - 1]) ? 0 : 1;
            size_t del = matrix[i - 1][j] + 1;
            size_t ins = matrix[i][j - 1] + 1;
            size_t sub = matrix[i - 1][j - 1] + cost;

            size_t min = del;
            if (ins < min) {
                min = ins;
            }
            if (sub < min) {
                min = sub;
            }
            matrix[i][j] = min;
        }
    }

    return matrix[len_a][len_b];
}

size_t ozero_cli_suggest_commands(const char *input,
                                  const char **out_suggestions,
                                  size_t max_suggestions) {
    if (input == nullptr || out_suggestions == nullptr ||
        max_suggestions == 0) {
        return 0;
    }

    size_t count = 0;

    for (size_t i = 0; i < OZERO_CLI_COMMAND_COUNT; i++) {
        size_t dist = oz__fuzzy_match(input, OZERO_CLI_COMMANDS[i].name);
        if (dist <= OZ_CLI_MAX_SUGGEST_DISTANCE) {
            out_suggestions[count] = OZERO_CLI_COMMANDS[i].name;
            count++;
            if (count >= max_suggestions) {
                break;
            }
        }
    }

    return count;
}
