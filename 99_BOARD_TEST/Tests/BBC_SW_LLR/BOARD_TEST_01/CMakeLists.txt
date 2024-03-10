cmake_minimum_required(VERSION 3.20)

if ( TEST_FRAMEWORK STREQUAL Unity )

    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    target_sources(${PROJECT_NAME} PRIVATE BBC_SW_LLR_BOARD_TEST_01.c)

    set(JRUN_CTEST_COMMAND "JRun" --rtt --device STM32F334C8 --if SWD --speed 4000 --usb 661050098 --silent)

    add_test(BBC_SW_LLR_BOARD_TEST_01_01 ${JRUN_CTEST_COMMAND} ${TEST_EXECUTABLE})

    set_tests_properties(BBC_SW_LLR_BOARD_TEST_01_01 PROPERTIES TIMEOUT 5)
    set_tests_properties(BBC_SW_LLR_BOARD_TEST_01_01 PROPERTIES PASS_REGULAR_EXPRESSION "\\*\\*\\* PASSED \\*\\*\\*") 
    set_tests_properties(BBC_SW_LLR_BOARD_TEST_01_01 PROPERTIES FAIL_REGULAR_EXPRESSION "\\*\\*\\* FAILED \\*\\*\\*")

endif()

set(TestList
    # Модуль инициализации АЦП.
    BBC_SW_LLR_BOARD_TEST_01_01
    BBC_SW_LLR_BOARD_TEST_01_02
    BBC_SW_LLR_BOARD_TEST_01_03
    BBC_SW_LLR_BOARD_TEST_01_04
    BBC_SW_LLR_BOARD_TEST_01_05
    BBC_SW_LLR_BOARD_TEST_01_06
    BBC_SW_LLR_BOARD_TEST_01_07
    BBC_SW_LLR_BOARD_TEST_01_08
    BBC_SW_LLR_BOARD_TEST_01_09
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
