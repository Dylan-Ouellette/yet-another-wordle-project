
#include "Solver.h"

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>

using namespace Wordle;

Solver::Solver(std::string guessListPath, std::string solutionListPath) {
  importWordList(guessListPath, guessList);
  importWordList(solutionListPath, solutionList);
  
  indexLetters(solutionList, letterIndex);

  bestGuess[0] = "trace";
  bestGuess[1] = "crate";
  bestGuess[2] = "salet";
  bestGuess[3] = "slate";
  bestGuess[4] = "tarse";

  best[0] = solutionList.size();
}

Solver::~Solver() {}

std::string Solver::getBestGuess() {
  return bestGuess[0];
}

void Solver::setGuess(std::string word, int colours[SIZE]) {
  std::cout << "Average outcome = " << averageSolutions(word) << std::endl;

  solutionList = possibleSolutions(word, colours);
  indexLetters(solutionList, letterIndex);

  std::cout << "Number of possible solutions = " << solutionList.size() << std::endl;

  using namespace std::chrono_literals;

  std::vector<std::future<double>> threadFutures;
  std::vector<std::string> threadWords;
  auto numThreads = std::thread::hardware_concurrency();
  double ret;

  if (numThreads) {
    std::cout << "Hardware Concurrency = " << std::thread::hardware_concurrency() << std::endl;
  } else {
    numThreads = THREADS;
  }

  numThreads = numThreads * 4;

  auto nextWord = guessList.begin();
  while (nextWord != guessList.end() || threadFutures.size() > 0) {
    while (threadFutures.size() < numThreads && nextWord != guessList.end()) {
      threadFutures.emplace_back(std::async(std::launch::async, &Solver::averageSolutions, this, *nextWord.base()));
      threadWords.emplace_back(*nextWord.base());
      nextWord++;
    }

    auto w = threadWords.begin();
    for (auto i = threadFutures.begin(); i != threadFutures.end(); ) {
      if (i.base()->wait_for(0ms) == std::future_status::ready) {
        ret = i.base()->get();

        for (int k = 0; k < 5; k++) {
          if (ret < best[k] || (ret == best[k] && *w.base() < bestGuess[k])) {
            for (int j = 4; j > k; j--) {
              best[j] = best[j - 1];
              bestGuess[j] = bestGuess[j - 1];
            }

            best[k] = ret;
            bestGuess[k] = *w.base();
            k = 5;
          }
        }
        
        w = threadWords.erase(w);
        i = threadFutures.erase(i);
      } else {
        w++;
        i++;
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
