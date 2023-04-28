cmake_minimum_required(VERSION 3.14) # 3.14 is required for CPM

project(sync CXX)

set(SYNC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SYNC_MAIN_PROJECT ON)
else ()
    set(SYNC_MAIN_PROJECT OFF)
endif()

option(SYNC_BUILD_TESTS "Build the tests" ${SYNC_MAIN_PROJECT})

add_library(sync INTERFACE)
target_include_directories(sync INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET sync PROPERTY CXX_STANDARD 14)
set_property(TARGET sync PROPERTY CXX_STANDARD_REQUIRED ON)
add_library(sync::sync ALIAS sync)

if (SYNC_BUILD_TESTS)
    include(CTest)
    include(cmake/CPM.cmake)
    CPMAddPackage("gh:catchorg/Catch2@3.3.2")

    add_executable(test-mutex test/test_mutex.cpp)
    target_link_libraries(test-mutex PRIVATE sync::sync Catch2::Catch2WithMain)
    add_test(NAME test-mutex COMMAND test-mutex)

    add_executable(test-rwlock test/test_rwlock.cpp)
    target_link_libraries(test-rwlock PRIVATE sync::sync Catch2::Catch2WithMain)
    add_test(NAME test-rwlock COMMAND test-rwlock)
endif ()
