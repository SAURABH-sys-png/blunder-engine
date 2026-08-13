cmake_minimum_required(VERSION 3.16)

project(BlunderEngine
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(blunder-engine
    src/main.cpp
    src/Board.cpp
    src/Pieces.cpp
)

target_include_directories(blunder-engine PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)