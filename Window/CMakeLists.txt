cmake_minimum_required(VERSION 3.6)
project(Hawl-Window CXX)



file(GLOB SOURCE_Window "src/*.cpp" "src/*.h" "interface/*.h")

add_library(Hawl-Window ${SOURCE_Window})

target_link_libraries(Hawl-Window
        PUBLIC
        Hawl-Core
        PRIVATE
        BuildSettings
        )



target_include_directories(Hawl-Window
        PUBLIC
        interface
        )