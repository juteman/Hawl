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

add_library(Hawl-Window STATIC ${SOURCE} ${INTERFACE})

target_link_libraries(Hawl-Window
PRIVATE
    Hawl-Core
    BuildSettings
	winmm
)
target_include_directories(Hawl-Window
PUBLIC 
    Windows/interface
)
target_link_options(Hawl-Window PUBLIC /subsystem:windows /ENTRY:mainCRTStartup)
endif()