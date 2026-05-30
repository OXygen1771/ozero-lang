# might add a flag to make it shared, don't see a reason for now
add_library(ozero_common STATIC)

# modules
set(COMMON_IO_SOURCES
    io/logger.c
)

set(COMMON_SOURCES
    ${COMMON_IO_SOURCES}
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
