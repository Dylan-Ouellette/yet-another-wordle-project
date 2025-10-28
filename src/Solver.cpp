
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


void indexLetters(const std::vector<std::string>& list, std::vector<int> letterIndex[SIZE][26]) {
  for (int i = 0; i < SIZE; i++)
    for (int j = 0; j < 26; j++)
      letterIndex[i][j].clear();

  for (int i = 0; i < list.size(); i++)
    for (int y = 0; y < SIZE; y++)
      letterIndex[y][list[i][y] - 97].emplace_back(i);
}


bool checkWord(const std::string& word, const std::string& guessWord, LetterColour colours[SIZE]) {  
  bool used[SIZE];
  for (int i = 0; i < SIZE; i ++)
    used[i] = false;
  bool found;

  for (int i = 0; i < SIZE; i++) {
    if (colours[i] == YELLOW) {
      if (word[i] == guessWord[i]) {
        return false;
      }

      found = false;

      for (int j = 0; !found && j < SIZE; j++) {
        if (!used[j] && word[j] == guessWord[i] && word[j] != guessWord[j]) {
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
        if (!used[j] && colours[j] != GREEN && word[j] == guessWord[i]) {
          return false;
        }
      }
    }
  }

  return true;
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


void Solver::setGuess(const std::string& word, LetterColour colours[SIZE]) {
  solutionList = possibleSolutions(word, colours);
  indexLetters(solutionList, letterIndex);

  using namespace std::chrono_literals;

  std::vector<std::future<Guess>> threadFutures;
  auto numThreads = std::thread::hardware_concurrency();
  Guess ret;

  if (!numThreads) {
    numThreads = THREADS;
  }

  numThreads = numThreads * 4;

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





std::vector<std::string> Solver::possibleSolutions(const std::string& word, LetterColour colours[SIZE]) {
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


Guess Solver::averageSolutions(const std::string& word) {
  std::vector<int> results;
  LetterColour colours[SIZE];
  int sum;
  double ret = 0;
  bool skip = false;

  for (int i = 0; i < SIZE; i++) {
    colours[i] = GREY;
  }

  while (colours[SIZE - 1] <= GREEN) {
    if (!skip) {
      ret = possibleSolutions(word, colours).size();
      if (ret > 0)
        results.emplace_back(ret);
    }

    skip = false;
    sum = 0;
    colours[0]++;

    for (int k = 0; k < SIZE; k++) {
      if (colours[k] > GREEN && k < SIZE - 1) {
        colours[k] = GREY;
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

  return Guess(word, ret);
}
