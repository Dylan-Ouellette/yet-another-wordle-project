
#include <chrono>
#include <iostream>

#include "Solver.h"


int main(int argc, char** argv) {
  Wordle::LetterColour colours[Wordle::SIZE];

  auto start = std::chrono::system_clock::now();
  Wordle::Solver solver("../data/combined_wordlist.txt", "../data/shuffled_real_wordles.txt");
  auto end = std::chrono::system_clock::now();

  std::cout << "constructor time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;

  std::string word;
  std::string colourInput;

  std::cout << "Best Starting Guess --- " << solver.getBestGuess().getWord() << std::endl;
  std::cout << std::endl;

  while (true) {
    std::cout << "Guess:   ";
    std::cin >> word;   
    if (word == "x")
      return 0;
    std::cout << "Colours: ";
    std::cin >> colourInput;
    
    if (word == "x") {
      return 0;
    } else {
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
      solver.setGuess(word, colours);
      end = std::chrono::system_clock::now();

      auto solutions = solver.getPossibleSolutions();

      std::cout << std::endl;
      std::cout << "Time: " << ((std::chrono::duration<double>)(end - start)).count() << "s" << std::endl;
      std::cout << "Number of Possible Solutions: " << solutions.size() << std::endl;
      if (solutions.size() < 10) {
        for (auto w : solutions) {
          std::cout << w << std::endl;
        }
      }
      std::cout << "Best Guesses:" << std::endl;
      for (int i = 0; i < 5; i++) {
        std::cout << solver.getBestGuess(i).getWord() << " - " << solver.getBestGuess(i).getAverageResult() << std::endl;
        if(solver.getBestGuess(i).getAverageResult() == 0.0)
          i = 5;
      }
      std::cout << std::endl;

      if (solver.getBestGuess().getAverageResult() == 0.0)
        return 0;
    }    
  }
}
