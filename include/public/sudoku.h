
#pragma once

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <optional>

namespace EVENT_ID {
static const unsigned int SUDOKU_PLACE = 4;
}; // namespace EVENT_ID

class Sudoku final {
public:
  // X Y location
  using ValueLocation = std::pair<std::size_t, std::size_t>;
  //Sudoku values can be optional, empty
  using SudokuValue = std::optional<unsigned int>;
  // Every sudoku value can be locked
  using LockableValue = std::pair<bool, SudokuValue>;
  Sudoku() = delete;
  Sudoku(std::size_t size);
  ~Sudoku();
  // Generates sudoku
  void GenerateSudoku();
  // Returns copy of values
  [[nodiscard]] const std::vector<SudokuValue> getValues() const;
  // Tries to place a value on a location
  // Returns true on success (value was placed)
  // Returns false on failure (value was not placed)
  bool PlaceValue(ValueLocation location, SudokuValue value);
  // Returns the sudoku section size
  [[nodiscard]] const std::size_t SectionSize() const { return size / 3;}
private:
  void SolveSudoku();
  [[nodiscard]] const std::size_t ScreenToWorldPos(ValueLocation gridPos) const noexcept; 
  const std::size_t size;
  std::vector<LockableValue> values;
};
