#ifndef WORDLE_H
  #define WORDLE_H

namespace Wordle {

constexpr int SIZE = 5;
constexpr int THREADS = 4;
constexpr int BEST_LIST_SIZE = 5; // 1-100 valid values.

enum Colour : int {
  GREY = 0,
  YELLOW = 1,
  GREEN = 2,
};

} // Wordle

#endif // WORDLE_H