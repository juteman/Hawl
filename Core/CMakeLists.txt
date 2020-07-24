cmake_minimum_required(VERSION 3.6) 
project(Hawl-Core CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
include(platform.cmake)

if(PLATFORM_LINUX)
set(INCLUDE 
	include/Linux/CriticalSection.h
	include/Linux/RWLock.h
)
endif()

set(INTERFACE
	interface/BaseType.h
	interface/Common.h
	interface/Timer.h
	interface/IAppBase.h
	interface/SmartPtr/RefCntPtr.h
	interface/SmartPtr/SharedPtr.h
	interface/SmartPtr/ReferenceCounterBase.h
	interface/Thread.h
	interface/Algorithm/LockfreeQueue.h
)

if(PLATFORM_WIN32)
set(SOURCE
	src/Windows/Timer.cc
)
endif()

if(PLATFORM_LINUX)
set(SOURCE
	src/Linux/Timer.cc
)
endif()
add_library(Hawl-Core SHARED ${SOURCE} ${INCLUDE} ${INTERFACE})

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

if(PLATFORM_LINUX)
target_link_libraries(Hawl-Core 
PRIVATE
	BuildSettings
)
endif()


source_group("src" FILES ${SOURCE})
source_group("include" FILES ${INCLUDE})
source_group("interface" FILES ${INTERFACE})