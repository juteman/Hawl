cmake_minimum_required(VERSION 3.6) 
project(Hawl-Window CXX)
add_definitions(-DUNICODE -D_UNICODE)
set(INTERFACE 
    interface/WinApp.h
)

set(SOURCE
    src/WinApp.cc
)

add_library(Hawl-Window ${SOURCE} ${INTERFACE})

target_link_libraries(Hawl-Window 
PUBLIC
    Hawl-Core
PRIVATE
    BuildSettings
	winmm
)


target_include_directories(Hawl-Window
PUBLIC 
    interface
)