cmake_minimum_required(VERSION 3.6) 
project(Hawl-Core CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
include(platform.cmake)

set(INCLUDE 
	include/coreafx.h
)

set(INTERFACE
	interface/BaseType.h
	interface/Common.h
	interface/Timer.h
	interface/RefCntPtr.h
)

if(PLATFORM_WIN32)
set(SOURCE
	src/Windows/Timer.cc
)
endif()

add_library(Hawl-Core STATIC ${SOURCE} ${INCLUDE} ${INTERFACE})

target_include_directories(Hawl-Core 
PRIVATE
    include
PUBLIC 
    interface
)

if(PLATFORM_WIN32)
target_link_libraries(Hawl-Core 
PRIVATE
	BuildSettings
	winmm
)
endif()


source_group("src" FILES ${SOURCE})
source_group("include" FILES ${INCLUDE})
source_group("interface" FILES ${INTERFACE})