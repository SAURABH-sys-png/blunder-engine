cmake_minimum_required(VERSION 3.15)
project(blunder-engine LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)


# Define executable
add_executable(blunder-engine
    src/main.cpp
    src/Pawn_Moves.cpp
    src/Knight_Moves.cpp
)

target_include_directories(blunder-engine PRIVATE include)
