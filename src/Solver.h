#ifndef SOLVER_H
  #define SOLVER_H

#include <array>
#include <mutex>
#include <string>
#include <vector>

#include "Guess.h"
#include "LetterColour.h"
#include "Wordle.h"

namespace Wordle {

class Solver {

private:
  std::mutex bestMutex;
  std::array<Guess, 5> bestGuesses;
  
  std::vector<std::string> guessList;
  std::vector<std::string> solutionList;

  std::vector<std::string> possibleSolutions(const LetterColour& colours, std::vector<std::string>& possibles);
  Guess averageSolutions(const std::string& word);
  void solutionsThread(size_t start, size_t end);

public:
  Solver(const std::string& guessListPath, const std::string& solutionListPath);
  Solver(const std::vector<std::string>& newGuessList, const std::vector<std::string>& newSolutionList);

  void operator=(const Solver& newSolver);

  Guess getBestGuess(int index = 0);
  void setGuess(const LetterColour& colours);

  const std::vector<std::string>& getPossibleSolutions();
  const std::vector<std::string>& getGuessWords();
}; // Solver

} // Wordle

#endif // SOLVER_H
