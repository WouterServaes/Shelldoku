#pragma once
#include "sudokuHelpers.h"
#include <vector>

class Solver;

class SudokuSolver_ {
public:
  SolverTypes solverType{SolverTypes::None};

  SudokuSolver_() = default;
  virtual ~SudokuSolver_() = default;

  // Returns true if sudoku can be solved, solves sudoku in values
  virtual bool Solve(Solver &solver) noexcept = 0;
  // Returns true if the sudoku can be solved
  [[nodiscard]] virtual bool CanBeSolved(const Solver &solver) noexcept;
  // Returns true if the values can be placed on a location in values
  [[nodiscard]] virtual bool CanBePlaced(const Solver &solver,
                                         ValueLocation location,
                                         SudokuValue value) const noexcept = 0;
  // Validates the correctness of the sudoku values
  [[nodiscard]] virtual bool ValidateSudoku(const Solver &solver) noexcept = 0;
  // Prepares the solver for a continuous game
  virtual void PrepareSudoku(const Solver &solver) noexcept = 0;
};

class SudokuSolver_bitmasks final : public SudokuSolver_ {
public:
  SolverTypes solverType{SolverTypes::Bitstring};

  SudokuSolver_bitmasks() = default;
  ~SudokuSolver_bitmasks() = default;

  [[nodiscard]] bool Solve(Solver &solver) noexcept override;
  [[nodiscard]] bool CanBePlaced(const Solver &solver, ValueLocation location,
                                 SudokuValue value) const noexcept override;
  [[nodiscard]] bool ValidateSudoku(const Solver &solver) noexcept override;
  [[nodiscard]] bool CanBeSolved(const Solver &solver) noexcept override;
  void PrepareSudoku(const Solver &solver) noexcept override;

private:
  [[nodiscard]] bool Solve(Solver &solver, ValueLocation location);
  // Generates the initial bitmask value according to given values
  void GenerateBitMasks(const Solver &solver);
  // Clears all bit masks
  void ClearBitMasks();
  // Helper to set all 3 bitmasks for location/value
  void SetBitMasks(const Solver &solver, ValueLocation location,
                   SudokuValue value) noexcept;
  // Helper to reset all 3 bitmasks for location/value
  void ResetBitMasks(const Solver &solver, ValueLocation location,
                     SudokuValue value) noexcept;
  // Returns true if bitmasks are generates
  [[nodiscard]] bool GeneratedBitMasks(const Solver &solver) const noexcept;
  // bitmasks for row, column, box
  std::vector<unsigned int> bitMasksRows{};
  std::vector<unsigned int> bitMasksColumns{};
  std::vector<unsigned int> bitMasksSquares{};
};