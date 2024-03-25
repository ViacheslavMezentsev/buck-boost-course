cmake_minimum_required(VERSION 3.20)

set(TestList
    BBC_SW_LLR_BOARD_TEST_10_01
    BBC_SW_LLR_BOARD_TEST_10_02
)

if ( TEST_SCRIPT_TYPE STREQUAL GDB )

    foreach (test ${TestList})
        add_test(${test} ${GDB_TEST_RUNNER} ${TEST_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${test} ".gdb")    
    endforeach()

    set_tests_properties(${TestList} PROPERTIES TIMEOUT 5)

elseif ( TEST_SCRIPT_TYPE STREQUAL GDB-PY)

    foreach (test ${TestList})
        add_test(${test} ${GDB_TEST_RUNNER} ${TEST_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${test} ".py")    
    endforeach()

    set_tests_properties(${TestList} PROPERTIES TIMEOUT 5)

endif()
