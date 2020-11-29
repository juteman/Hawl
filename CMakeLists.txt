cmake_minimum_required(VERSION 3.16)
project(Hawl)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# set the platform information
include(platform.cmake)
############################################################
# set the ThirdParty 
############################################################
add_subdirectory(ThirdParty)
add_subdirectory(Core)
if(WIN32)
add_subdirectory(Window)
endif()
add_subdirectory(Renderer)

add_subdirectory(Test)