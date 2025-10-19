
#include <chrono>
#include <iostream>

#include "Solver.h"


int main(int argc, char** argv) {
    int colours[5] = {2, 1, 0, 2, 2};
    bool exit = false;

    auto start = std::chrono::system_clock::now();
    Wordle::Solver test("../data/WordList.txt", "../data/shuffled_real_wordles.txt");
    auto end = std::chrono::system_clock::now();

    std::cout << "constructor time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;

    std::string word;
    std::string colourInput;

    while (!exit) {
        std::cout << "Guess:   ";
        std::cin >> word;
        if (word == "x")
            return 0;
        std::cout << "Colours: ";
        std::cin >> colourInput;

        if (word == "x") {
            exit = true;
        } else {
            for (int i = 0; i < 5; i++) {
                if (colourInput[i] == 'b') {
                    colours[i] = 0;
                } else if (colourInput[i] == 'y') {
                    colours[i] = 1;
                } else if (colourInput[i] == 'g') {
                    colours[i] = 2;
                }
            }

            start = std::chrono::system_clock::now();
            test.setGuess(word, colours);
            end = std::chrono::system_clock::now();

            std::cout << "time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;
        }
    }

    return 0;
}
