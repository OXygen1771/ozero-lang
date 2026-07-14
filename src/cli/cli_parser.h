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
    size_t target_offset; // Offset to parsed result.

    const char *help;       // Description for the `help` command.
    const char *value_name; // "PATH" for --output [PATH], etc.
    bool required;

    const char **allowed_values; // All allowed values for a command. If
                                 // nullptr, everything is allowed, as long as
                                 // it's of the correct type.
    size_t allowed_values_count;

    // only for POSITIONAL option arrays
    size_t array_count_offset; // offsetof(..., count)
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

// Command-specific arguments.
typedef union OzeroCLICommandArgs {
    struct {
        const char **files;
        size_t file_count;
    } run;

    struct {
        const char **files;
        size_t file_count;
    } check;

    struct {
        const char **files;
        size_t file_count;
        bool in_place;
    } fmt;

    struct {
        const char *project_name;
    } create_project;

    struct {
        const char *command_name; // nullptr for general help
    } help;

    struct {
    } repl; // has no specific options (yet?)
} OzeroCLICommandArgs;

// The result of parsing CLI arguments.
typedef struct OzeroCLIParsedArgs {
    OzeroCLIGlobalOptions global;

    const OzeroCLICommand
        *command; // Pointer to matched command (or nullptr, if no match).
    OzeroCLICommandArgs command_args;

    // Arguments after -- (passed to script)
    const char **script_args;
    size_t script_args_count;
} OzeroCLIParsedArgs;

// Parse argc/argv into structured arguments.
// Uses the provided allocator for string/array allocations, if needed.
// Returns OZ_OK on success, error code on failure.
OzeroErrorCode ozero_cli_parse(i32 argc, char **argv, OzeroAllocator allocator,
                               OzeroCLIParsedArgs *out);

// Free any memory allocated during parsing.
void ozero_cli_parser_free(OzeroCLIParsedArgs *args, OzeroAllocator allocator);
