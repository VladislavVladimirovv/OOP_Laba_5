cmake_minimum_required(VERSION 3.14)
project(DynamicListLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная программа - запуск ./main
add_executable(main
    src/main.cpp
    src/dynamic_memory_resource.cpp
)

target_include_directories(main PRIVATE include)

# Тесты - запуск ./tests
add_executable(tests
    tests/test_dynamic_memory_resource.cpp
    tests/test_dynamic_list.cpp
    tests/test_complex_type.cpp
    src/dynamic_memory_resource.cpp
)

target_include_directories(tests PRIVATE include)

# GTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(tests
    GTest::gtest
    GTest::gtest_main
    pthread
)

include(GoogleTest)
gtest_discover_tests(tests)