cmake_minimum_required(VERSION 3.6)

project(core-test)

add_executable(TimerTest TimerTest.cc)
target_link_libraries(TimerTest Hawl-Core)