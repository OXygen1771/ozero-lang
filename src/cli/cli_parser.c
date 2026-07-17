#include "cli_parser.h"
#include "commands.h"
#include "error/error.h"
#include "log/logger.h"
#include "log/oz_assertions.h"
#include "report.h"

#include "platform/memory.h"

#include <stdio.h>
#include <string.h>

typedef enum OzeroCLIParserState {
    OZ_CLI_STATE_GLOBAL_OPTS,
    OZ_CLI_STATE_COMMAND_OPTS,
    OZ_CLI_STATE_EXPECT_VALUE,
    OZ_CLI_STATE_PASSTHROUGH,
} OzeroCLIParserState;

// Represents the current context of the CLI parser state machine. Passed to
// parser functions.
typedef struct OzeroCLIParserContext {
    // Global options, obtained from parser caller.
    i32 argc;                 //!< `argc` passed to main().
    char **argv;              //!< `argv` passed to main().
    OzeroAllocator allocator; //!< Allocator to use for temporary allocations.

    i32 current_index; //!< Index of the argument currently being processed.
    OzeroCLIParserState state;
    const OzeroCLIOption *pending_option; //!< Option currently being processed.

    const OzeroCLIOption
        *current_options; //!< Option scope currently being processed.
    size_t current_option_count;

    OzeroCLIParsedArgs *out; //!< Parsing result.
} OzeroCLIParserContext;

// Find a CLI option by its string out of all available options. Accepts both
// long and short options.
// Accepts `*options` - all options available in the
// current context, and the number of options available.
// Returns a pointer to a valid and available OzeroCLIOption or nullptr, if none
// were found. Also sets an out_repeat_count, if the option is of type COUNT.
static const OzeroCLIOption *oz__cli_find_option(const OzeroCLIOption *options,
                                                 size_t count, const char *arg,
                                                 size_t *out_repeat_count);

// Apply a FLAG or COUNT to an option.
// WARNING: performs no option type validation, the caller MUST provide correct
// values.
static void oz__cli_apply_flag(const OzeroCLIOption *opt,
                               OzeroCLIParsedArgs *out, size_t repeat_count);

// Apply an integer or string value to an option that expects an argument.
// TODO: parse integer values.
// WARNING: performs no option type validation, the caller MUST provide correct
// values. NOTE: for string arguments, the options do NOT own the strings. They
// are merely pointing to the string in argv.
// Returns OZ_OK on success, OZ_ERR_INVALID_CMD_ARGUMENT if any parsing errors
// occurred.
static OzeroErrorCode oz__cli_apply_value(const OzeroCLIOption *opt,
                                          OzeroCLIParsedArgs *out,
                                          const char *val);

// Validate that all required options and arguments have been provided.
// Returns OZ_OK on success, OZ_ERR_INVALID_CMD_ARGUMENT if any required
// arguments or options are missing.
static OzeroErrorCode oz__cli_validate_required(OzeroCLIParserContext *ctx);

