
#include <chrono>
#include <iostream>

#include "Solver.h"


int main(int argc, char** argv) {
    int colours[5] = {2, 1, 0, 2, 2};

    auto start = std::chrono::system_clock::now();
    Wordle::Solver test("../WordList.txt", "../WordList.txt");
    auto end = std::chrono::system_clock::now();

    std::cout << "constructor time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;


    start = std::chrono::system_clock::now();
    test.setGuess(test.getBestGuess(), colours);
    end = std::chrono::system_clock::now();

    std::cout << "time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;

    return 0;
}
