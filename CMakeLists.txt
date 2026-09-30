cmake_minimum_required(VERSION 3.16)

project(algorithms4th LANGUAGES CXX)

find_package(Threads REQUIRED)

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

add_executable(conv1d_stream_test tests/conv1d_stream_test.cpp)
target_link_libraries(conv1d_stream_test PRIVATE algorithms4th Threads::Threads)

add_test(NAME conv1d_stream_test COMMAND conv1d_stream_test)

add_executable(sort_test tests/sort_test.cpp)
target_link_libraries(sort_test PRIVATE algorithms4th)

add_test(NAME sort_test COMMAND sort_test)

add_executable(hash_map_test tests/hash_map_test.cpp)
target_link_libraries(hash_map_test PRIVATE algorithms4th)
foreach(implementation chaining open_addressing)
    foreach(stage empty basic collisions growth churn random generic rehash scaling)
        add_test(NAME hash_map_${implementation}_${stage}
                 COMMAND hash_map_test ${implementation} ${stage})
        set_tests_properties(hash_map_${implementation}_${stage}
                             PROPERTIES TIMEOUT 10)
    endforeach()
endforeach()
add_test(NAME hash_map_open_addressing_tombstones
         COMMAND hash_map_test open_addressing tombstones)
set_tests_properties(hash_map_open_addressing_tombstones PROPERTIES TIMEOUT 10)