OzeroErrorCode ozero_cli_parse(const i32 argc, char **argv, // NOLINT
                               const OzeroAllocator allocator,
                               OzeroCLIParsedArgs *out) {
    // initialize to zero
    oz_platform_mem_zero_memory(out, sizeof(OzeroCLIParsedArgs));

    // no args
    if (argc < 2) {
        out->command = nullptr; // no command -> will show help later
        return OZ_OK;
    }

    OzeroCLIParserContext ctx = {
        .argc = argc,
        .argv = argv,
        .allocator = allocator,
        .current_index = 1,
        .state = OZ_CLI_STATE_GLOBAL_OPTS,
        .pending_option = nullptr,
        .current_options = OZERO_CLI_GLOBAL_OPTIONS,
        .current_option_count = OZERO_CLI_GLOBAL_OPTION_COUNT,
        .out = out,
    };
    out->argc = argc;
    out->argv = argv;

    for (; ctx.current_index < argc; ctx.current_index++) {
        const char *arg = argv[ctx.current_index];
        OZ_TRACE("CLI parser: state=%d, arg='%s'", ctx.state, arg);

        switch (ctx.state) {
        case OZ_CLI_STATE_PASSTHROUGH: {
            if (ctx.out->script_args.count == 0) {
                ctx.out->script_args.start = ctx.current_index;
            }
            ctx.out->script_args.count++;
            break;
        }

        case OZ_CLI_STATE_EXPECT_VALUE: {
            OZ_ASSERT(ctx.pending_option != nullptr,
                      "No pending option when expecting a value");
            OzeroErrorCode err =
                oz__cli_apply_value(ctx.pending_option, ctx.out, arg);
            if (err != OZ_OK) {
                return err;
            }
            ctx.state = (ctx.out->command == nullptr)
                            ? OZ_CLI_STATE_GLOBAL_OPTS
                            : OZ_CLI_STATE_COMMAND_OPTS;
            ctx.pending_option = nullptr;
            break;
        }
        case OZ_CLI_STATE_GLOBAL_OPTS:
        case OZ_CLI_STATE_COMMAND_OPTS: {
            // if only `--`, parse passthrough options
            if (strcmp(arg, "--") == 0) {
                ctx.state = OZ_CLI_STATE_PASSTHROUGH;
                break;
            }

            // option
            // arg[1] != '\0' is for `-` as stdin: `cat file.oz | ozero run -`
            if (arg[0] == '-' && arg[1] != '\0') {
                size_t repeat_count = 1;
                const OzeroCLIOption *opt = oz__cli_find_option(
                    ctx.current_options, ctx.current_option_count, arg,
                    &repeat_count);
                if (opt == nullptr) {
                    OZ_CLI_ERROR("unknown or unsupported option: '%s'", arg);
                    return OZ_ERR_INVALID_CMD_ARGUMENT;
                }
                OZ_TRACE("CLI parser: found option '%s', repeated %zu times",
                         opt->long_name != nullptr ? opt->long_name
                                                   : "(short name)",
                         repeat_count);

                if (opt->type == OZ_CLI_OPT_FLAG ||
                    opt->type == OZ_CLI_OPT_COUNT) {
                    oz__cli_apply_flag(opt, ctx.out, repeat_count);
                } else {
                    ctx.pending_option = opt;
                    ctx.state = OZ_CLI_STATE_EXPECT_VALUE;
                }
                break;
            }

            // positional argument
            if (ctx.state == OZ_CLI_STATE_GLOBAL_OPTS) {
                ctx.out->command = ozero_cli_find_command(arg);
                if (ctx.out->command == nullptr) {
                    OZ_CLI_ERROR("unknown command: '%s'", arg);

                    // 5 should be enough?
                    static const size_t max_suggestions = 5;
                    const char *suggestions[max_suggestions];
                    size_t suggestions_num = ozero_cli_suggest_commands(
                        arg, suggestions, max_suggestions);
                    if (suggestions_num > 0) {
                        // should be enough
                        static const size_t hint_buf_size = 1024;
                        char hint_buf[hint_buf_size];
                        size_t offset = 0;

                        offset += snprintf(hint_buf + offset,
                                           sizeof(hint_buf) - offset,
                                           "did you mean: ");
                        for (size_t i = 0; i < suggestions_num; i++) {
                            offset += snprintf(
                                hint_buf + offset, sizeof(hint_buf) - offset,
                                "'%s'%s", suggestions[i],
                                (i + 1 < suggestions_num) ? ", " : "?");
                        }
                        OZ_CLI_HINT("%s", hint_buf);
                    }
                    return OZ_ERR_INVALID_CMD_ARGUMENT;
                }
                ctx.current_options = ctx.out->command->options;
                ctx.current_option_count = ctx.out->command->option_count;
                ctx.state = OZ_CLI_STATE_COMMAND_OPTS;
                OZ_DEBUG("CLI parser: command '%s'", ctx.out->command->name);
            } // if (ctx.state == OZ_CLI_STATE_GLOBAL_OPTS)
            // command-specific options
            else {
                if (ctx.out->positional_args.count == 0) {
                    ctx.out->positional_args.start = ctx.current_index;
                }
                ctx.out->positional_args.count++;
            }
            break;
        }
        }
    }

    if (ctx.state == OZ_CLI_STATE_EXPECT_VALUE) {
        const char *name = ctx.pending_option->long_name != nullptr
                               ? ctx.pending_option->long_name
                               : "(short option)";
        OZ_CLI_ERROR("option '%s' requires a value", name);
        return OZ_ERR_INVALID_CMD_ARGUMENT;
    }

    OZ_DEBUG("CLI parser: got %d positional and %d script arguments",
             ctx.out->positional_args.count, ctx.out->script_args.count);

    return oz__cli_validate_required(&ctx);
}

void ozero_cli_parser_free(OzeroCLIParsedArgs *args,
                           const OzeroAllocator allocator) {
    (void)allocator;

    if (args == nullptr) {
        return;
    }

    // nothing to free yet!
    // but zero it just in case
    oz_platform_mem_zero_memory(args, sizeof(OzeroCLIParsedArgs));
}

