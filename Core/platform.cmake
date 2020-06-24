set(PLATFORM_WIN32 FALSE CACHE INTERNAL "")
set(PLATFORM_UNIVERSAL_WINDOWS FALSE CACHE INTERNAL "")
set(PLATFORM_ANDROID FALSE CACHE INTERNAL "")
set(PLATFORM_LINUX FALSE CACHE INTERNAL "")
set(PLATFORM_MACOS FALSE CACHE INTERNAL "")
set(PLATFORM_IOS FALSE CACHE INTERNAL "")
set(D3D12_SUPPORTED FALSE CACHE INTERNAL "D3D12 is not spported")
set(VULKAN_SUPPORTED FALSE CACHE INTERNAL "Vulkan is not spported")

if(WIN32)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
        set(PLATFORM_UNIVERSAL_WINDOWS TRUE CACHE INTERNAL "Target platform: Windows Store")
        message("Target platform: Universal Windows. SDK Version: " ${CMAKE_SYSTEM_VERSION})
    else()
        set(PLATFORM_WIN32 TRUE CACHE INTERNAL "Target platform: Win32") #WIN32 is a variable, so we cannot use string "WIN32"
        message("Target platform: Win32. SDK Version: " ${CMAKE_SYSTEM_VERSION})
    endif()
else()
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(PLATFORM_ANDROID TRUE CACHE INTERNAL "Target platform: Android")
        message("Target platform: Android")
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(PLATFORM_LINUX TRUE CACHE INTERNAL "Target platform: Linux")
        message("Target Platform: Linux")
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        if(IOS)
            set(PLATFORM_IOS TRUE CACHE INTERNAL "Target platform: iOS")
            message("Target Platform: iOS")
        else()
            set(PLATFORM_MACOS TRUE CACHE INTERNAL "Target platform: MacOS")
            message("Target Platform: MacOS")
        endif()
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
        set(PLATFORM_IOS TRUE CACHE INTERNAL "Target platform: iOS")
        message("Target Platform: iOS")
    else()
        message(FATAL_ERROR "Unsupported platform")
    endif()
endif(WIN32)

add_library(BuildSettings INTERFACE)

if(PLATFORM_WIN32)
    if(MSVC)
        if(NOT CMAKE_SYSTEM_VERSION STREQUAL "8.1")
            set(D3D12_SUPPORTED TRUE CACHE INTERNAL "D3D12 is supported on Win32 platform")
        endif()
    else()
        message(FATAL_ERROR "Use MSVC on windows!")
        set(D3D12_SUPPORTED FALSE CACHE INTERNAL "D3D12 requires compiling with MSVC")
    endif()
    target_compile_definitions(BuildSettings INTERFACE PLATFORM_WIN32=1)
    elseif(PLATFORM_UNIVERSAL_WINDOWS)
	if(NOT CMAKE_SYSTEM_VERSION STREQUAL "8.1")
        set(D3D12_SUPPORTED TRUE CACHE INTERNAL "D3D12 is supported on Univeral Windows platform")
    else()
        set(D3D11_SUPPORTED TRUE CACHE INTERNAL "D3D11 is supported on Univeral Windows platform")
	endif()
    target_compile_definitions(BuildSettings INTERFACE PLATFORM_UNIVERSAL_WINDOWS=1)
elseif(PLATFORM_ANDROID)
    set(GLES_SUPPORTED TRUE CACHE INTERNAL "OpenGLES is supported on Android platform")
    string(REGEX MATCH [0-9]+ ANDROID_API_LEVEL ${ANDROID_PLATFORM})
    if(${ARCH} EQUAL 64 AND ${ANDROID_API_LEVEL} GREATER 23)
        set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Android platform")
    endif()
    target_compile_definitions(BuildSettings INTERFACE PLATFORM_ANDROID=1)
elseif(PLATFORM_LINUX)
    if(${ARCH} EQUAL 64)
        set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Linux64 platform")
    endif()
    target_compile_definitions(BuildSettings INTERFACE PLATFORM_LINUX=1)
else()
    message(FATAL_ERROR "No PLATFORM_XXX variable defined. Make sure that 'Reforge-Core' folder is processed first")
