#include "include/public/sudokuGenerator.h"
#include "include/private/sudokuGenerator_.h"

#include "include/public/sudokuHelpers.h"
#include "include/public/sudokuSolver.h"

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

  do {
    Shuffle(generator);
  } while (pSudokuSolver->ValidateSudoku(solver) 
        && generator.maxGenerationTime < (std::chrono::steady_clock::now() - startT));

  return true;
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
