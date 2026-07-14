#include "cli/commands.h"
#include "cli/report.h"

i32 ozero_cli_cmd_help(const OzeroCLIParsedArgs *args) {
    (void)args;
    OZ_CLI_WARN("command 'help' is not implemented yet");
    return 0;
}