static const OzeroCLIOption *
oz__cli_find_option(const OzeroCLIOption *options, // NOLINT
                    const size_t count, const char *arg,
                    size_t *out_repeat_count) {
    // an option MUST start with '-'
    if (arg[0] != '-') {
        return nullptr;
    }

    // long option
    if (arg[1] == '-') {
        const char *name = arg + 2;

        // `--key=value`
        if (strchr(name, '=') != nullptr) {
            // TODO: add support for `--key=value`
            return nullptr;
        }

        for (size_t i = 0; i < count; i++) {
            if (options[i].long_name != nullptr &&
                strcmp(options[i].long_name, name) == 0) {
                return &options[i];
            }
        }
    } // if (arg[1] == '-')
    // short option
    else {
        const size_t len = strlen(arg);

        // `-vvv`, `-xyz`, `-O3`
        if (len > 2) {
            const char letter = arg[1];
            bool same = true;
            for (size_t i = 2; i < len; i++) {
                if (arg[i] != letter) {
                    same = false;
                    break;
                }
            }

            // `-xyz`, `-O3`
            if (!same) {
                // TODO: add support for option clustering and no-space flags
                return nullptr;
            }

            const OzeroCLIOption *opt = nullptr;
            for (size_t i = 0; i < count; i++) {
                if (options[i].short_name == letter) {
                    opt = &options[i];
                    break;
                }
            }

            if (opt != nullptr) {
                if (opt->type == OZ_CLI_OPT_COUNT) {
                    *out_repeat_count = len - 1;
                    return opt;
                }
                if (opt->type == OZ_CLI_OPT_FLAG) {
                    *out_repeat_count = 1;
                    return opt;
                }
                // `-O3`
                return nullptr;
            }

            return nullptr;
        }

        const char short_name = arg[1];
        for (size_t i = 0; i < count; i++) {
            if (options[i].short_name == short_name) {
                return &options[i];
            }
        }
    }

    return nullptr;
}

static void oz__cli_apply_flag(const OzeroCLIOption *opt,
                               OzeroCLIParsedArgs *out,
                               const size_t repeat_count) {
    // magic pointer arithmetic
    // write option value directly to OzeroCLIParsedArgs
    void *base = (opt->scope == OZ_CLI_SCOPE_GLOBAL)
                     ? (void *)&out->global
                     : (void *)&out->command_opts;
    void *target = (u8 *)base + opt->target_offset;

    if (opt->type == OZ_CLI_OPT_FLAG) {
        *(bool *)target = true;
    } else if (opt->type == OZ_CLI_OPT_COUNT) {
        for (size_t i = 0; i < repeat_count; i++) {
            (*(i32 *)target)++;
        }
    }
}

static OzeroErrorCode oz__cli_apply_value(const OzeroCLIOption *opt,
                                          OzeroCLIParsedArgs *out,
                                          const char *val) {
    // magic pointer arithmetic
    // write option value directly to OzeroCLIParsedArgs
    void *base = (opt->scope == OZ_CLI_SCOPE_GLOBAL)
                     ? (void *)&out->global
                     : (void *)&out->command_opts;
    void *target = (u8 *)base + opt->target_offset;

    if (opt->type == OZ_CLI_OPT_STRING) {
        *(const char **)target = val;
    } else if (opt->type == OZ_CLI_OPT_INT) {
        // TODO: parse integer values
        *(i32 *)target = 0;
    }

    return OZ_OK;
}

static OzeroErrorCode oz__cli_validate_required(OzeroCLIParserContext *ctx) {
    // no command => print help, ok
    if (ctx->out->command == nullptr) {
        return OZ_OK;
    }

    for (size_t i = 0; i < ctx->out->command->option_count; i++) {
        const OzeroCLIOption *opt = &ctx->out->command->options[i];
        if (!opt->required) {
            continue;
        }

        if (opt->type == OZ_CLI_OPT_POSITIONAL) {
            if (ctx->out->positional_args.count == 0) {
                // TODO: do proper positional argument validation
                OZ_CLI_ERROR("command '%s' requires %s",
                             ctx->out->command->name,
                             opt->value_name != nullptr ? opt->value_name
                                                        : "an argument");
                return OZ_ERR_INVALID_CMD_ARGUMENT;
            }
        }
    }
    return OZ_OK;
}
