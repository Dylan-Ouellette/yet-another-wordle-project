#ifndef SOLVER_H
  #define SOLVER_H

#include <string>
#include <vector>

#include "Guess.h"
#include "LetterColour.h"
#include "Wordle.h"

namespace Wordle {

class Solver {

private:
  Guess bestGuesses[SIZE];
  
  std::vector<std::string> guessList;
  std::vector<std::string> solutionList;
  std::vector<size_t>letterIndex[SIZE][26];

  std::vector<std::string> possibleSolutions(const LetterColour& colours);
  Guess averageSolutions(const std::string& word);

public:
  Solver(const std::string& guessListPath, const std::string& solutionListPath);
  Solver(const std::vector<std::string>& newGuessList, const std::vector<std::string>& newSolutionList);

  Guess getBestGuess(int index = 0);
  void setGuess(const LetterColour& colours);

  const std::vector<std::string>& getPossibleSolutions();
  const std::vector<std::string>& getGuessWords();
}; // Solver

} // Wordle

#endif // SOLVER_H