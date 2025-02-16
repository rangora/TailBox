cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME TailBox)
set(CMAKE_CXX_STANDARD 20)

# CMake settings
set(DCMAKE_GENERATOR_PLATFORM "x64")


# Variables
set(SOURCE_ROOT src)

# Sources
file(GLOB_RECURSE MAIN_SOURCE
    "src/*.c"
    "src/*.h"
    "src/*.cpp"
    "src/*.hpp"
)

# Solution
foreach(source IN LISTS MAIN_SOURCE)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path_msvc}")
    string(REPLACE "src" "" source_path_final "${source_path_msvc}")
    source_group("${source_path_final}" FILES "${source}")
endforeach()