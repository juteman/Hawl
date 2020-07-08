add_subdirectory(Core)
add_subdirectory(Renderer)
add_executable(IsSubClassOf GC/TestIsSubClass.cc)
target_link_libraries(IsSubClassOf Hawl-Core)