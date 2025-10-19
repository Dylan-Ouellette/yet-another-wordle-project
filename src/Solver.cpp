
#include "Solver.h"

#include <fstream>
#include <iostream>

using namespace Wordle;

Solver::Solver(std::string guessListPath, std::string solutionListPath) {
  importWordList(guessListPath, guessList);
  importWordList(solutionListPath, solutionList);
  
  indexLetters(solutionList, letterIndex);

  bestGuess[0] = "tares";
  best[0] = solutionList.size();
}

Solver::~Solver() {}

std::string Solver::getBestGuess() {
  return bestGuess[0];
}

void Solver::setGuess(std::string word, int colours[SIZE]) {
  double tmp;

  solutionList = possibleSolutions(word, colours);
  indexLetters(solutionList, letterIndex);

  for (auto w : guessList) {
    if (best[0] != 0) {
      tmp = averageSolutions(w);

      for (int i = 0; i < 5; i++) {
        if (tmp < best[i]) {
          for (int j = 4; j > i; j--) {
            best[j] = best[j - 1];
            bestGuess[j] = bestGuess[j - 1];
          }

          best[i] = tmp;
          bestGuess[i] = w;
          i = 5;
        }
      }
    }
  }

  for (int i = 0; i < SIZE; i++) {
    std::cout << bestGuess[i] << " --- " << best[i] << std::endl;
  }
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
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < 26; j++)
      letterIndex[i][j].clear();

  for (int i = 0; i < list.size(); i++)
    for (int y = 0; y < SIZE; y++)
      letterIndex[y][list[i][y] - 97].emplace_back(i);
}

bool Solver::checkWord(std::string checkWord, std::string guessWord, int colours[SIZE]) {  
  bool used[SIZE];
  for (int i = 0; i < SIZE; i ++)
    used[i] = false;
  bool found;

  for (int i = 0; i < SIZE; i++) {
    if (colours[i] == YELLOW) {
      if (checkWord[i] == guessWord[i]) {
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
        return false;
      }
    }
  }

  for (int i = 0; i < SIZE; i++) {
    if (colours[i] == GREY) {
      for (int j = 0; j < SIZE; j++) {
        if (!used[j] && colours[j] != GREEN && checkWord[j] == guessWord[i]) {
          return false;
        }
      }
    }
  }

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
        while (index[i] < letterIndex[i][word[i] - 97].size() && letterIndex[i][word[i] - 97][index[i]] < largest) {
          index[i]++;
        }

        if (!(index[i] < letterIndex[i][word[i] - 97].size()))
          return solutions;

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
  int sum;
  double ret = 0;
  bool skip = false;

  for (int i = 0; i < SIZE; i++) {
    colours[i] = GREY;
  }

  while (colours[SIZE - 1] < 3) {
    if (!skip) {
      ret = possibleSolutions(word, colours).size();
      if (ret > 0)
        results.emplace_back(ret);
    }

    skip = false;
    sum = 0;
    colours[0]++;

    for (int k = 0; k < SIZE; k++) {
      if (colours[k] == 3 && k < SIZE - 1) {
        colours[k] = 0;
        colours[k + 1]++;
      } else if (colours[k] == YELLOW) {
        for (int j = 0; j < k; j++) {
          if (colours[j] == GREY && word[j] == word[k])
            skip = true;
        }
      }

      sum += colours[k];
    }

    if (sum == 9)
      skip = true;
  }

  if (ret == 1)
    ret = 0 - (((double)ret) / results.size());
  else
    ret = 0;
  for (int i : results) 
    ret += ((double)i) / results.size();

  return ret;
}
