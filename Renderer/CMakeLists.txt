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
Private
    include/
)
