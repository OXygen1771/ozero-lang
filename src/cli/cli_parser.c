#include "cli_parser.h"
#include "commands.h"
#include "error/error.h"
#include "report.h"

#include "platform/memory.h"

#include <stdio.h>

OzeroErrorCode ozero_cli_parse(const i32 argc, char **argv,
                               const OzeroAllocator allocator,
                               OzeroCLIParsedArgs *out) {
    (void)allocator; // no use for it yet

    // initialize to zero
    oz_platform_mem_zero_memory(out, sizeof(OzeroCLIParsedArgs));

    // no args
    if (argc < 2) {
        out->command = nullptr; // no command -> will show help later
        return OZ_OK;
    }

    const char *command_name = argv[1];
    if (command_name[0] == '-') {
        // TODO: parse global options
        OZ_CLI_WARN("Global options aren't supported yet!");
        OZ_CLI_ERROR("unexpected option: '%s'", command_name);
        return OZ_ERR_INVALID_CMD_ARGUMENT;
    }

    out->command = ozero_cli_find_command(command_name);
    if (out->command == nullptr) {
        OZ_CLI_ERROR("unknown command: '%s'", command_name);

        // 5 should be enough?
        static const size_t max_suggestions = 5;
        const char *suggestions[max_suggestions];
        size_t suggestions_num = ozero_cli_suggest_commands(
            command_name, suggestions, max_suggestions);
        if (suggestions_num > 0) {
            // should be enough
            static const size_t hint_buf_size = 1024;
            char hint_buf[hint_buf_size];
            size_t offset = 0;

            offset += snprintf(hint_buf + offset, sizeof(hint_buf) - offset,
                               "did you mean: ");
            for (size_t i = 0; i < suggestions_num; i++) {
                offset += snprintf(hint_buf + offset, sizeof(hint_buf) - offset,
                                   "'%s'%s", suggestions[i],
                                   (i + 1 < suggestions_num) ? ", " : "?");
            }
            OZ_CLI_HINT("%s", hint_buf);
        }
        return OZ_ERR_INVALID_CMD_ARGUMENT;
    }

    return OZ_OK;
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
