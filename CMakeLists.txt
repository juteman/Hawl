cmake_minimum_required(VERSION 3.12)
project(Hawl)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set the platform information
include(platform.cmake)
############################################################
# set the ThirdParty 
############################################################
add_subdirectory(ThirdParty)
add_subdirectory(Core)
add_subdirectory(Window)
add_subdirectory(Renderer)
add_subdirectory(Test)