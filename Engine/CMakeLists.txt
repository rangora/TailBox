cmake_minimum_required(VERSION 3.26)

project(Engine LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)
set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "./*.cpp" "./*.c")
file(GLOB HEADERS "./*.hpp" "./*.h")

#include_directories("../")

add_library(Engine SHARED ${SOURCES} ${HEADERS})

target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_compile_definitions(Engine INTERFACE ENGINE_IMPORTS)

target_compile_options(Engine PRIVATE /GR-)

set_target_properties(Engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})