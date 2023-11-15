
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
  Sudoku(std::size_t size, std::vector<SudokuValue> values);
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
  // Returns true if sudoku is solvable
  [[nodiscard]] bool IsSolvable() noexcept;

  void Solve();
private:
  // Returns true on success
  // Tries to place value on location in given vector
  bool PlaceValue(std::vector<LockableValue>& toPlaceOn, ValueLocation location, SudokuValue value) const;
  // Returns true on success
  // Tries to solve given vector
  bool SolveSudoku(std::vector<LockableValue>& toSolve);
  // Gathers all peer indexes of given idx, stores them in foundPeers
  // Peer (20 for a 9x9 sudoku): All values inside the square of the index, all horizontal values, all vertical values
  void GatherPeers(const std::vector<LockableValue>& toGatherFrom, const std::size_t idx, std::vector<SudokuValue>& foundPeers) const noexcept;
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

  // Returns true if value can be placed on location
  [[nodiscard]] bool CanPlaceValue(const std::vector<LockableValue>& toPlace, ValueLocation location, SudokuValue value) const noexcept;

  // Returns true if sudoku is solved
  [[nodiscard]] bool IsSolved(const std::vector<LockableValue>& toCheck) const noexcept;

  const std::size_t size;
  std::vector<LockableValue> values;
  unsigned int solveCounter{};
};
