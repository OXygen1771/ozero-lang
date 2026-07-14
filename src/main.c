#include "cli/cli.h"
#include "terminal/terminal.h"

i32 main(i32 argc, char **argv) {
    oz_term_init();
    const i32 exit_code = ozero_cli_run(argc, argv);
    return exit_code;
}
