cmake_minimum_required(VERSION 3.16)

project(algorithms4th LANGUAGES CXX)

add_library(algorithms4th INTERFACE)
target_compile_features(algorithms4th INTERFACE cxx_std_17)
target_include_directories(
    algorithms4th
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

add_executable(union_find_test tests/union_find_test.cpp)
target_link_libraries(union_find_test PRIVATE algorithms4th)

add_test(NAME union_find_test COMMAND union_find_test)
