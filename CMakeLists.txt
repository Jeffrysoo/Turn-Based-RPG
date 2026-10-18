cmake_minimum_required(VERSION 3.16)
project(TurnRPG C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # lets VS Code find raylib.h (no red lines)

# Downloads and builds raylib automatically the first time
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_executable(rpg
    src/main.cpp
    src/GameUI.cpp
    src/Battle.cpp
    src/Fighter.cpp
    src/TurnQueue.cpp
    src/Utils.cpp
)
target_include_directories(rpg PRIVATE src)
target_link_libraries(rpg PRIVATE raylib)

# Copy the assets folder next to the program after each build (if you have one)
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET rpg POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:rpg>/assets)
endif()