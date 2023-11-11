#include "include/public/sudoku.h"
#include "include/public/shelldokuPrinter.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <random>
#include <math.h>

// layout
//X/Y  0 1 2  3 4 5  6 7 8
//     │ │ │  │ │ │  │ │ │
//  0 ─010203│111213│212223
//  1 ─040506│141516│242526   0   1   2
//  2 ─070809│171819│272829
//     ──────┼──────┼──────
//  3 ─313233│414243│515253
//  4 ─343536│444546│545556   3   4   5
//  5 ─373839│474849│575859
//     ──────┼──────┼──────
//  6 ─616263│717273│818283
//  7 ─646566│747576│848586   6   7   8
//  8 ─676869│777879│878889

// container layout:
// Every sudoku section is placed next to eachother. Where section 0 is top left section, and value 0 is top left value in section. 
// sudoku value idx:  01  02  03  04  05  06  07  08  09  | 11  12  13  14  15  16  17  18  19  | 21  22  23  24  25  26  27  28  29  | 31  32  33  34  35  36  37  38  39  | 41  42  43  44  45  46  47  48  49...
// container idx:     0   1   2   3   4   5   6   7   8   | 9   10  11  12  13  14  15  16  17  | 18  19  20  21  22  23  24  25  26  | 27  28  29  30  31  32  33  34  35  | 36  37  38  39  40  41  42  43  44...
// XY:                00  10  20  01  11  21  02  12  22  | 30  40  50  31  41  51  32  42  52  | 60  70  80  61  71  81  62  72  82  | 03  13  23  04  14  24  50  51  52  | 33  34  35  43  44  45  53  54  55
// => XY to container:
//    
// 

// layout reasoning:
// sudoku value idx:  for sudoku algorithms
// container idx:     values are stored in a container
// XY:                for input handling 

// Placement of values:
// X, Y refer to X, Y in full grid layout. 
// 4, 4 is sudoku value idx 45  - container idx 40
// 1, 6 is sudoku value idx 62  - container idx


/// =====================
// the positioning of sudoku values in the vector should really be by row
// This will make the 2d to 1d easier


Sudoku::Sudoku(std::size_t _size) : size(_size) { values.reserve(size * size); }

Sudoku::~Sudoku() {}

void Sudoku::GenerateSudoku() {
  values.resize(size * size);

  std::random_device rd;
  std::mt19937_64 g(rd());
  for (int idx{}; idx < size; idx++) {
    int counter{};

    std::for_each_n(values.begin() + (idx * size), size,
                    [&counter](LockableValue &value) { value.second = counter++; value.first = true;});

    std::shuffle(values.begin() + (idx * size),
                 (values.begin() + (idx * size) + size), g);
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



  const auto idx{size * location.first + location.second};
  // Only place it if the value is not locked
  if(!values.at(idx).first) {
    values.at(idx).second = value;
    return true;
  }
  return false;
}

void Sudoku::SolveSudoku()
{
   
}

const std::size_t Sudoku::ScreenToWorldPos(ValueLocation gridPos) const noexcept
{
  std::size_t wPos;

  // Every 3 of Y where X % 0 == 0, we are in a different square

  return wPos;
}
