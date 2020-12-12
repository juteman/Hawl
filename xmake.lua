set_project("tbox")

-- set xmake minimum version
set_xmakever("2.3.2")

-- set project version
set_version("0.0.1", {build = "%Y%m%d%H%M"})

set_languages("cxx20")


-- add build modes
add_rules("mode.release", "mode.debug", "mode.profile", "mode.coverage")

if is_mode("debug") then
    add_defines("_DEBUG")
    add_defines("DEBUG")
end


