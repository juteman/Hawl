set(INCLUDE
    include/DX12helper.h
)

set(INTERFACE 
    interface/Renderer.h
    interface/DX12Device.h
    interface/DX12Fence.h)

set(SOURCE
    src/Renderer.cc
    src/DX12Device.cc
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