add_library(ozero_platform STATIC)

if(WIN32)
    set(PLATFORM_SOURCES
        error_win32.c
        memory_win32.c
    )
else()
    # POSIX
    set(PLATFORM_SOURCES
        error_posix.c
        memory_posix.c
    )

    # also POSIX.1-2008 (sysconf)
    add_compile_definitions(_XOPEN_SOURCE=700)
endif()

target_sources(ozero_platform PRIVATE "${PLATFORM_SOURCES}")
target_include_directories(ozero_platform
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/"
        "${CMAKE_SOURCE_DIR}/src/common" # defines.h
)
