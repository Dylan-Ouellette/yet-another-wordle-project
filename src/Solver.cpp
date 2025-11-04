
#include "Solver.h"

#include <algorithm>
#include <fstream>
#include <thread>

using namespace Wordle;


int importWordList(const std::string& filepath, std::vector<std::string>& list) {
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


int writeBestList(const std::array<Guess, BEST_LIST_SIZE>& guesses,
                  const std::string& filepath = "../data/best_starting_guesses") {
  std::fstream file(filepath);

  if (!file.is_open()) {
    return 1;
  }

  for (int i = 0; i < BEST_LIST_SIZE; i++) {
    file << guesses[i] << std::endl;
  }

  file.close();

  return 0;
}


Solver::Solver() : bestGuesses({Guess("")}) {
  importWordList("../data/combined_wordlist.txt", guessList);
  importWordList("../data/shuffled_real_wordles.txt", solutionList);

  std::fstream file("../data/best_starting_guesses.txt");
  std::string str;

  for (int i = 0; i < BEST_LIST_SIZE && getline(file, str); i++) {
    bestGuesses[i] = Guess::readGuess(str);
  }

  file.close();
}


Solver::Solver(const std::string& guessListPath, const std::string& solutionListPath) : bestGuesses({Guess("")}) {
  importWordList(guessListPath, guessList);
  importWordList(solutionListPath, solutionList);
  
  findBest();
}


Solver::Solver(const std::vector<std::string>& newGuessList, const std::vector<std::string>& newSolutionList)
  : bestGuesses({Guess("")}), guessList(newGuessList), solutionList(newSolutionList) {
  findBest();
}


Solver::Solver(
  const std::vector<std::string>& newGuessList, 
  const std::vector<std::string>& newSolutionList, 
  const std::array<Guess, BEST_LIST_SIZE>& newBestGuesses
) : bestGuesses(newBestGuesses), guessList(newGuessList), solutionList(newSolutionList) {}


void Solver::operator=(const Solver& newSolver) {
  bestGuesses = newSolver.bestGuesses;
  guessList = newSolver.guessList;
  solutionList = newSolver.solutionList;
}


const std::vector<std::string>& Solver::getPossibleSolutions() const {
  return solutionList;
}


const std::vector<std::string>& Solver::getGuessWords() const {
  return guessList;
}


const std::array<Guess, BEST_LIST_SIZE>& Solver::getBestGuess() const {
  return bestGuesses;
}


void Solver::setGuess(const WordColour& colours) {
  solutionList = possibleSolutions(colours, solutionList);
  findBest();
}


void Solver::findBest() {
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
  Guess best[BEST_LIST_SIZE];

  for (size_t i = start; i < end; i++) {
    Guess ret = averageSolutions(guessList[i]);

    for (int j = 0; j < BEST_LIST_SIZE; j++) {
      if (ret < best[j]) {
        for (int k = BEST_LIST_SIZE - 1; k > j; k--) {
          best[k] = best[k - 1];
        }

        best[j] = ret;
        j = BEST_LIST_SIZE;
      }
    }
  }

  bestGuessesMutex.lock();

  for (int i = 0; i < BEST_LIST_SIZE; i++) {
    for (int j = 0; j < BEST_LIST_SIZE; j++) {
      if (best[i] < bestGuesses[j]) {
        for (int k = BEST_LIST_SIZE - 1; k > j; k--) {
          bestGuesses[k] = bestGuesses[k - 1];
        }

        bestGuesses[j] = best[i];
        j = BEST_LIST_SIZE;
      } else if (j == BEST_LIST_SIZE - 1) {
        i = BEST_LIST_SIZE;
      }
    }
  }

  bestGuessesMutex.unlock();
}


Guess Solver::averageSolutions(const std::string& word) {
  std::vector<std::string> possibles = solutionList;
  std::vector<int> results;
  double ret = 0;

  for (WordColour colours = word; colours[SIZE - 1] <= GREEN; colours++) {
    ret = possibleSolutions(colours, possibles).size();
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


std::vector<std::string> Solver::possibleSolutions(const WordColour& colours, std::vector<std::string>& possibles) {
  std::vector<std::string> solutions;
  auto i = possibles.begin();

  while (i < possibles.end()) {
    if (colours.check(*i)) {
      solutions.emplace_back(*i);
      i = possibles.erase(i);
    } else {
      i++;
    }
  }

  return solutions;
}
