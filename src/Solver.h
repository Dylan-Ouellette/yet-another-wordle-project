#ifndef SOLVER_H
  #define SOLVER_H

#include <string>
#include <thread>
#include <vector>

namespace Wordle {

const int SIZE = 5;
const int GREY = 0;
const int YELLOW = 1;
const int GREEN = 2;

class Solver {

private:
  std::string bestGuess[SIZE];
  double best[SIZE];
  
  std::vector<std::string> guessList;
  std::vector<std::string> solutionList;
  std::vector<int> letterIndex[SIZE][26];

  static int importWordList(std::string filepath, std::vector<std::string>& list);
  static void indexLetters(const std::vector<std::string>& list, std::vector<int> letterIndex[SIZE][26]);

  bool checkWord(std::string checkWord, std::string guessWord, int colours[SIZE]);
  std::vector<std::string> possibleSolutions(std::string word, int colours[SIZE]);
  double averageSolutions(std::string word);

public:
  Solver(std::string guessListPath, std::string solutionListPath);
  ~Solver();

  std::string getBestGuess();
  void setGuess(std::string word, int colours[SIZE]);
};

}

#endif