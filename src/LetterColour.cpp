
#include "LetterColour.h"

using namespace Wordle;

inline Colour operator++(Colour& colour, int) {
  Colour ret = colour;
  colour = static_cast<Colour>(colour + 1);
  return ret;
}


LetterColour::LetterColour(const std::string& word, const std::array<Colour, SIZE>& colours) : 
    word(word), colours(colours) {
}


void LetterColour::operator++() {
  bool valid = false;

  while(!valid) {
    int sum = 0;
    valid = true;
    colours[0]++;

    for (int i = 0; i < SIZE; i++) {
      if (colours[i] > GREEN && i < SIZE - 1) {
        colours[i] = GREY;
        colours[i + 1]++;
      } else if (colours[i] == YELLOW) {
        for (int j = 0; j < i; j++) {
          if (colours[j] == GREY && word[j] == word[i])
            valid = false;
        }
      }

      sum += colours[i];
    }

    if (sum == 9)
      valid = false;
  }
}


void LetterColour::operator++(int) {
  operator++();
}


char LetterColour::letter(size_t index) const {
  return word[index];
}


Colour& LetterColour::operator[](size_t index) {
  return colours[index];
}


const Colour& LetterColour::operator[](size_t index) const {
  return colours[index];
}


void LetterColour::setColour(const std::array<Colour, SIZE>& newColours) {
  colours = newColours;
}


bool LetterColour::check(const std::string& checkWord) const {  
  bool used[SIZE] = {false};

  for (int i = 0; i < SIZE; i++) {
    if (colours[i] == GREEN && word[i] != checkWord[i]) {
      return false;
    } else if (colours[i] == YELLOW) {
      if (checkWord[i] == word[i]) {
        return false;
      }

      bool found = false;

      for (int j = 0; !found && j < SIZE; j++) {
        if (!used[j] && checkWord[j] == word[i] && checkWord[j] != word[j]) {
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
        if (!used[j] && colours[j] != GREEN && checkWord[j] == word[i]) {
          return false;
        }
      }
    }
  }

  return true;
}
