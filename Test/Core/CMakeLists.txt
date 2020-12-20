cmake_minimum_required(VERSION 3.6)

project(core-test)
if (PLATFORM_WIN32)
    add_executable(EnumExpanded Windows/EnumExpanded.cpp)
    target_link_libraries(EnumExpanded
            Hawl-Core
            )
endif (PLATFORM_WIN32)
if (PLATFORM_LINUX)
    add_executable(SharedCoreTest Linux/SharedCoreTest.cpp)
    target_link_libraries(SharedCoreTest Hawl-Core)
endif (PLATFORM_LINUX)


