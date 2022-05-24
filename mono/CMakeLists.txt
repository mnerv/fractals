cmake_minimum_required(VERSION 3.12)
project(mono)

# output location
if (NOT DEFINED WORKSPACE_LOCATION)
    set(WORKSPACE_LOCATION ".")
endif()

# compilation flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

# warnings and errors
if (NOT MSVC)
    string(JOIN " " CMAKE_CXX_FLAGS
        "-Wall"
        "-Wextra"
        "-Wpedantic"
        "-Wconversion"
        "-Werror"
    )
else()
    string(JOIN " " CMAKE_CXX_FLAGS
        "/W4"
        "/WX"
        "/wd4201"
        "/wd4189"
    )
endif()

# source files and includes
file(GLOB_RECURSE TARGET_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_library(${PROJECT_NAME} ${TARGET_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME}
    glfw
    glad
    spdlog::spdlog
    glm::glm_static
)
include_directories(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/include/mono"
    glfw
    glad
    spdlog::spdlog
    glm::glm_static
)

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME}
        "-lpthread"
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
