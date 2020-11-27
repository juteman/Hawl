
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
add_subdirectory(EASTL)

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
# config the tbb lib
############################################################
set(TBB_BUILD_TESTS OFF CACHE INTERNAL "")
add_subdirectory(tbb)

############################################################
# config the spdlog lib for logging system
############################################################
add_subdirectory(spdlog)

############################################################
# config the volk
############################################################
add_subdirectory(volk)

############################################################
# config the Vulkan header
############################################################
add_subdirectory(Vulkan-Headers)