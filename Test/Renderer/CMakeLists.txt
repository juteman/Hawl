cmake_minimum_required(VERSION 3.6)

project(Renderer-test)
if (PLATFORM_WIN32)
    add_executable(TestBase TestBase.cc)
    target_link_libraries(TestBase
            PUBLIC
            RendererDX12
            BuildSettings
            )
endif ()

