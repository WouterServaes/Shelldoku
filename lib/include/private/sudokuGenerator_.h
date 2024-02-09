#pragma once
#include "sudokuHelpers.h"
#include "sudokuSolver.h"

#include <cstddef>
#include <memory>
#include <vector>

class Generator;

class SudokuGenerator_ {
public:
  SudokuGenerator_();
  ~SudokuGenerator_() = default;
  SudokuGenerator_(const SudokuGenerator_ &) = delete;
  SudokuGenerator_(SudokuGenerator_ &&) = delete;
  SudokuGenerator_ &operator=(const SudokuGenerator_ &) = delete;
  SudokuGenerator_ &operator=(SudokuGenerator_ &&) = delete;

  [[nodiscard]] bool Generate(Generator &generator);

private:
  void Shuffle(Generator &generator);
  std::unique_ptr<SudokuSolver> pSudokuSolver;
};
