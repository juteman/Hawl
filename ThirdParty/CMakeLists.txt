
############################################################
# config the assimp
############################################################
# set(ASSIMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assimp" CACHE INTERNAL "location of assimp")
# option(ASSIMP_BUILD_SAMPLES OFF)
# option(ASSIMP_BUILD_TESTS OFF)
# option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
# add_subdirectory(assimp)

############################################################
# config the EASTL
############################################################
#set(EASTL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EASTL" CACHE INTERNAL "location of EASTL")
#add_subdirectory(EASTL)

############################################################
# config the mathfu
############################################################
#set(MATHFU_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mathfu" CACHE INTERNAL "location of mathfu")
#set(MATHFU_DIR_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/mathfu/include"
#   CACHE INTERNAL "location of mathfu head file")
#set(mathfu_build_benchmarks OFF CACHE INTERNAL "")
#set(mathfu_build_tests OFF CACHE INTERNAL "")
#add_subdirectory(mathfu)


############################################################
# config the ozz-animation
############################################################
# set(OZZANIMA "${CMAKE_CURRENT_SOURCE_DIR}/ozz-animation" CACHE INTERNAL "location of ozzanimation")
# set(ozz_build_samples OFF CACHE INTERNAL "")
# set(ozz_build_tools OFF CACHE INTERNAL "")
# set(ozz_build_howtos OFF CACHE INTERNAL "")
# set(ozz_build_msvc_rt_dll  OFF CACHE INTERNAL "")
# set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
# add_subdirectory(ozz-animation)
# set(BUILD_SHARED_LIBS ON CACHE INTERNAL "")

############################################################
# config the tbb lib
############################################################
set(TBB_BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(tbb)

############################################################
# config the spdlog lib for logging system
############################################################
add_subdirectory(spdlog)