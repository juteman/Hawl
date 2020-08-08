cmake_minimum_required(VERSION 3.6)

project(core-test)
if (PLATFORM_WIN32)
    add_executable(TimerTest Windows/TimerTest.cc)
    add_executable(adapterinfo Windows/AdapterInfo.cc)
    target_link_libraries(adapterinfo RendererDX12)
endif (PLATFORM_WIN32)
if (PLATFORM_LINUX)
    add_executable(TimerTest Linux/TimerTest.cc)
    add_executable(ThreadTest Linux/ThreadTest.cc)
    target_link_libraries(ThreadTest Hawl-Core)
endif (PLATFORM_LINUX)
    
add_executable(tbb-test tbb/TbbTest.cc)
target_link_libraries(tbb-test tbb)

target_link_libraries(TimerTest Hawl-Core)

