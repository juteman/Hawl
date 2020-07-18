cmake_minimum_required(VERSION 3.6) 
project(Hawl-Render CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

###################################################
# Windows Platform Setting
###################################################
if(PLATFORM_WIN32)
add_definitions(-DUNICODE -D_UNICODE)
set(INTERFACE 
    Windows/interface/WinApp.h
)

set(SOURCE
    Windows/src/WinApp.cc
)

add_library(Hawl-Window SHARED ${SOURCE} ${INTERFACE})

target_link_libraries(Hawl-Window
PUBLIC
    Hawl-Core
PRIVATE
    BuildSettings
	winmm
)


target_include_directories(Hawl-Window
PUBLIC 
    Windows/interface
)
endif()

###################################################
# DirectX12 Build Setting
###################################################
if(D3D12_SUPPORTED)
set(INTERFACE 
    DX12/interface/Renderer.h
    DX12/interface/DX12Device.h
)

set(SOURCE
    DX12/src/Renderer.cc
)

add_library(RendererDX12 SHARED ${SOURCE} ${INTERFACE})
target_link_libraries(RendererDX12
PUBLIC
    Hawl-Core
PRIVATE
    BuildSettings
)
target_include_directories(RendererDX12
PUBLIC
    DX12/interface
)
endif()
