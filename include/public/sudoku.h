
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
  [[nodiscard]] inline const std::size_t SectionSize() const { return size / 3;}
private:
  void SolveSudoku();
  // Returns the indexes of the peers of a given index if the given index is valid
  // Peer (20 for a 9x9 sudoku): All values inside the square of the index, all horizontal values, all vertical values
  [[nodiscard]] const std::optional<std::vector<std::size_t>> GetPeers(const std::size_t idx) const noexcept;
  // Returns the indexes of the direct peers of a given index if the given index is valid
  // Direct Peer (8 for a 9x9 sudoku): All values inside the square of the index, excluding the given idx
  [[nodiscard]] const std::optional<std::vector<std::size_t>> GetDirectPeers(const std::size_t idx) const noexcept;
  // Returns the square index of the given index if the given index is valid
  [[nodiscard]] const std::optional<std::size_t> GetSquareIndex(const std::size_t idx) const noexcept;
  // Returns the first value index of the given square if the given square index is valid
  [[nodiscard]] const std::optional<std::size_t> GetIndexOfSquare(const std::size_t squareIdx) const noexcept;
  
  [[nodiscard]] inline const std::size_t InputToSudokuPos(ValueLocation inputPos) const noexcept {
    return size * inputPos.second + inputPos.first;
  } 
  [[nodiscard]] inline const ValueLocation SudokuPosToInput(std::size_t idx) const noexcept {
    return {idx % size, idx / size};
  }

  const std::size_t size;
  std::vector<LockableValue> values;
};
