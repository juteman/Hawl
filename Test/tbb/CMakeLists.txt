cmake_minimum_required(VERSION 3.6)

project(tbb-test)

add_executable(parallel_for parallel_for.cpp)


target_link_libraries(parallel_for Hawl-Core BuildSettings tbb)