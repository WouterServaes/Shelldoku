#include "include/public/sudoku.h"
#include "include/public/shelldokuPrinter.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <random>

// layout
// 123│123│123
// 456│456│456   0   1   2
// 789│789│789
// ───┼───┼───
// 123│123│123
// 456│456│456   3   4   5
// 789│789│789
// ───┼───┼───
// 123│123│123
// 456│456│456   6   7   8
// 789│789│789

Sudoku::Sudoku(std::size_t _size) : size(_size) { values.reserve(size * size); }

Sudoku::~Sudoku() {}

void Sudoku::GenerateSudoku() {
  values.resize(size * size);

  for (int idx{}; idx < size; idx++) {
    int counter{};
    std::random_device rd;
    std::mt19937_64 g(rd());

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