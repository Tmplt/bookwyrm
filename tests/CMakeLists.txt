function(get_test_regex inp output_pass output_fail)
    file(STRINGS ${inp} output_pass_ REGEX "#PASS ")
    file(STRINGS ${inp} output_fail_ REGEX "#FAIL ")

    # Check for number of arguments - should only be one:
    list(LENGTH output_pass_ listcount_pass)
    list(LENGTH output_fail_ listcount_fail)
    if(listcount_pass GREATER 1)
        message(FATAL_ERROR "More than one PASS expressions defined in test ${inp}")
    endif()
    if(listcount_fail GREATER 1)
        message(FATAL_ERROR "More than one FAIL expressions defined in test ${inp}")
    endif()

    # Escape possible regex patterns in the expected output:
    escape_regex("${output_pass_}" output_pass_)
    escape_regex("${output_fail_}" output_fail_)

    set(${output_pass} "${output_pass_}" PARENT_SCOPE)
    set(${output_fail} "${output_fail_}" PARENT_SCOPE)
endfunction()

function(escape_regex inp output)
    # Escape possible regex patterns in the expected output:
    string(REPLACE "#PASS "    ""     _tmp_str "${inp}")
    string(REPLACE "#FAIL "    ""     _tmp_str "${_tmp_str}")
    string(REPLACE "#PASSOSX " ""     _tmp_str "${_tmp_str}")
    string(REPLACE "#FAILOSX " ""     _tmp_str "${_tmp_str}")
    string(REPLACE "\\"        "\\\\" _tmp_str "${_tmp_str}")
    string(REPLACE "?"         "\\?"  _tmp_str "${_tmp_str}")
    string(REPLACE "+"         "\\+"  _tmp_str "${_tmp_str}")
    string(REPLACE "*"         "\\*"  _tmp_str "${_tmp_str}")
    string(REPLACE "("         "\\\(" _tmp_str "${_tmp_str}")
    string(REPLACE ")"         "\\\)" _tmp_str "${_tmp_str}")
    string(REPLACE "["         "\\\[" _tmp_str "${_tmp_str}")
    string(REPLACE "]"         "\\\]" _tmp_str "${_tmp_str}")
    set(${output} "${_tmp_str}" PARENT_SCOPE)
endfunction()

function(add_plugin_handler_test test)
    add_test(NAME ${test}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/src/test_run_plugin.sh" "${CMAKE_BINARY_DIR}/tests/test_run_plugin" "${test}" "${CMAKE_BINARY_DIR}/src/core/bindings/"
    )

    # Parse plugin for pass/fail conditions:
    get_test_regex(${test} expressions_pass expressions_fail)
    if(expressions_pass)
        set_tests_properties(${test} PROPERTIES PASS_REGULAR_EXPRESSION "${expressions_pass}")
    endif()
    if(expressions_fail)
        set_tests_properties(${test} PROPERTIES FAIL_REGULAR_EXPRESSION "${expressions_fail}")
    endif()

endfunction()

######################################
# Plugin handler functionality tests #
######################################

option(test_plugin_handlers "Perform unit tests to ensure functionality of plugin handlers?" ON)

if(test_plugin_handlers)
    # Test detaching working plugin threads
    add_executable(test_detach src/test_detach.cpp)
    target_include_directories(test_detach BEFORE PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_detach bookwyrm-core)
    add_test(NAME "core/thread_detach"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND "${CMAKE_BINARY_DIR}/tests/test_detach" "${CMAKE_CURRENT_SOURCE_DIR}/detach" "${CMAKE_BINARY_DIR}/src/core/bindings/")

    # Test plugin behavior
    add_executable(test_run_plugin src/test_run_plugin.cpp)
    target_include_directories(test_run_plugin BEFORE PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_run_plugin bookwyrm-core)

    file(GLOB test_list_plugins RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} plugins/*)
    list(LENGTH test_list_plugins num_test_plugins)
    # Account for core/thread_detach
    math(EXPR num_test_plugins "${num_test_plugins} + 1")
    # TODO: add one for the detach test
    message(STATUS "Unit tests: ${num_test_plugins} plugin handler tests")
    foreach(test ${test_list_plugins})
        add_plugin_handler_test(${test})
    endforeach()
else()
    message(STATUS "Unit tests: plugin handler tests deactivated.")
endif()