endif()


if(NOT (${VULKAN_SUPPORTED} OR ${D3D12_SUPPORTED}))
    message(FATAL_ERROR "No rendering backends are select to build")
endif()

message("D3D12_SUPPORTED: " ${D3D12_SUPPORTED})
message("VULKAN_SUPPORTED: " ${VULKAN_SUPPORTED})

target_compile_definitions(BuildSettings 
INTERFACE 
    D3D12_SUPPORTED=$<BOOL:${D3D12_SUPPORTED}>
    VULKAN_SUPPORTED=$<BOOL:${VULKAN_SUPPORTED}>
)


if(MSVC)
    # For msvc, enable level 4 warnings except for
    # - w4100 - unreferenced formal parameter
    # - w4505 - unreferenced local function has been removed
    # - w4201 - nonstandard extension used: nameless struct/union
    target_compile_options(BuildSettings INTERFACE /W4 /wd4100 /wd4505 /wd4201)
    # In all release modes also:
    # - disable w4189 - local variable is initialized but not referenced
    # - Disable RTTI (/GR-)
    # - Enable whole program optimization (/GL)
    # - Enable string pooling (/GF)
    set(MSVC_ALL_RELEASE_COMPILE_OPTIONS /wd4189 /GR- /GL /GF)
    #target_compile_options(Diligent-BuildSettings INTERFACE "$<$<CONFIG:RELEASE>:/wd4189 /Ot")
    # In RELEASE mode:
    # - Set favor fast code option (/Ot)
    # - Enable intrinsic functions (/Oi)
	# - Maximize Speed (/O2)
    # - Inline any suitable function (/Ob2)
    set(MSVC_RELEASE_COMPILE_OPTIONS ${MSVC_ALL_RELEASE_COMPILE_OPTIONS} /Ot /Oi /Ob2 /O2)
    set(MSVC_RELWITHDEBINFO_COMPILE_OPTIONS ${MSVC_RELEASE_COMPILE_OPTIONS})
    # In MINSIZEREL mode set favor small code option (/Os)
    set(MSVC_MINSIZEREL_COMPILE_OPTIONS ${MSVC_ALL_RELEASE_COMPILE_OPTIONS} /Os)
    target_compile_options(BuildSettings INTERFACE "$<$<CONFIG:RELEASE>:${MSVC_RELEASE_COMPILE_OPTIONS}>")
    target_compile_options(BuildSettings INTERFACE "$<$<CONFIG:MINSIZEREL>:${MSVC_MINSIZEREL_COMPILE_OPTIONS}>")
    target_compile_options(BuildSettings INTERFACE "$<$<CONFIG:RELWITHDEBINFO>:${MSVC_RELWITHDEBINFO_COMPILE_OPTIONS}>")
    # !!!NOTE!!! For some reason above is the only form of generator expression that works
    # For instance, this way
    # target_compile_options(Diligent-BuildSettings INTERFACE "$<$<CONFIG:RELEASE>:/Ot>")
    # does not work as expected

    set(DEBUG_MACROS REFORGE_DEVELOPMENT REFORGE_DEBUG)
    target_compile_definitions(BuildSettings INTERFACE "$<$<CONFIG:DEBUG>:${DEBUG_MACROS}>")
else()
    # Todo: use __attribute__((always_inline)), but it needs to be defined in a header file
    target_compile_definitions(BuildSettings INTERFACE __forceinline=inline)

    set(DEBUG_MACROS _DEBUG DEBUG REFORGE_DEVELOPMENT REFORGE_DEBUG)
    set(RELEASE_MACROS NDEBUG)

    foreach(DBG_CONFIG ${DEBUG_CONFIGURATIONS})
        target_compile_definitions(BuildSettings INTERFACE "$<$<CONFIG:${DBG_CONFIG}>:${DEBUG_MACROS}>")
    endforeach()

    foreach(REL_CONFIG ${RELEASE_CONFIGURATIONS})
        target_compile_definitions(BuildSettings INTERFACE "$<$<CONFIG:${REL_CONFIG}>:${RELEASE_MACROS}>")
    endforeach()
endif(MSVC)