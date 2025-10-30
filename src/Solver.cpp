
#include "Solver.h"

#include <algorithm>
#include <fstream>
#include <thread>

using namespace Wordle;


int importWordList(std::string filepath, std::vector<std::string>& list) {
  std::fstream file(filepath);

  if (!file.is_open()) {
    return 1;
  }

  std::string word;

  while (getline(file, word)) {
    list.emplace_back(word);
  }

  if (!file.eof()) {
    return 2;
  }

  file.close();

  std::sort(list.begin(), list.end());

  return 0;
}


void indexLetters(const std::vector<std::string>& list, std::vector<size_t> letterIndex[SIZE][26]) {
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < 26; j++)
      letterIndex[i][j].clear();

  for (size_t i = 0; i < list.size(); i++)
    for (int y = 0; y < SIZE; y++)
      letterIndex[y][list[i][y] - 97].emplace_back(i);
}


Solver::Solver(const std::string& guessListPath, const std::string& solutionListPath) {
  importWordList(guessListPath, guessList);
  importWordList(solutionListPath, solutionList);
  
  indexLetters(solutionList, letterIndex);

  bestGuesses[0] = Guess("trace", solutionList.size());
  bestGuesses[1] = Guess("crate", solutionList.size());
  bestGuesses[2] = Guess("salet", solutionList.size());
  bestGuesses[3] = Guess("slate", solutionList.size());
  bestGuesses[4] = Guess("tarse", solutionList.size());
}


Solver::Solver(const std::vector<std::string>& newGuessList, const std::vector<std::string>& newSolutionList) : 
    guessList(newGuessList),
    solutionList(newSolutionList) {

  indexLetters(solutionList, letterIndex);

  bestGuesses[0] = Guess("trace", solutionList.size());
  bestGuesses[1] = Guess("crate", solutionList.size());
  bestGuesses[2] = Guess("salet", solutionList.size());
  bestGuesses[3] = Guess("slate", solutionList.size());
  bestGuesses[4] = Guess("tarse", solutionList.size());
}


void Solver::operator=(const Solver& newSolver) {
  guessList = newSolver.guessList;
  solutionList = newSolver.solutionList;
  bestGuesses = newSolver.bestGuesses;
  for (int i = 0; i < SIZE; i ++) {
    for (int j = 0; j < 26; j ++) {
      letterIndex[i][j] = newSolver.letterIndex[i][j];
    }
  }
}


const std::vector<std::string>& Solver::getPossibleSolutions() {
  return solutionList;
}


const std::vector<std::string>& Solver::getGuessWords() {
  return guessList;
}


Guess Solver::getBestGuess(int index) {
  return bestGuesses[index];
}


void Solver::setGuess(const LetterColour& colours) {
  solutionList = possibleSolutions(colours);
  indexLetters(solutionList, letterIndex);

  std::vector<std::thread> threads;
  auto numThreads = std::thread::hardware_concurrency();
  Guess ret;

  if (!numThreads) {
    numThreads = THREADS;
  }

  size_t threadWordNum = guessList.size() / numThreads;

  for (unsigned int i = 0; i < numThreads - 1; i++) {
    threads.emplace_back(std::thread(&Solver::solutionsThread, this, threadWordNum * i, threadWordNum * (i + 1)));
  }

  threads.emplace_back(std::thread(&Solver::solutionsThread, this, threadWordNum * (numThreads - 1), guessList.size()));

  for (unsigned int i = 0; i < numThreads; i++) {
    threads[i].join();
  }
}


void Solver::solutionsThread(size_t start, size_t end) {
  Guess best[5];

  for (size_t i = start; i < end; i++) {
    Guess ret = averageSolutions(guessList[i]);

    for (int j = 0; j < 5; j++) {
      if (ret < best[j]) {
        for (int k = 4; k > j; k--) {
          best[k] = best[k - 1];
        }

        best[j] = ret;
        j = 5;
      }
    }
  }

  bestMutex.lock();

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (best[i] < bestGuesses[j]) {
        for (int k = 4; k > j; k--) {
          bestGuesses[k] = bestGuesses[k - 1];
        }

        bestGuesses[j] = best[i];
        j = 5;
      } else if (j == 4) {
        i = 5;
      }
    }
  }

  bestMutex.unlock();
}


Guess Solver::averageSolutions(const std::string& word) {
  std::vector<int> results;
  double ret = 0;

  for (LetterColour colours = word; colours[SIZE - 1] <= GREEN; colours++) {
    ret = possibleSolutions(colours).size();
    if (ret > 0)
      results.emplace_back(ret);
  }

  if (ret == 1)
    ret = 0 - (static_cast<double>(ret) / results.size());
  else
    ret = 0;
  for (int i : results) 
    ret += static_cast<double>(i) / results.size();

  return Guess(word, ret);
}


std::vector<std::string> Solver::possibleSolutions(const LetterColour& colours) {
  std::vector<std::string> solutions;
  size_t largest = 0;
  bool allLargest;
  size_t index[SIZE];

  for (int i = 0; i < SIZE; i++)
    index[i] = 0;

  while (true) {
    allLargest = true;

    for (int i = 0; i < SIZE; i++) {
      if (colours[i] == GREEN) {
        while (index[i] < letterIndex[i][colours.letter(i) - 'a'].size() && letterIndex[i][colours.letter(i) - 'a'][index[i]] < largest) {
          index[i]++;
        }

        if (!(index[i] < letterIndex[i][colours.letter(i) - 'a'].size()))
          return solutions;

        if (letterIndex[i][colours.letter(i) - 'a'][index[i]] > largest) {
          largest = letterIndex[i][colours.letter(i) - 'a'][index[i]];
          allLargest = false;
        }
      }
    }

    if (allLargest) {
      if (colours.check(solutionList[largest])) {
        solutions.emplace_back(solutionList[largest]);
      }

      largest++;

      if (largest >= solutionList.size()) {
        return solutions;
      }
    }
  }
}
