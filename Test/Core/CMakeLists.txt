cmake_minimum_required(VERSION 3.6)

project(core-test)
if(PLATFORM_WIN32)
add_executable(TimerTest Windows/TimerTest.cc)
add_executable(RefCntTest Windows/RefCntTest.cc)
endif(PLATFORM_WIN32)

if(PLATFORM_LINUX)
add_executable(TimerTest Linux/TimerTest.cc)
endif(PLATFORM_LINUX)

target_link_libraries(TimerTest Hawl-Core)
target_link_libraries(RefCntTest Hawl-Core)