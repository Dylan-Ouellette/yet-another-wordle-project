#ifndef LETTERCOLOUR_H
  #define LETTERCOLOUR_H

#include <array>
#include <string>

#include "Wordle.h"

namespace Wordle {

class LetterColour {

protected:
  std::string word;
  std::array<Colour, SIZE> colours;

public:
  LetterColour(const std::string& word, const std::array<Colour, SIZE>& colours = {GREY});

  inline void operator++();
  void operator++(int);
  char letter(size_t index) const;
  Colour& operator[](size_t index);
  const Colour& operator[](size_t index) const;

  void setColour(const std::array<Colour, SIZE>& newColours);
  bool check(const std::string& checkWord) const;
}; // LetterColour

} // Wordle

#endif // LETTERCOLOUR_H