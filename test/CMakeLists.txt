
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(solver-test
  ../src/Solver.cpp
  ../src/LetterColour.cpp
  Solver_Test.cpp
)

target_compile_options(solver-test
  PRIVATE
  -O3
  -Wall
)

target_link_libraries(solver-test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(solver-test)