#pragma once
#include "sudokuGenerator.h"
#include "sudokuSolver.h"

#include <functional>
#include <memory>

class Generator;

class SudokuGenerator_ {
public:
  SudokuGenerator_();
  SudokuGenerator_(std::function<void(Generator &)> generateFunction_);
  ~SudokuGenerator_() = default;
  SudokuGenerator_(const SudokuGenerator_ &) = delete;
  SudokuGenerator_(SudokuGenerator_ &&) = delete;
  SudokuGenerator_ &operator=(const SudokuGenerator_ &) = delete;
  SudokuGenerator_ &operator=(SudokuGenerator_ &&) = delete;

  [[nodiscard]] bool Generate(Generator &generator);
  [[nodiscard]] unsigned int TotalTries() const;
  void SetGenerateFunction(std::function<void(Generator &)> generateFunction);

private:
  std::function<void(Generator &)> generateFunction;
  unsigned int totalTries{};

protected:
  std::unique_ptr<SudokuSolver> pSudokuSolver;
};