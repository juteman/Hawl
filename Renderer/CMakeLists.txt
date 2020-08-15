cmake_minimum_required(VERSION 3.6) 
project(Hawl-Render CXX)
if(D3D12_SUPPORTED)
set(INCLUDE
    include/DX12/DX12Helper.h
    include/DX12/DX12Handle.h
)
endif()
set(INTERFACE 
    interface/IRenderer.h
    interface/DX12Device.h
    interface/DX12Fence.h)

set(SOURCE
    src/Renderer.cc
    src/DX12Device.cc
    src/DX12Fence.cc
)

add_library(RendererDX12 ${SOURCE} ${INCLUDE} ${INTERFACE})
target_link_libraries(RendererDX12
PUBLIC
    Hawl-Core
PRIVATE
    BuildSettings
    "d3d12.lib"
    "dxgi.lib" 
    "d3dcompiler.lib"
)
target_include_directories(RendererDX12
PUBLIC
    interface/
Private
    include/
)
