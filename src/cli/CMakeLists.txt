add_library(ozero_cli STATIC)

set(CLI_BASE_SOURCES
    cli.c
    cli_parser.c

    report.c
    commands.c
)
set(CLI_COMMANDS
    commands/check.c
    commands/create.c
    commands/fmt.c
    commands/help.c
    commands/repl.c
    commands/run.c
)

set(CLI_SOURCES
    ${CLI_BASE_SOURCES}
    ${CLI_COMMANDS}
)
target_sources(ozero_cli PRIVATE "${CLI_SOURCES}")
target_include_directories(ozero_cli
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        # leave private
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/" # platform etc.
        "${CMAKE_SOURCE_DIR}/src/common"
)
target_link_libraries(ozero_cli
    PRIVATE
        ozero_common
)
