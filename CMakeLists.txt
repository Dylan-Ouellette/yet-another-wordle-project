cmake_minimum_required(VERSION 3.16)

project(yet-another-wordle-project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build the tests from the `test/` directory" FALSE)

link_libraries(pthread)

add_executable(yet-another-wordle-project.exe
  src/main.cpp
  src/Solver.cpp
)

target_compile_options(yet-another-wordle-project.exe
  PRIVATE
  -O3
  -Wall
)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()