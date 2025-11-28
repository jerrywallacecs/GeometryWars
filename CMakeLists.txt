cmake_minimum_required(VERSION 3.28)
project(GeometryWars LANGUAGES CXX)

# Output settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch SFML
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Fetch ImGui + ImGui-SFML
FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(Imgui)
FetchContent_GetProperties(Imgui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)

FetchContent_Declare(Imgui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG v3.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(Imgui-SFML)

# Source files - automatic discovery
file(GLOB SOURCES 
    CONFIGURE_DEPENDS
    "src/*.cpp"
)

file(GLOB HEADERS
    CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.hpp"
    "src/Core/*.h"
    "src/Core/*.hpp"
    "src/Components/*.h"
    "src/Components/*.hpp"
)

# Executable
add_executable(GeometryWars 
    src/main.cpp
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(GeometryWars PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(GeometryWars PRIVATE
    SFML::Graphics
    ImGui-SFML::ImGui-SFML
)

# handling config file
add_custom_command(
    TARGET GeometryWars POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:GeometryWars>/config"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/config.txt"
            "$<TARGET_FILE_DIR:GeometryWars>/config/config.txt"
    COMMENT "Copying config.txt -> $<TARGET_FILE_DIR:GeometryWars>/config/"
)