# For the Schunk SLS202 project https://schleissheimer.atlassian.net/browse/SCHPSLSC-120
# three executables are defined
#   - Bootloader
#   - GoldeBootloader
#   - Application (tests all the application)

cmake_minimum_required(VERSION 3.30)
project(SLS202_tst VERSION 0.1.0 LANGUAGES C CXX)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS WIN32=${WIN32})
remove_definitions(__GNUC__)

# Define global directory
set(TEST_TARGET_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..)

#define mocks directory
set(GLOBAL_MOCK_DIR ${CMAKE_CURRENT_LIST_DIR}/mock)
set(MOCKED_HEADERS_GLOBAL  ${GLOBAL_MOCK_DIR})

enable_testing()
include(CTest)
include(FetchContent)
set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
    )

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/cmake"
)
include(add_test_runner)

set(TEST_SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR})
set(TEST_SOURCE_DIRECTORY ${CMAKE_BINARY_DIR}/build/tests)

set(TEST_MAIN ${CMAKE_CURRENT_LIST_DIR}/test_main.cpp)
add_subdirectory("Core/Print")

set(TARGET_INCLUDE_DIRECTORY)
set(TARGET_SOURCES  )
set(TEST_SOURCES
  test_foo.cpp
  test_main.cpp
)
set(TEST_MOCKS )

add_test_runner(core_test ${TARGET_INCLUDE_DIRECTORY} ${TARGET_SOURCES} ${TEST_SOURCES} ${TEST_MOCKS})


get_property(_allTargets GLOBAL PROPERTY TARGETS)
message(STATUS "All targets GLOBAL: ${_allTargets}")
