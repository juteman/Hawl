cmake_minimum_required(VERSION 3.6)
project(Hawl-Render CXX)
set(INTERFACE
        interface/IRenderer.h
    interface/RenderInfo.h)

if (D3D12_SUPPORTED)
    set(INCLUDE
            include/DX12/DX12Helper.h
            include/DX12/DX12Handle.h
            )

    set(SOURCE
            src/DX12/DX12Device.cc
            )

    add_library(RendererDX12 ${SOURCE} ${INCLUDE} ${INTERFACE})
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
            PRIVATE
            include/
            )
endif ()

if (VULKAN_SUPPORTED)
    find_package(Vulkan REQUIRED FATAL_ERROR)
    set(INCLUDE
            )

    set(SOURCE
            src/Vulkan/VulkanDevice.cc
            )

    add_library(RendererVulkan ${SOURCE} ${INCLUDE} ${INTERFACE})
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