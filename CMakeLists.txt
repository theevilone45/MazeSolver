cmake_minimum_required(VERSION 3.20)
project(FunctionalMaze)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GlobalFlags
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wsign-conversion
    -O3
)

add_compile_options(${GlobalFlags})

set(BinName maze)
set(Sources
    src/main.cpp
    src/maze.cpp
    src/config.cpp
    src/renderer.cpp
    src/app.cpp
)
set(Includes
    include
)

include(FetchContent)
include(cmake/sfml.cmake)

add_executable(${BinName} ${Sources})
target_include_directories(${BinName} PRIVATE ${Includes})
target_link_libraries(${BinName} PRIVATE sfml-graphics sfml-window sfml-system)
