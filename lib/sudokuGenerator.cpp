#include "sudokuGenerator.h"
#include "sudokuGenerator_.h"

#include "sudokuHelpers.h"
#include "sudokuSolver.h"

#include "logger.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>

//================
// public library functions
///================

SudokuGenerator::SudokuGenerator() : pSudokuGenerator(new SudokuGenerator_()) {}

SudokuGenerator::~SudokuGenerator() {
  if (pSudokuGenerator) {
    delete pSudokuGenerator;
  }
}

bool SudokuGenerator::Generate(Generator &generator) {
  if (generator.values.empty() ||
      (generator.values.size() != (generator.size * generator.size))) {
    generator.values.resize(generator.size * generator.size);
    const auto sectionSize{generator.sectionSize};
    for (int idx{}; idx < generator.size; idx++) {
      int counter{1};
      std::for_each_n(generator.values.begin() + (idx * generator.size),
                      generator.size,
                      [&counter, idx, sectionSize](auto &value) {
                        // 1 2 3
                        value = ((counter - 1) % sectionSize) + 1;
                        // increase 1 2 3 by 3 according to row
                        value.value() += (idx % sectionSize) * sectionSize;
                        counter++;
                      });
    }
  }
  return pSudokuGenerator->Generate(generator);
}

//================
// private library functions
///================

SudokuGenerator_::SudokuGenerator_()
    : pSudokuSolver(std::make_unique<SudokuSolver>()) {}

bool SudokuGenerator_::Generate(Generator &generator) {
  Solver solver(generator.size, generator.sectionSize, SolverTypes::Bitstring);
  solver.values = generator.values;
  auto startT{std::chrono::steady_clock::now()};

  auto timeleft{std::chrono::steady_clock::now() - startT};
  Log::Debug("Starting sudoku generation...");
  do {
    timeleft = std::chrono::steady_clock::now() - startT;
    Shuffle(generator);
    if (pSudokuSolver->ValidateSudoku(solver)) {
      Log::Debug("validated sudoku!");
      return true;
    }
  } while (generator.maxGenerationTime > timeleft);
  Log::Debug("Unable to generate sudoku in time...");
  return false;
}

void SudokuGenerator_::Shuffle(Generator &generator) {
  std::random_device rd;
  std::mt19937_64 g(rd());

  for (int squareIdx{}; squareIdx < generator.size; squareIdx++) {
    auto idxs{GetAllIndexesOfSquare(generator.size, generator.sectionSize,
                                    squareIdx)};
    const auto originalIdxs{idxs};
    std::shuffle(idxs.begin(), idxs.end(), g);
    for (int idx{}; idx < idxs.size(); idx++) {
      auto v1{generator.values[originalIdxs[idx]]};
      auto v2{generator.values[idxs[idx]]};
      generator.values[originalIdxs[idx]] = v2;
      generator.values[idxs[idx]] = v1;
    }
  }
}
