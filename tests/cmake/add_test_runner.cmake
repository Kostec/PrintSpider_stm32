add_custom_target(test_runner)

function(add_test_runner name)

message(TESTNAME=${name})
message(TARGET_INCLUDE_DIRECTORY=${TARGET_INCLUDE_DIRECTORY})
message(TARGET_SOURCES=${TARGET_SOURCES})
message(TEST_SOURCES=${TEST_SOURCES})
message(TEST_MOCKS=${TEST_MOCKS})

add_executable(${name}
    ${TEST_SOURCES}
    ${TEST_MOCKS}
    ${TARGET_SOURCES}
)

target_include_directories(${name} PUBLIC
    ${MOCKED_HEADERS_GLOBAL}
    ${TARGET_INCLUDE_DIRECTORY}
)

target_link_libraries(${name}
    PUBLIC GTest::gtest_main
)

gtest_discover_tests(${name}
  TEST_PREFIX ${name}.
  WORKING_DIRECTORY $<TARGET_FILE_DIR:${name}>
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${name}>
)

add_dependencies(test_runner ${name})

endfunction()