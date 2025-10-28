
#include "../src/Solver.h"

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

using namespace Wordle;


class SoverTest : public testing::Test {
  protected:
  Solver solver;
  std::string solution;
  std::vector<std::string> guessListCopy;
  std::vector<std::string> solutionListCopy;


  SoverTest() : solver("../data/combined_wordlist.txt", "../data/shuffled_real_wordles.txt") {
    guessListCopy = solver.getGuessWords();
    solutionListCopy = solver.getPossibleSolutions();
  }


  void setColours(const std::string& guessWord, LetterColour colours[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
      if (guessWord[i] == solution[i]) {
        colours[i] = GREEN;
      } else {
        colours[i] = GREY;
      }
    }

    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (colours[i] != GREEN && colours[j] == GREY && solution[i] == guessWord[j]) {
          colours[j] = YELLOW;
          j = SIZE;
        }
      }
    }
  }


  int run() {
    std::string guessWord = solver.getBestGuess().getWord();
    LetterColour colours[SIZE];
    int guessNum = 1;

    while (guessWord != solution) {
      setColours(guessWord, colours);

      solver.setGuess(guessWord, colours);
      guessWord = solver.getBestGuess().getWord();
      guessNum++;
    }

    return guessNum;
  }


  void reset() {
    solver = Solver(guessListCopy, solutionListCopy);
  }
};


TEST_F(SoverTest, DoubleLetterTest) { // 3.4s
  solution = "guppy";
  EXPECT_EQ(run(), 4);
}


TEST_F(SoverTest, DifficultWords) { // 3.85484 in 270s
  std::fstream file("../data/difficult_wordlist.txt");

  ASSERT_TRUE(file.is_open());

  std::string word;
  int guessCount = 0;
  int wordCount = 0;
  int ret;

  while (getline(file, word)) {
    solution = word;
    ret = run();
    guessCount += ret;
    wordCount++;
    reset();

    EXPECT_LE(ret, 6);
  }

  ASSERT_TRUE(file.eof());

  file.close();

  EXPECT_EQ(static_cast<int>(10000 * static_cast<double>(guessCount) / wordCount), static_cast<int>(10000 * 3.85484));
}
