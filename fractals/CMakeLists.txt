cmake_minimum_required(VERSION 3.20.0)
project(fractals)

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
    freetype
    mono
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
if (NOT MSVC AND NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "-Wall"
        "-Wextra"
        "-Wconversion"
        "-Wpedantic"
        "-Wshadow"
        "-Werror"
    )
    target_link_libraries(${PROJECT_NAME}
        "-lpthread"
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        "/W4"
        "/WX"
        "/wd4201"
        "/wd4189"
    )
endif()

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

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${TARGET_SOURCE_FILES})

