cmake_minimum_required(VERSION 3.20.0)
project(fractals)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)         # Generate compile_commands.json for ccls
set(CMAKE_SUPPRESS_REGENERATION ON)           # Remove ZERO_CHECK target
set_property(GLOBAL PROPERTY USE_FOLDERS ON)  # Group CMake targets inside a folder

set(WORKSPACE_LOCATION "${PROJECT_SOURCE_DIR}/")

# GLFW configurations
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_UPDATE_MAPPINGS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON)

# FreeType configurations
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

add_subdirectory(deps/spdlog)
add_subdirectory(deps/glm)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glad)
add_subdirectory(deps/freetype)

add_subdirectory(mono)
add_subdirectory(fractals)
