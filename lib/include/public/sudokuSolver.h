#pragma once
#include "sudokuHelpers.h"
#include <memory>
#include <vector>

class SudokuSolver_;

struct Solver {
  Solver() = delete;
  Solver(const std::size_t size_, const std::size_t sectionSize_,
         const SolverTypes solverType_)
      : size(size_), sectionSize(sectionSize_), solvertType(solverType_) {}

  std::vector<SudokuValue> values{};
  const std::size_t size;
  const std::size_t sectionSize;
  const SolverTypes solvertType;
};

class SudokuSolver {
public:
  SudokuSolver();
  ~SudokuSolver();
  SudokuSolver(const SudokuSolver &) = delete;
  SudokuSolver(SudokuSolver &&) = delete;
  SudokuSolver &operator=(const SudokuSolver &) = delete;
  SudokuSolver &operator=(SudokuSolver &&) = delete;

  [[nodiscard]] bool CanBeSolved(const Solver &solver);
  [[nodiscard]] bool CanBePlaced(const Solver &solver, ValueLocation location,
                                 SudokuValue value);
  [[nodiscard]] bool ValidateSudoku(const Solver &solver);
  [[nodiscard]] bool Solve(Solver &solver);

private:
  void InitiateSolver(const SolverTypes solverType);
  std::unique_ptr<SudokuSolver_> pSudokuSolver;
};