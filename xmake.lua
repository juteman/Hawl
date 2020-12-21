set_project("tbox")

-- set xmake minimum version
set_xmakever("2.3.2")

-- set project version
set_version("0.0.1", {build = "%Y%m%d%H%M"})

set_languages("cxx20")

add_requires("eastl", "spdlog", "mimalloc")
add_requires("conan::tbb/2020.3", {alias = "tbb"})

-- add build modes
add_rules("mode.release", "mode.debug", "mode.profile", "mode.coverage")

if is_mode("debug") then
    add_defines("_DEBUG")
    add_defines("DEBUG")
    add_defines("GRAPHICS_DEBUG")
end

if is_host("windows") then
    set_config("arch", "x86")
end 

if is_plat("windows") then
    add_cxxflags("/W4", "/wd4100", "/wd4505", "/wd4201", {force = true})
    if is_mode("release") then
        add_cxxflags("/wd4189", "/GR-", "/GL", "/GF", {force = true})
    end
end

includes("Core")