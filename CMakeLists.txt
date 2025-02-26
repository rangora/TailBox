cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME TailBox)
set(CMAKE_CXX_STANDARD 20)

project(${PROJECT_NAME})

# CMake settings
set(DCMAKE_GENERATOR_PLATFORM "x64")

# Variables
# set(SOURCE_ROOT src)

# Sources
file(GLOB_RECURSE MAIN_SOURCE RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

# Solution
foreach(source IN LISTS MAIN_SOURCE)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    string(REPLACE "src" "" source_path_final "${source_path_msvc}")
    source_group("${source_path_final}" FILES "${source}")
endforeach()

add_subdirectory("Engine")
include_directories("Engine")

add_executable(${PROJECT_NAME} ${MAIN_SOURCE})

target_link_libraries(${PROJECT_NAME} PRIVATE Engine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(Engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)