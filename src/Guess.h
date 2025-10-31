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
  Guess(const std::string& word = "", double averageResult = std::numeric_limits<double>::max())
  : w(word), average(averageResult) {}

  static Guess readGuess(const std::string& str) {
    return Guess(str.substr(0, 5), stod(str.substr(8)));
  }

  friend inline std::ostream& operator<<(std::ostream& l, const Guess& r) {
    return  l << r.w << std::string(" - ") << std::to_string(r.average);
  }

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

  const std::string& getWord() const {
    return w;
  }

  double getAverageResult() const {
    return average;
  }

}; // Guess

} // Wordle

#endif // GUESS_H
