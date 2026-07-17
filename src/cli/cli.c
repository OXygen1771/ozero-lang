#include "ozerolang/config.h"

#include "cli.h"

#include "cli_parser.h"
#include "memory/allocator.h"
#include "memory/arena_allocator.h"
#include "report.h"

#include <stdio.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_RUNTIME_ERROR 1
#define EXIT_CLI_ERROR 2

i32 ozero_cli_run(i32 argc, char **argv) {
    // 64 KB should be enough to parse arguments...
    static const size_t arena_size = (size_t)64 * 1024;
    u8 arena_buffer[arena_size];
    OzeroArenaAllocator arena;
    oz_alloc_arena_create(&arena, arena_buffer, sizeof(arena_buffer));
    OzeroAllocator allocator = oz_alloc_arena_allocator(&arena);

    OzeroCLIParsedArgs parsed = {false};
    OzeroErrorCode parse_result =
        ozero_cli_parse(argc, argv, allocator, &parsed);

    if (parse_result != OZ_OK) {
        // parsing error will be printed by ozero_cli_parse()
        oz_alloc_arena_reset(&arena);
        return EXIT_CLI_ERROR;
    }

    if (parsed.global.show_version) {
        OZ_CLI_SUCCESS("OZero interpreter, version %s", OZERO_VERSION);
        oz_alloc_arena_reset(&arena);
        return EXIT_SUCCESS;
    }

    // no command: general help
    if (parsed.command == nullptr) {
        // TODO: print general help
        OZ_CLI_ERROR("no command specified");

        oz_alloc_arena_reset(&arena);
        return EXIT_CLI_ERROR;
    }

    i32 exit_code = parsed.command->handler(&parsed);

    oz_alloc_arena_reset(&arena);
    return exit_code;
}
