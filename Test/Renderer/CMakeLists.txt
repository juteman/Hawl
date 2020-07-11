cmake_minimum_required(VERSION 3.6)

project(Renderer-test)
add_executable(Window-test TestBase.cc)
target_link_libraries(Window-test
PUBLIC
Hawl-Window
BuildSettings
)
