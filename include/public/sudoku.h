
#pragma once
#include "sudokuHelpers.h"
#include "sudokuSolver.h"

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <optional>

namespace EVENT_ID {
static const unsigned int SUDOKU_PLACE = 5;
static const unsigned int SUDOKU_SOLVED = 6;
static const unsigned int SUDOKU_FAIL = 7;
}; // namespace EVENT_ID

class Sudoku final {
public:
  Sudoku() = delete;
  Sudoku(std::size_t size, std::unique_ptr<SudokuSolver> pSudokuSolver);
  Sudoku(std::size_t size, std::unique_ptr<SudokuSolver> pSudokuSolver, std::vector<SudokuValue> values);
  ~Sudoku();
  // Generates sudoku
  void GenerateSudoku();

  // Returns copy of values
  [[nodiscard]] const std::vector<SudokuValue> GetValues() const;
  // Returns the sudoku section size
  [[nodiscard]] inline const std::size_t SectionSize() const { return size / 3;}

  // Tries to place a value on a location
  // Returns true on success (value was placed)
  bool PlaceValue(ValueLocation location, SudokuValue value);
  // Returns true if sudoku is solvable
  [[nodiscard]] bool IsSolvable() noexcept;
  // Solves the sudoku
  void Solve();
  // Start for continuous solving with user input
  void Start();
  // Stop for continuous solving with user input, locks all values
  void Stop();
  // Returns true if the sudoku is solved
  [[nodiscard]] bool IsSolved() const noexcept;
private:
  // Sets values to Lockable values vector
  void SetValues(const std::vector<SudokuValue> values);

  // Returns true if value can be placed on location
  [[nodiscard]] bool CanPlaceValue(const std::vector<LockableValue>& toPlace, ValueLocation location, SudokuValue value) const noexcept;

  // Returns true if sudoku is solved
  [[nodiscard]] bool IsSolved(const std::vector<LockableValue>& toCheck) const noexcept;

  void Shuffle();

  const std::size_t size;
  std::vector<LockableValue> values;
  std::unique_ptr<SudokuSolver> pSudokuSolver;
};
