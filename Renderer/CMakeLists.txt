cmake_minimum_required(VERSION 3.6)
project(Hawl-Render CXX)


if (D3D12_SUPPORTED)
    file(GLOB SOURCE_DX12 "src/Vulkan/*.cpp" "src/Vulkan/*.h")

    add_library(RendererDX12 ${SOURCE_DX12})
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
            )
endif ()

if (VULKAN_SUPPORTED)
    find_package(Vulkan REQUIRED FATAL_ERROR)
    file(GLOB SOURCE_VULKAN
            "src/Vulkan/*.cpp" "src/Vulkan/*.h"
            )
    add_library(RendererVulkan ${SOURCE_VULKAN})
    target_link_libraries(RendererVulkan
            PUBLIC
            Hawl-Core
            BuildSettings
            PRIVATE
            volk
            Vulkan-Headers
            )

    target_include_directories(RendererVulkan
            PUBLIC
            interface/
            )
    if (PLATFORM_WIN32)
        set(VULKAN_COMPILE_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR=1)
    endif ()
    target_compile_definitions(RendererVulkan PUBLIC ${VULKAN_COMPILE_DEFINITIONS})
endif ()