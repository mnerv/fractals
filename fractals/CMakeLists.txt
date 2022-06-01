cmake_minimum_required(VERSION 3.20.0)
project(fractals)

# Output location
if (NOT WORKSPACE_LOCATION)
    set(WORKSPACE_LOCATION ".")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}")

# Compilation flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

# Warnings and Errors
if (NOT MSVC)
    string(JOIN " " CMAKE_CXX_FLAGS
        ${CMAKE_CXX_FLAGS}
        "-Wall"
        "-Wextra"
        "-Wpedantic"
        "-Wconversion"
        "-Werror"
    )
else()
    string(JOIN " " CMAKE_CXX_FLAGS
        ${CMAKE_CXX_FLAGS}
        "/W4"
        "/WX"
        "/wd4201"
        "/wd4189"
    )
endif()

# Source files, include directories and library links
file(GLOB_RECURSE TARGET_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_executable(${PROJECT_NAME} ${TARGET_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
    glfw
    glad
    spdlog::spdlog
    glm::glm_static
    mono
)
target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    glfw
    glad
    spdlog::spdlog
    glm::glm_static
    mono
)

if (APPLE)
    target_link_libraries(${PROJECT_NAME}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME}
        "-dl"
        "-m"
        "-GL"
        "-X11"
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME}
        "OpenGL32.lib"
    )
endif()

if (FRACTALS_RUN_TARGET)
    add_custom_target(run
        COMMAND "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}"
        DEPENDS ${PROJECT_NAME}
        "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}"
    )
endif()

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${TARGET_SOURCE_FILES})

