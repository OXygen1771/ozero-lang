#pragma once

#include "defines.h"

// Run the OZero CLI. Parses the argument list (argc and argv are provided by
// main()) and returns an exit code.
// Returns:
//      - 0 = success   - 1 = runtime error   - 2 = invalid argument
i32 ozero_cli_run(i32 argc, char **argv);
