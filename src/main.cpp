
#include <chrono>
#include <iostream>

#include "Solver.h"


int main(int argc, char** argv) {
  auto start = std::chrono::system_clock::now();
  Wordle::Solver solver;

  if (argc == 3) {
    std::cout << "Finding best starting words..." << std::endl;
    solver = Wordle::Solver(argv[1], argv[2]);
  }

  auto end = std::chrono::system_clock::now();

  std::string word;
  std::string colourInput;

  while (true) {
    auto solutions = solver.getPossibleSolutions();

    std::cout << "Time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;
  
    std::cout << "Number of Possible Solutions: " << solutions.size() << std::endl;
    if (solutions.size() < 10) {
      for (auto w : solutions) {
        std::cout << w << std::endl;
      }
    }

    std::cout << "Best Guesses:" << std::endl;
    for (int i = 0; i < Wordle::BEST_LIST_SIZE; i++) {
      std::cout << solver.getBestGuess()[i] << std::endl;
      if(solver.getBestGuess()[i].getAverageResult() == 0.0)
        i = Wordle::BEST_LIST_SIZE;
    }

    if (solver.getBestGuess()[0].getAverageResult() == 0.0)
      return 0; 
    
    std::cout << std::endl;
    std::cout << "Guess:   ";
    std::cin >> word;   
    if (word == "x")
      return 0;
    std::cout << "Colours: ";
    std::cin >> colourInput;
    std::cout << std::endl;

    if (word == "x") {
      return 0;
    }

    Wordle::LetterColour colours = word;

    for (int i = 0; i < Wordle::SIZE; i++) {
      if (colourInput[i] == 'b') {
        colours[i] = Wordle::GREY;
      } else if (colourInput[i] == 'y') {
        colours[i] = Wordle::YELLOW;
      } else if (colourInput[i] == 'g') {
        colours[i] = Wordle::GREEN;
      }
    }

    start = std::chrono::system_clock::now();
    solver.setGuess(colours);
    end = std::chrono::system_clock::now();
  }
}
