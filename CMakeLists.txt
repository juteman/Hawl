cmake_minimum_required(VERSION 3.6)
project(Hawl)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
############################################################
# set the ThirdParty 
############################################################
add_subdirectory(ThirdParty)
add_subdirectory(Core)
add_subdirectory(Test)