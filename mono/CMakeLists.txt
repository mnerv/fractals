cmake_minimum_required(VERSION 3.12)
project(mono)
# output location
if (NOT DEFINED WORKSPACE_LOCATION)
    set(WORKSPACE_LOCATION ".")
endif()
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/bin")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/lib")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/lib")

# compilation flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Werror")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /wd4201")
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
    spdlog
    glm::glm_static
)
include_directories(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/include/mono"
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
