cmake_minimum_required(VERSION 3.6) 
project(Hawl-Render CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

###################################################
# Windows Platform Setting
###################################################
if(PLATFORM_WIN32)
add_subdirectory(Windows)
endif()

###################################################
# DirectX12 Build Setting
###################################################
if(D3D12_SUPPORTED)
add_subdirectory(DX12)
endif()
