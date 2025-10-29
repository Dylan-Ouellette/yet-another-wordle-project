
#include "Solver.h"

#include <algorithm>
#include <fstream>
#include <future>
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

  std::vector<std::future<Guess>> threadFutures;
  auto numThreads = std::thread::hardware_concurrency();
  Guess ret;

  if (!numThreads) {
    numThreads = THREADS;
  }

  numThreads = numThreads * 4;

  using namespace std::chrono_literals;

  auto nextWord = guessList.begin();
  while (nextWord != guessList.end() || threadFutures.size() > 0) {
    while (threadFutures.size() < numThreads && nextWord != guessList.end()) {
      threadFutures.emplace_back(std::async(std::launch::async, &Solver::averageSolutions, this, *nextWord.base()));
      nextWord++;
    }

    auto i = threadFutures.begin();
    while (i != threadFutures.end()) {
      if (i.base()->wait_for(0ms) == std::future_status::ready) {
        ret = i.base()->get();

        for (int k = 0; k < 5; k++) {
          if (ret < bestGuesses[k]) {
            for (int j = 4; j > k; j--) {
              bestGuesses[j] = bestGuesses[j - 1];
            }

            bestGuesses[k] = ret;
            k = 5;
          }
        }
        
        i = threadFutures.erase(i);
      } else {
        i++;
      }
    }
  }
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
