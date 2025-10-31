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
  std::mutex bestGuessesMutex;
  std::array<Guess, BEST_LIST_SIZE> bestGuesses;
  
  std::vector<std::string> guessList;
  std::vector<std::string> solutionList;

  std::vector<std::string> possibleSolutions(const LetterColour& colours, std::vector<std::string>& possibles);
  Guess averageSolutions(const std::string& word);
  void solutionsThread(size_t start, size_t end);
  void findBest();

public:
  Solver();
  Solver(const std::string& guessListPath, const std::string& solutionListPath);
  Solver(const std::vector<std::string>& newGuessList, const std::vector<std::string>& newSolutionList);
  Solver(
    const std::vector<std::string>& newGuessList,
    const std::vector<std::string>& newSolutionList,
    const std::array<Guess, BEST_LIST_SIZE>& newBestGuesses
  );

  void operator=(const Solver& newSolver);

  const std::array<Guess, BEST_LIST_SIZE>& getBestGuess() const;
  void setGuess(const LetterColour& colours);

  const std::vector<std::string>& getPossibleSolutions() const;
  const std::vector<std::string>& getGuessWords() const;
}; // Solver

} // Wordle

#endif // SOLVER_H
