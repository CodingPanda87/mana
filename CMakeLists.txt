cmake_minimum_required(VERSION 3.20)
project(mana VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(MANA_BUILD_TESTS "Build tests" ON)
option(MANA_BUILD_EXAMPLES "Build examples" ON)

# Library target
add_library(mana SHARED
    src/log.cpp
    src/io.cpp
    src/context.cpp
    src/thread_pool.cpp
    src/event_bus.cpp
    src/framework.cpp
)

target_include_directories(mana PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(mana PRIVATE MANA_EXPORTS)

# Tests
if(MANA_BUILD_TESTS)
    enable_testing()
    add_executable(mana_tests
        tests/test_util.cpp
        tests/test_string.cpp
        tests/test_log.cpp
        tests/test_io.cpp
        tests/test_context.cpp
        tests/test_thread_pool.cpp
        tests/test_event_bus.cpp
        tests/test_framework.cpp
    )
    target_link_libraries(mana_tests PRIVATE mana)
    add_test(NAME mana_tests COMMAND mana_tests)
endif()

# Example
if(MANA_BUILD_EXAMPLES)
    add_executable(mana_example examples/main.cpp)
    target_link_libraries(mana_example PRIVATE mana)
endif()
