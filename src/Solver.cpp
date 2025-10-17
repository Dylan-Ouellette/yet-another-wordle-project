
#include "Solver.h"

#include <fstream>
#include <iostream>

using namespace Wordle;

Solver::Solver(std::string guessListPath, std::string solutionListPath) {
  bestGuess = "aahed";
  
  std::cout << importWordList(guessListPath, guessList);
  std::cout << importWordList(solutionListPath, solutionList);
  std::cout << std::endl;

  indexLetters(solutionList, letterIndex);
}

Solver::~Solver() {

}

std::string Solver::getBestGuess() {
  return guessList[100];
}

void Solver::setGuess(std::string word, int colours[SIZE]) {
  // std::cout << word << " !!! " << checkWord(word, word, colours) << std::endl;
  // std::cout << word << std::endl;
  // std::cout << possibleSolutions(word, colours).size() << std::endl;
  std::cout << word << " --- " << averageSolutions(word) << std::endl;

  // for (auto w : guessList) {
  //   if (averageSolutions(w) != 14855)
  //     std::cout << w << std::endl;
  // }
}

int Solver::importWordList(std::string filepath, std::vector<std::string>& list) {
  std::fstream file(filepath);

  if (!file.is_open()) {
    return 1;
  }

  std::string word;

  while (getline(file, word)) {
    for (auto i = list.begin(); i != list.end(); i++) {
      if (*i.base() > word) {
        list.emplace(i, word);
        i = list.end() - 1;
        word = "";
      }
    }

    if (word != "")
      list.emplace_back(word);
  }

  if (!file.eof()) {
    return -1;
  }

  file.close();

  return 0;
}

void Solver::indexLetters(const std::vector<std::string>& list, std::vector<int> letterIndex[SIZE][26]) {
  for (int i = 0; i < list.size(); i++)
    for (int y = 0; y < SIZE; y++)
      letterIndex[y][list[i][y] - 97].emplace_back(i);
}

bool Solver::checkWord(std::string checkWord, std::string guessWord, int colours[SIZE]) {
  // std::cout << guessWord << " <- check word -> " << checkWord << " --- " << colours[0] << colours[1] << colours[2] << colours[3] << colours[4];
  
  bool used[SIZE];
  bool found;

  for (int i = 0; i < SIZE; i ++)
    used[i] = false;

  for (int i = 0; i < SIZE; i++) {
    if (colours[i] == YELLOW) {
      if (checkWord[i] == guessWord[i]) {
        // std::cout << std::endl;
        return false;
      }

      found = false;

      for (int j = 0; !found && j < SIZE; j++) {
        if (!used[j] && checkWord[j] == guessWord[i] && checkWord[j] != guessWord[j]) {
          used[j] = true;
          found = true;
        }
      }

      if (!found) {
        // std::cout << std::endl;
        return false;
      }
    }
  }

  for (int i = 0; i < SIZE; i++) {
    if (colours[i] == GREY) {
      for (int j = 0; j < SIZE; j++) {
        if (!used[j] && colours[j] != GREEN && checkWord[j] == guessWord[i]) {
          // std::cout << std::endl;
          return false;
        }
      }
    }
  }

  // std::cout << " true" << std::endl;
  return true;
}

std::vector<std::string> Solver::possibleSolutions(std::string word, int colours[SIZE]) {
  std::vector<std::string> solutions;
  int largest = 0;
  bool allLargest;
  int index[SIZE];

  for (int i = 0; i < SIZE; i++)
    index[i] = 0;

  while (true) {
    allLargest = true;

    for (int i = 0; i < SIZE; i++) {
      if (colours[i] == GREEN) {
        while (letterIndex[i][word[i] - 97][index[i]] < largest) {
          if (index[i] < letterIndex[i][word[i] - 97].size() - 1) {
            index[i]++;
          } else {
            return solutions;
          }
        }

        if (letterIndex[i][word[i] - 97][index[i]] > largest) {
          largest = letterIndex[i][word[i] - 97][index[i]];
          allLargest = false;
        }
      }
    }

    if (allLargest) {
      if (checkWord(solutionList[largest], word, colours)) {
        solutions.emplace_back(solutionList[largest]);
      }

      largest++;

      if (largest >= solutionList.size()) {
        return solutions;
      }
    }
  }
}

double Solver::averageSolutions(std::string word) {
  std::vector<int> results;
  int colours[SIZE];
  bool skip;

  for (int i = 0; i < SIZE; i++) {
    colours[i] = GREY;
  }

  std::vector<std::string> copyList; 

  while (colours[SIZE - 1] < 3) {
    skip = false;

    for (int i = 0; i < SIZE; i++) {
      if (colours[i] == YELLOW) {
        for (int j = 0; j < i; j++) {
          if (colours[j] == GREY && word[j] == word[i])
            skip = true;
        }
      }
    }

    if (!skip) {
      std::vector<std::string> res = possibleSolutions(word, colours);

      for (auto w : res) {
        for (int i = 0; i < copyList.size(); i++) {
          if (copyList[i] == w)
            std::cout << "----------- Duplicate " << w << " -----------" << std::endl;
        }

        copyList.emplace_back(w);
      }

      results.emplace_back(res.size());
    }

    colours[0]++;

    for (int k = 0; k < SIZE - 1; k++) {
      if (colours[k] == 3) {
        colours[k] = 0;
        colours[k + 1]++;
      } else {
        k = SIZE;
      }
    }
  }

  double sum = 0;

  for (int i : results) 
    sum += i;

  return sum;
}
