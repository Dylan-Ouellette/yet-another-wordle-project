#ifndef GUESS_H
  #define GUESS_H 

#include <string>
#include <tuple>

namespace Wordle {

class Guess {

private:
  std::string w;
  double average;

public:
  Guess(const std::string& word = "", double averageResult = 999999) : w(word), average(averageResult) {}

  friend inline bool operator==(const Guess& l, const Guess& r) {
    return l.average == r.average;
  }

  friend inline bool operator!=(const Guess& l, const Guess& r) {
    return !(l == r);
  }

  friend inline bool operator<(const Guess& l, const Guess& r) {
    return std::tie(l.average, l.w) < std::tie(r.average, r.w);
  }

  friend inline bool operator>(const Guess& l, const Guess& r) {
    return r < l;
  }

  friend inline bool operator<=(const Guess& l, const Guess& r) {
    return !(l > r);
  }

  friend inline bool operator>=(const Guess& l, const Guess& r) {
    return !(l < r);
  }

  const std::string& getWord() {
    return w;
  }

  double getAverageResult() {
    return average;
  }

}; // Guess

} // Wordle

#endif // GUESS_H
