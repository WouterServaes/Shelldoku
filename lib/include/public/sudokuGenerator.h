#pragma once
#include "sudokuHelpers.h"
#include <chrono>
#include <memory>
#include <vector>

class SudokuGenerator_;

struct Generator {
  Generator() = delete;
  Generator(const std::size_t size_, const std::size_t sectionSize_,
            std::chrono::seconds maxGenerationTime_)
      : size(size_), sectionSize(sectionSize_),
        maxGenerationTime(maxGenerationTime_) {}

  std::vector<SudokuValue> values{};
  const std::size_t size;
  const std::size_t sectionSize;

  std::chrono::seconds maxGenerationTime;
};

class SudokuGenerator {
public:
  SudokuGenerator();
  ~SudokuGenerator();
  SudokuGenerator(const SudokuGenerator &) = delete;
  SudokuGenerator(SudokuGenerator &&) = delete;
  SudokuGenerator &operator=(const SudokuGenerator &) = delete;
  SudokuGenerator &operator=(SudokuGenerator &&) = delete;

  [[nodiscard]] bool Generate(Generator &generator);

private:
  SudokuGenerator_ *pSudokuGenerator;
};
