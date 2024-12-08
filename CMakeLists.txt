cmake_minimum_required(VERSION 3.13)
project(shell-cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES src/*.cc include/*.hh)
add_executable(shell_cpp ${SOURCE_FILES})

include_directories("include")

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG        origin/main
# )
# FetchContent_MakeAvailable(googletest)

# enable_testing()

# file(GLOB_RECURSE TEST_FILES src/shell/*.cc tests/*.cc include/*.hh)
# add_executable(shell_cpp_test ${TEST_FILES})

# target_link_libraries(
#   shell_cpp_test
#   GTest::gtest_main
# )

# include(GoogleTest)
# gtest_discover_tests(shell_cpp_test)
