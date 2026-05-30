# might add a flag to make it shared, don't see a reason for now
add_library(ozero_common STATIC)

# use POSIX system functions
if(NOT WIN32)
    # POSIX.1-2008 (isatty and fileno)
    add_compile_definitions(_XOPEN_SOURCE=700)
endif()

# modules
set(COMMON_LOG_SOURCES
    log/logger.c
)
set(COMMON_STRING_SOURCES
    string/oz_strings.c
)
set(COMMON_STYLER_SOURCES)
set(COMMON_TERMINAL_SOURCES
    terminal/terminal.c
)

set(COMMON_SOURCES
    ${COMMON_LOG_SOURCES}
    ${COMMON_STRING_SOURCES}
    ${COMMON_STYLER_SOURCES}
    ${COMMON_TERMINAL_SOURCES}
)
target_sources(ozero_common PRIVATE "${COMMON_SOURCES}")
target_include_directories(ozero_common
    PRIVATE
        # to be made public someday
        "${CMAKE_SOURCE_DIR}/include"
        # leave private
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/common"
)
