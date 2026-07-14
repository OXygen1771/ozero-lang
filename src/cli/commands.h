#pragma once

#include "cli_parser.h"

// All registered CLI commands.
extern const OzeroCLICommand OZERO_CLI_COMMANDS[];

// Number of registered commands.
extern const size_t OZERO_CLI_COMMAND_COUNT;

// Find a command by name. Returns nullptr if not found.
const OzeroCLICommand *ozero_cli_find_command(const char *name);

// Find commands similar to `input` via fuzzy matching. Writes matching command
// names into `out_suggestions`. Returns the number of suggestions written, or 0
// if none were found..
size_t ozero_cli_suggest_commands(const char *input,
                                  const char **out_suggestions,
                                  size_t max_suggestions);

// Command handlers, each implemented in commands/

i32 ozero_cli_cmd_run(const OzeroCLIParsedArgs *args);
i32 ozero_cli_cmd_repl(const OzeroCLIParsedArgs *args);
i32 ozero_cli_cmd_check(const OzeroCLIParsedArgs *args);
i32 ozero_cli_cmd_fmt(const OzeroCLIParsedArgs *args);
i32 ozero_cli_cmd_create(const OzeroCLIParsedArgs *args);
i32 ozero_cli_cmd_help(const OzeroCLIParsedArgs *args);
