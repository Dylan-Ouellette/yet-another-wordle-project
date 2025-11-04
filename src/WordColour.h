#ifndef WordColour_H
  #define WordColour_H

#include <array>
#include <string>

#include "Wordle.h"

namespace Wordle {

class WordColour {

protected:
  std::string word;
  std::array<Colour, SIZE> colours;

public:
  WordColour(const std::string& word, const std::array<Colour, SIZE>& colours = {GREY});

  void operator++();
  void operator++(int);
  char letter(size_t index) const;
  Colour& operator[](size_t index);
  const Colour& operator[](size_t index) const;

  void setColour(const std::array<Colour, SIZE>& newColours);
  bool check(const std::string& checkWord) const;
}; // WordColour

} // Wordle

#endif // WordColour_H