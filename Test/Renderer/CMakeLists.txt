cmake_minimum_required(VERSION 3.6)

project(Renderer-test)
    add_executable(TestBase TestBase.cc)
    target_link_libraries(TestBase
            PUBLIC
            volk
            RendererVulkan
            BuildSettings
            )

