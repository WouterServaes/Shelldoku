#include "sudokuGenerator.h"
#include "sudokuGenerator_.h"

#include "sudokuHelpers.h"
#include "sudokuSolver.h"

#include "logger.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>

//===============
// private generator functions
///===============
void None(Generator &){};
void Shuffle(Generator &generator);
void Shift(Generator &generator);
//================
// public library functions
///================

SudokuGenerator::SudokuGenerator()
    : pSudokuGenerator(std::make_unique<SudokuGenerator_>()) {}

SudokuGenerator::~SudokuGenerator() {}

bool SudokuGenerator::Generate(Generator &generator) {
  Fill(generator);
  InitiateGenerator(generator.generatorType);
  return pSudokuGenerator->Generate(generator);
}

unsigned int SudokuGenerator::TotalTries() const {
  return pSudokuGenerator->TotalTries();
}

void SudokuGenerator::InitiateGenerator(const GeneratorTypes generatorType) {

  switch (generatorType) {
  case GeneratorTypes::Shuffle:
    pSudokuGenerator->SetGenerateFunction(Shuffle);
    break;
  case GeneratorTypes::Shift:
    pSudokuGenerator->SetGenerateFunction(Shift);
    break;
  case GeneratorTypes::None:
    pSudokuGenerator->SetGenerateFunction(None);
  default:
    break;
  }
}

void SudokuGenerator::Fill(Generator &generator) {
  if (generator.values.empty() ||
      (generator.values.size() != (generator.size * generator.size))) {
    generator.values.resize(generator.size * generator.size);
    const auto sectionSize{generator.sectionSize};
    for (int idx{}; idx < generator.size; idx++) {
      int counter{1};
      auto itBegin{generator.values.begin() + (idx * generator.size)};
      std::for_each_n(itBegin, generator.size,
                      [&counter, idx, sectionSize](auto &value) {
                        // 1 2 3
                        value = ((counter - 1) % sectionSize) + 1;
                        // increase 1 2 3 by 3 according to row
                        value.value() += (idx % sectionSize) * sectionSize;
                        counter++;
                      });
    }
  }
}

//================
// private library functions
///================

SudokuGenerator_::SudokuGenerator_()
    : pSudokuSolver(std::make_unique<SudokuSolver>()), generateFunction(None) {}

SudokuGenerator_::SudokuGenerator_(
    std::function<void(Generator &)> generateFunction_)
    : pSudokuSolver(std::make_unique<SudokuSolver>()),
      generateFunction(generateFunction_) {}

unsigned int SudokuGenerator_::TotalTries() const { return totalTries; }

void SudokuGenerator_::SetGenerateFunction(
    std::function<void(Generator &)> generateFunction_) {
  generateFunction = generateFunction_;
}

bool SudokuGenerator_::Generate(Generator &generator) {
  Solver solver(generator.size, generator.sectionSize, SolverTypes::Bitstring);
  solver.values = generator.values;
  auto startT{std::chrono::steady_clock::now()};

  auto timeleft{std::chrono::steady_clock::now() - startT};
  Log::Debug("Starting sudoku generation...");
  do {
    totalTries++;
    timeleft = std::chrono::steady_clock::now() - startT;
    generateFunction(generator);
    if (pSudokuSolver->ValidateSudoku(solver)) {
      Log::Debug("validated sudoku!");
      return true;
    }
  } while (generator.maxGenerationTime > timeleft);
  Log::Debug("Unable to generate sudoku in time...");
  return false;
}

//===============
// private generator functions
///===============
void Shuffle(Generator &generator) {
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

void Shift(Generator &generator) {
  // randomize first row
  // set second row by shifting first row by n1
  // set third row by shifting seconds row by n1

  // set fourth row by shifting third row by 1
  // set fifth row by shifting fourth row by n2
  //  ...
  for (std::size_t idx{}; idx < generator.size; idx++) {

    // auto itBegin{generator.values.begin() + (idx * generator.size)};
    // std::for_each_n(itBegin, generator.size, [idx](){});
  }
}