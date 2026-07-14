cmake_minimum_required(VERSION 3.20)

project(OZeroLang
    LANGUAGES C
    VERSION 0.0.1
    DESCRIPTION "A simple interpreted programming language for learning and research."
)
# use C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# out
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

### OPTIONS ###

# Logging/assertions
# NOTE: ERROR and FATAL-level messages are enabled in Release builds by default.
option(OZERO_FORCE_ENABLE_LOGGING "Force enable all logging levels and all assertions, even in Release builds" OFF)
option(OZERO_FORCE_DISABLE_LOGGING "Force disable all logging levels, even ERROR and FATAL. This also disables all assertions" OFF)

option(OZERO_FORCE_DISABLE_ASSERTIONS "Disable all assertions. Some assertions are enabled in Release builds by default" OFF)

configure_file(
    "${CMAKE_SOURCE_DIR}/include/ozerolang/config.h.in"
    "${CMAKE_BINARY_DIR}/include/ozerolang/config.h"
)


### SOURCES ###

# src/
add_subdirectory(src/common)
add_subdirectory(src/cli)
add_subdirectory(src/platform)

option(OZERO_BUILD_TESTS "Build tests for ${PROJECT_NAME}" ON)
if (OZERO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


# the entrypoint
add_executable(ozero)
target_sources(ozero PRIVATE src/main.c)
target_include_directories(ozero
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        # leave private
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/" # platform etc.
        "${CMAKE_SOURCE_DIR}/src/common"
)
target_link_libraries(ozero
    PRIVATE
        ozero_cli
        ozero_common
)
install(TARGETS ozero DESTINATION bin)
