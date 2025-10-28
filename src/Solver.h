#ifndef SOLVER_H
  #define SOLVER_H

#include "Guess.h"

#include <string>
#include <vector>

namespace Wordle {

constexpr int SIZE = 5;
constexpr int THREADS = 4;

enum LetterColour : int {
  GREY = 0,
  YELLOW = 1,
  GREEN = 2,
};

inline LetterColour& operator++(LetterColour& colour) {
  colour = static_cast<LetterColour>(colour + 1);
  return colour;
}

inline LetterColour operator++(LetterColour& colour, int) {
  LetterColour temp = colour;
  ++colour;
  return temp;
}


class Solver {

private:
  Guess bestGuesses[SIZE];
  
  std::vector<std::string> guessList;
  std::vector<std::string> solutionList;
  std::vector<int> letterIndex[SIZE][26];

  std::vector<std::string> possibleSolutions(const std::string& word, LetterColour colours[SIZE]);
  Guess averageSolutions(const std::string& word);

public:
  Solver(const std::string& guessListPath, const std::string& solutionListPath);
  Solver(const std::vector<std::string>& newGuessList, const std::vector<std::string>& newSolutionList);

  Guess getBestGuess(int index = 0);
  void setGuess(const std::string& word, LetterColour colours[SIZE]);

  const std::vector<std::string>& getPossibleSolutions();
  const std::vector<std::string>& getGuessWords();
}; // Solver

} // Wordle

#endif // SOLVER_H