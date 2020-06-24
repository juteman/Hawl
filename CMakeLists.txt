cmake_minimum_required(VERSION 3.6)
project(Hawl)
enable_testing()
############################################################
# set the ThirdParty 
############################################################
add_subdirectory(ThirdParty)
add_subdirectory(Core)
add_subdirectory(Test)