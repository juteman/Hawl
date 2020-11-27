cmake_minimum_required(VERSION 3.6)
project(Hawl-Render CXX)

option(HAWL_BUILD_RENDERDER_SHARED "Enable building share library" ON)

set(INTERFACE
        interface/IRenderer.h
        interface/RenderInfo.h
        interface/Renderer.inl)

if (D3D12_SUPPORTED)
    set(INCLUDE
            include/DX12/DX12Helper.h
            include/DX12/DX12Handle.h
            )

    set(SOURCE
            src/DX12/DX12Device.cc
            src/DX12/DX12CommandQueue.cc
            )
    if (HAWL_BUILD_RENDERDER_SHARED)
        add_library(RendererDX12 ${SOURCE} ${INCLUDE} ${INTERFACE})
        target_compile_definitions(BuildSettings
                INTERFACE
                RENDERER_SHARED=TRUE)
    endif ()
    target_link_libraries(RendererDX12
            PUBLIC
            Hawl-Core
            BuildSettings
            PRIVATE
            "d3d12.lib"
            "dxgi.lib"
            "d3dcompiler.lib"
            )
    target_include_directories(RendererDX12
            PUBLIC
            interface/
            include/
            )
endif ()

if (VULKAN_SUPPORTED)
    find_package(Vulkan REQUIRED FATAL_ERROR)
    set(INCLUDE
            include/Vulkan/VulkanInstance.h
            )

    set(SOURCE
            src/Vulkan/VulkanDevice.cc
            src/Renderer.inl
            src/Vulkan/VulkanInstance.cpp
            )
    if (HAWL_BUILD_RENDERDER_SHARED)
        add_library(RendererVulkan SHARED ${SOURCE} ${INCLUDE} ${INTERFACE})
        target_compile_definitions(BuildSettings
                INTERFACE
                RENDERER_SHARED)
    endif ()
    target_link_libraries(RendererVulkan
            PUBLIC
            Hawl-Core
            BuildSettings
            PRIVATE
            volk
            )

    target_include_directories(RendererVulkan
            PUBLIC
            interface/
            PRIVATE
            include/
            )


endif ()