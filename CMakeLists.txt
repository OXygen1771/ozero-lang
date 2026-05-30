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

configure_file(
    "${CMAKE_SOURCE_DIR}/include/ozerolang/config.h.in"
    "${CMAKE_BINARY_DIR}/include/ozerolang/config.h"
)


### SOURCES ###

# src/
add_subdirectory(src/common)
add_subdirectory(src/cli)

option(OZERO_BUILD_TESTS "Build tests for ${PROJECT_NAME}" ON)
if (OZERO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
