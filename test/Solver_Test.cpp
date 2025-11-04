
#include "../src/Solver.h"

#include <gtest/gtest.h>

#include <fstream>

using namespace Wordle;


class SoverTest : public testing::Test {
  protected:
  Solver solver;
  std::string solution;
  std::array<Guess, BEST_LIST_SIZE> bestGuessCopy;
  std::vector<std::string> guessListCopy;
  std::vector<std::string> solutionListCopy;


  SoverTest() {
    bestGuessCopy = solver.getBestGuess();
    guessListCopy = solver.getGuessWords();
    solutionListCopy = solver.getPossibleSolutions();
  }


  void setColours(WordColour& colours) {
    for (int i = 0; i < SIZE; i++) {
      if (colours.letter(i) == solution[i]) {
        colours[i] = GREEN;
      } else {
        colours[i] = GREY;
      }
    }

    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (colours[i] != GREEN && colours[j] == GREY && solution[i] == colours.letter(j)) {
          colours[j] = YELLOW;
          j = SIZE;
        }
      }
    }
  }


  int run() {
    std::string guessWord = solver.getBestGuess()[0].getWord();
    int guessNum = 1;

    while (guessWord != solution) {
      WordColour colours = guessWord;
      setColours(colours);

      solver.setGuess(colours);
      guessWord = solver.getBestGuess()[0].getWord();
      guessNum++;
    }

    return guessNum;
  }


  void reset() {
    solver = Solver(guessListCopy, solutionListCopy, bestGuessCopy);
  }
};


TEST_F(SoverTest, DoubleLetterTest) { // 0.8s
  solution = "guppy";
  EXPECT_EQ(run(), 4);
}


TEST_F(SoverTest, DifficultWords) { // 3.85484 in 35.2s
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
