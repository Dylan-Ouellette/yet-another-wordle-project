# yet-another-wordle-project
It has become a cliche to make a Wordle based project, but as it turns out that is because it's just that much fun. This project focuses on solving Wordle puzzles by finding the word with the lowest average number of possible solution words.

### How to Build
CMake 3.16 or newer is required.
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### How to Use
Run the executable `yet-another-wordle-project.exe`

A list of top guesses and their average results will be presented.
Enter a guess and then the colours like the example bellow:
```
Guess:    trace
Colours:  bbgyg
```
*NOTE: lowercase is required.*

Repeat until the solution is found or enter just `x` as a Guess to exit.

### Project TODO's
- add Docker containers.
- add UI using Qt.
- add linter.
- add test coverage analysis.
- add more tests.
- add option for recursive depth.
- add input checking and error handling.
