cmake_minimum_required(VERSION 3.20.0)
project(mono)

# source files and includes
file(GLOB_RECURSE TARGET_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_library(${PROJECT_NAME} STATIC ${TARGET_SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    PRIVATE
    "${PROJECT_SOURCE_DIR}/include/mono"
)
target_link_libraries(${PROJECT_NAME}
    glfw
    glad
    spdlog::spdlog
    glm::glm_static
    freetype
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

