#include "include/public/sudoku.h"
#include "include/public/shelldokuPrinter.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <random>
#include <math.h>
//https://norvig.com/sudoku.html

// layout
//X/Y  0 1 2  3 4 5  6 7 8
//     │ │ │  │ │ │  │ │ │
//  0 ─A1A2A3│A4A5A6│A6A7A8
//  1 ─B1B2B3│B4B5B6│B6B7B8   0   1   2
//  2 ─C1C2C3│C4C5C6│C6C7C8
//     ──────┼──────┼──────
//  3 ─D1D2D3│D4D5D6│D6D7D8
//  4 ─E1E2E3│E4E5E6│E6E7E8   3   4   5
//  5 ─F1F2F3│F4F5F6│F6F7F8
//     ──────┼──────┼──────
//  6 ─G1G2G3│G4G5G6│G6G7G8
//  7 ─H1H2H3│H4H5H6│H6H7H8   6   7   8
//  8 ─I1I2I3│I4I5I6│I6I7I8

// container layout:
// Every sudoku section is placed next to eachother. Where section 0 is top left section, and value 0 is top left value in section. 
// sudoku value idx:  A1  A2  A3  A4  A5  A6  A6  A7  A8    B1  B2  B3  B4  B5  B6  B6  B7  B8    C1  C2  C3  C4  C5  C6  C6  C7  C8    D1  D2  D3  D4  D5  D6  D6  D7  D8    E1  E2  E3  E4  E5  E6  E6  E7  E8...
// container idx:     0   1   2   3   4   5   6   7   8   | 9   10  11  12  13  14  15  16  17  | 18  19  20  21  22  23  24  25  26  | 27  28  29  30  31  32  33  34  35  | 36  37  38  39  40  41  42  43  44...
// XY:                00  10  20  30  40  50  60  70  80    90  01  11  21  31  41  51  61  71    81  91  02  12  22  32  42  52  62    72  82  92  03  13  23  33  43  53    63  73  83  93  04  14  24  34  44...
// => XY to container:


Sudoku::Sudoku(std::size_t _size) : size(_size) { values.reserve(size * size); }

Sudoku::~Sudoku() {}

void Sudoku::GenerateSudoku() {
  values.resize(size * size);

  std::random_device rd;
  std::mt19937_64 g(rd());
  for (int idx{}; idx < size; idx++) {
    int counter{1};

    std::for_each_n(values.begin() + (idx * size), size,
                    [&counter](LockableValue &value) { value.second = counter++; value.first = true;});

    // std::shuffle(values.begin() + (idx * size),
    //              (values.begin() + (idx * size) + size), g);
  }
}

const std::vector<Sudoku::SudokuValue> Sudoku::getValues() const 
{
      std::vector<SudokuValue> c;
      c.resize(values.size());
      auto cIt{c.begin()};
      auto vIt{values.begin()};
      for(;cIt != c.end() && vIt != values.end();cIt++, vIt++) {
        *cIt = vIt->second;
      }
      return c;
}

bool Sudoku::PlaceValue(ValueLocation location, SudokuValue value) {
  // Only place it if the value is not locked
  if(!values.at(InputToSudokuPos(location)).first) {
    values.at(InputToSudokuPos(location)).second = value;
    return true;
  }
  return false;
}

void Sudoku::SolveSudoku()
{
   
}

