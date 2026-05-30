add_executable(ozero)

# modules
set(CLI_SOURCES
    main.c
)
target_sources(ozero PRIVATE "${CLI_SOURCES}")
target_include_directories(ozero
    PRIVATE
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/common"
)
target_link_libraries(ozero
    PRIVATE
        ozero_common
)

install(TARGETS ozero DESTINATION bin)
