macro(ozero_add_unit_test test_filename)
    add_executable(test_${test_filename} unit/${test_filename}.c)
    target_link_libraries(test_${test_filename} PRIVATE ozero_common)
    target_include_directories(test_${test_filename} PRIVATE
        "${CMAKE_SOURCE_DIR}/src/common"
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_SOURCE_DIR}/thirdparty"
    )
    add_test(NAME test_${test_filename} COMMAND test_${test_filename})
endmacro()

ozero_add_unit_test(terminal)
