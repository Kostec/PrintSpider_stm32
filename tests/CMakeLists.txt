include(GoogleTest)

add_executable(core_tests
  test_main.cpp
  test_foo.cpp
)

message(STATUS WIN32=${WIN32})
message(STATUS CMSIS_SIMULATION=${CMSIS_SIMULATION})

if (WIN32)
  target_compile_definitions(core_tests PRIVATE
    GTEST_HAS_PTHREAD=0
  )
endif()

target_link_libraries(core_tests PRIVATE
  gtest_main
  core_lib
)

target_include_directories(core_lib PUBLIC ${CORE_INCLUDES} ${CMAKE_HOME_DIRECTORY}/tests/mock)

message(STATUS "discover_tests for core_tests")
gtest_discover_tests(core_tests
    DISCOVERY_TIMEOUT 10
    PROPERTIES TIMEOUT 5)