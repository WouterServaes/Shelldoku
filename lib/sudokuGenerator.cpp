#include "sudokuGenerator.h"
#include "sudokuGenerator_.h"

#include "sudokuHelpers.h"
#include "sudokuSolver.h"

#include "logger.h"

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>
#include <ranges>
#include <utility>
#include <vector>
//===============
// private generator functions
///===============
void None(Generator &){};
void Shuffle(Generator &generator);
void Shift(Generator &generator);

void Fill(Generator &generator, bool squareBased);

//================
// public library functions
///================

SudokuGenerator::SudokuGenerator()
    : pSudokuGenerator(std::make_unique<SudokuGenerator_>()) {}

SudokuGenerator::~SudokuGenerator() {}

bool SudokuGenerator::Generate(Generator &generator) {
  InitiateGenerator(generator);
  return pSudokuGenerator->Generate(generator);
}

unsigned int SudokuGenerator::TotalTries() const {
  return pSudokuGenerator->TotalTries();
}

void SudokuGenerator::InitiateGenerator(Generator &generator) {
  auto squareBased{false};
  switch (generator.generatorType) {
  case GeneratorTypes::Shuffle:
    pSudokuGenerator->SetGenerateFunction(Shuffle);
    squareBased = true;
    break;
  case GeneratorTypes::Shift:
    pSudokuGenerator->SetGenerateFunction(Shift);
    break;
  case GeneratorTypes::None:
    pSudokuGenerator->SetGenerateFunction(None);
  default:
    break;
  }
  Fill(generator, squareBased);
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
  auto startT{std::chrono::steady_clock::now()};

  auto timeleft{std::chrono::steady_clock::now() - startT};
  Log::Debug("Starting sudoku generation...");
  const auto oValues{generator.values};
  do {
    totalTries++;
    timeleft = std::chrono::steady_clock::now() - startT;
    generateFunction(generator);
    solver.values = generator.values;
    if (pSudokuSolver->ValidateSudoku(solver)) {
      Log::Debug("validated sudoku!");
      return true;
    }
    return false;
    generator.values = oValues;
  } while (generator.maxGenerationTime > timeleft);
  Log::Debug("Unable to generate sudoku in time...");
  return false;
}

//===============
// private generator functions
///===============

void Fill(Generator &generator, bool squareBased) {
  if (generator.values.empty() ||
      (generator.values.size() != (generator.size * generator.size))) {
    generator.values.resize(generator.size * generator.size);
    const auto sectionSize{generator.sectionSize};
    for (auto idx :
         std::ranges::iota_view(0, static_cast<int>(generator.size))) {
      auto itBegin{generator.values.begin() + (idx * generator.size)};
      if (squareBased) {
        int counter{1};
        std::for_each_n(itBegin, generator.size,
                        [&counter, idx, sectionSize](auto &value) {
                          // 1 2 3
                          value = ((counter - 1) % sectionSize) + 1;
                          value.value() += (idx % sectionSize) * sectionSize;
                          // increase 1 2 3 by 3 according to row
                          counter++;
                        });
      } else {
        std::iota(itBegin, itBegin + generator.size, 1);
      }
    }
  }
}

void Shuffle(Generator &generator) {
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto i : std::ranges::iota_view{0, 10}) {
    // Get the indexes of the value array corresponding to the sudoku square
    auto idxs{GetAllIndexesOfSquare(generator.size, generator.sectionSize, i)};
    const auto originalIdxs{idxs};
    std::shuffle(idxs.begin(), idxs.end(), g);
    // Swap the actual values according to the shuffles indexes
    std::for_each(idxs.begin(), idxs.end(),
                  [originalIdxs, &generator](std::size_t idx) {
                    auto v1{generator.values[originalIdxs[idx]]};
                    auto v2{generator.values[idx]};
                    generator.values[originalIdxs[idx]] = v2;
                    generator.values[idx] = v1;
                  });
  }
}

void Shift(Generator &generator) {
  // randomize first row
  // set second row by shifting first row by n1
  // set third row by shifting seconds row by n1

  // set fourth row by shifting third row by 1
  // set fifth row by shifting fourth row by n2
  //  ...
  std::random_device rd;
  std::mt19937_64 g{rd()};
  std::shuffle(generator.values.begin(),
               generator.values.begin() + generator.size, g);

  std::array<std::pair<int, int>, 8> lineShiftOrder{
      {{1, 3}, {2, 3}, {3, 1}, {4, 3}, {5, 3}, {6, 1}, {7, 3}, {8, 3}}};

  std::for_each(lineShiftOrder.begin(), lineShiftOrder.end(),
                [&generator](auto lineShift) {
                  for (auto i : std::ranges::iota_view(
                           0, static_cast<int>(generator.size))) {
                    auto shiftTo{i + lineShift.second};
                    if (shiftTo >= generator.size) {
                      shiftTo -= generator.size;
                    }

                    auto itPrevRow{generator.values.begin() +
                                   (lineShift.first - 1) * generator.size};
                    auto itBegin{generator.values.begin() +
                                 lineShift.first * generator.size};
                    *(itBegin + i) = *(itPrevRow + shiftTo);
                  }
                });
}