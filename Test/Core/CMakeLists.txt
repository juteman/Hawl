cmake_minimum_required(VERSION 3.6)

project(core-test)
if (PLATFORM_WIN32)
    add_executable(TimerTest Windows/TimerTest.cc)
    add_executable(adapterinfo Windows/AdapterInfo.cc)
    target_link_libraries(adapterinfo RendererDX12)
endif (PLATFORM_WIN32)
if (PLATFORM_LINUX)
    add_executable(SharedCoreTest Linux/SharedCoreTest.cpp)
    target_link_libraries(SharedCoreTest Hawl-Core)
endif (PLATFORM_LINUX)


