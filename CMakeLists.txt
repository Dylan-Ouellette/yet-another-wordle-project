cmake_minimum_required(VERSION 3.16)

project(yet-another-wordle-project)

link_libraries(pthread)

add_executable(yet-another-wordle-project.exe
    src/main.cpp
    src/Solver.cpp
)

