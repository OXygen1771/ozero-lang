#pragma once

#include "error/error.h"
#include "memory/allocator.h"

// Represents the type of a command option.
typedef enum OzeroCLIOptionType {
    // -------------------- examples here:
    OZ_CLI_OPT_FLAG,        // --verbose, --debug (i.e. a bool)
    OZ_CLI_OPT_COUNT,       // -v, -vv, -vvv      (i.e. a counter)
    OZ_CLI_OPT_STRING,      // --output <PATH>
    OZ_CLI_OPT_INT,         // -O<level>
    OZ_CLI_OPT_POSITIONAL,  // ozero run file.oz
    OZ_CLI_OPT_PASSTHROUGH, // ozero run file.oz -- --args-for-file-oz
} OzeroCLIOptionType;

// Represents the scope of an option's target field.
typedef enum OzeroCLIOptionScope {
    OZ_CLI_SCOPE_GLOBAL,
    OZ_CLI_SCOPE_COMMAND,
} OzeroCLIOptionScope;

// Represents a single CLI option, global or command-specific.
typedef struct OzeroCLIOption {
    const char *long_name; // Full name without "--" (e.g. verbose).
    char short_name;       // Short name without "-" (e.g. v) or '\0'.

    OzeroCLIOptionType type;
    OzeroCLIOptionScope scope;
    size_t target_offset; // Offset to parsed result of FLAG, COUNT, STRING, INT
                          // options.

    const char *help;       // Description for the `help` command.
    const char *value_name; // "PATH" for --output [PATH], etc.
    bool required;

    const char **allowed_values; // All allowed values for a command. If
                                 // nullptr, everything is allowed, as long as
                                 // it's of the correct type.
    size_t allowed_values_count;
} OzeroCLIOption;

// forward decl
struct OzeroCLIParsedArgs;
// Represents a single CLI command.
typedef struct OzeroCLICommand {
    const char *name;  // e.g. "run"
    const char *help;  // Short description for the command list.
    const char *usage; // "[options] <file.oz> [-- args...]"

    const OzeroCLIOption *options; // Options available for the command.
    size_t option_count;

    // Handler for the command. Should return an exit code.
    i32 (*handler)(const struct OzeroCLIParsedArgs *args);
} OzeroCLICommand;

// Global options available to all commands.
typedef struct OzeroCLIGlobalOptions {
    bool show_version;       // --version
    bool no_color;           // --no-color
    const char *log_file;    // --log <file>
    i32 verbose_level;       // -v, -vv, -vvv --> levels 1, 2, 3
    bool debug;              // --debug
    const char *emit_stage;  // --emit <tokens|ast|...>
    const char *emit_output; // --emit-output <file>
} OzeroCLIGlobalOptions;

// Command-specific options. Does not contain information about positional
// arguments.
typedef union OzeroCLICommandOptions {
    struct {
        u8 _no_opts_yet;
    } run;

    struct {
        u8 _no_opts_yet;
    } check;

    struct {
        bool in_place;
    } fmt;

    struct {
        u8 _no_opts_yet;
    } create_project;

    struct {
        u8 _no_opts_yet;
    } help;

    struct {
        u8 _no_opts_yet;
    } repl; // has no specific options (yet?)
} OzeroCLICommandOptions;

// The range of command-specific arguments in argv.
typedef struct OzeroArgRange {
    // i32 because argc is i32
    i32 start;
    i32 count;
} OzeroArgRange;

// The result of parsing CLI arguments.
typedef struct OzeroCLIParsedArgs {
    OzeroCLIGlobalOptions global;

    const OzeroCLICommand
        *command; // Pointer to matched command (or nullptr, if no match).
    OzeroCLICommandOptions command_opts;

    // for handler context
    i32 argc;
    char **argv;

    // Positional arguments for the current command.
    OzeroArgRange positional_args;

    // Arguments after -- (passed to script)
    OzeroArgRange script_args;
} OzeroCLIParsedArgs;

// Parse argc/argv into structured arguments.
// Uses the provided allocator for string/array allocations, if needed.
// Returns OZ_OK on success, error code on failure.
OzeroErrorCode ozero_cli_parse(i32 argc, char **argv, OzeroAllocator allocator,
                               OzeroCLIParsedArgs *out);

// Free any memory allocated during parsing.
void ozero_cli_parser_free(OzeroCLIParsedArgs *args, OzeroAllocator allocator);
