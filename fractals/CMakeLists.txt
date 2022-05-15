cmake_minimum_required(VERSION 3.17)
project(fractals)

# output location
if (NOT DEFINED WORKSPACE_LOCATION)
    set(WORKSPACE_LOCATION ".")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WORKSPACE_LOCATION}/bin")
# set(CMAKE_XCODE_SCHEME_WORKING_DIRECTORY "${WORKSPACE_LOCATION}")

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
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
add_executable(${PROJECT_NAME} ${TARGET_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
    spdlog
    mono
)
include_directories(${PROJECT_NAME}
    "${PROJECT_SOURCE_DIR}/src"
    mono
)

if (FRACTALS_RUN_TARGET)
    add_custom_target(run
        COMMAND "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}"
        DEPENDS ${PROJECT_NAME}
        "${WORKSPACE_LOCATION}/bin/${PROJECT_NAME}"
    )
endif()

source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${TARGET_SOURCE_FILES})

#message(${PROJECT_SOURCE_DIR})
#message(${CMAKE_CURRENT_LIST_DIR})
