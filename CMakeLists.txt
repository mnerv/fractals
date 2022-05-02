cmake_minimum_required(VERSION 3.12)
project(fractals)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(WORKSPACE_LOCATION "${PROJECT_SOURCE_DIR}/")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_UPDATE_MAPPINGS OFF CACHE BOOL "" FORCE)

add_subdirectory(deps/spdlog)
add_subdirectory(deps/glm)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glad)
add_subdirectory(fractals)

