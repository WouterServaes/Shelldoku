#include "sudokuGenerator.h"
#include "sudokuGenerator_.h"
#include "sudokuHelpers.h"
#include "sudokuSolver.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <memory>
#include <random>
#include <ranges>
#include <utility>
#include <vector>
//===============
// private generator functions
///===============
enum class FillStyle { RowBased, SquareBased };

void None(Generator &){};
void Shuffle(Generator &generator);
void Shift(Generator &generator);

void Fill(Generator &generator, FillStyle squareBased);
void ShuffleRowsColumns(Generator &generator);

void PokeHoles(Generator &generator, Solver &solver,
               std::function<bool(const Solver &)> sudokuValidator);
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

void SudokuGenerator::Reset() { pSudokuGenerator->Reset(); }

void SudokuGenerator::InitiateGenerator(Generator &generator) {
  FillStyle fillStyle{};
  switch (generator.generatorType) {
  case GeneratorTypes::Shuffle:
    pSudokuGenerator->SetGenerateFunction(Shuffle);
    fillStyle = FillStyle::SquareBased;
    break;
  case GeneratorTypes::Shift:
    pSudokuGenerator->SetGenerateFunction(Shift);
    fillStyle = FillStyle::RowBased;
    break;
  case GeneratorTypes::None:
    pSudokuGenerator->SetGenerateFunction(None);
  default:
    break;
  }
  Fill(generator, fillStyle);
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

void SudokuGenerator_::Reset() { totalTries = 0; }

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
    PokeHoles(generator, solver,
              std::bind(&SudokuSolver::ValidateSudoku, pSudokuSolver.get(),
                        std::placeholders::_1));
    if (pSudokuSolver->ValidateSudoku(solver)) {
      Log::Debug("validated sudoku!");
      return true;
    }
    generator.values = oValues;
  } while (generator.maxGenerationTime > timeleft);
  Log::Debug("Unable to generate sudoku in time...");
  return false;
}

//===============
// private generator functions
///===============

void Fill(Generator &generator, FillStyle fillStyle) {
  if (generator.values.empty() ||
      (generator.values.size() != (generator.size * generator.size))) {
    generator.values.resize(generator.size * generator.size);
    const auto sectionSize{generator.sectionSize};

    std::function<int()> g{};
    switch (fillStyle) {
    case FillStyle::SquareBased:
      g = [sectionSize]() -> int {
        static int r{};
        static int c{};
        if (r == 9) {
          r = 0;
        }
        if (c == 9) {
          r++;
          c = 0;
        }

        auto v = ((++c - 1) % sectionSize) + 1;
        v += ((r % sectionSize) * sectionSize);

        return v;
      };

      break;
    case FillStyle::RowBased:
    default:
      g = []() -> int {
        static int c{};
        if (c == 9) {
          c = 0;
        }
        return ++c;
      };
      break;
    }

    std::ranges::generate(generator.values, g);
  }
}

void Shuffle(Generator &generator) {
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto sqrIdx :
       std::ranges::iota_view{0, static_cast<int>(generator.size)}) {
    // Get the indexes of the value array corresponding to the sudoku square
    auto idxs{
        GetAllIndexesOfSquare(generator.size, generator.sectionSize, sqrIdx)};
    const auto originalIdxs{idxs};
    std::shuffle(idxs.begin(), idxs.end(), g);
    // Swap the actual values according to the shuffles indexes
    auto itValuesBegin{generator.values.begin()};
    std::ranges::for_each(idxs, [&itValuesBegin, originalIdxs](auto idx) {
      std::swap(itValuesBegin[originalIdxs[idx]], itValuesBegin[idx]);
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
  // https://gamedev.stackexchange.com/a/138228
  // randomize the first row
  std::random_device rd;
  std::mt19937_64 g{rd()};
  std::shuffle(generator.values.begin(),
               generator.values.begin() + generator.size, g);
  // shift each row by some amount
  static const std::array<std::pair<int, int>, 8> lineShiftOrder{
      {{1, 3}, {2, 3}, {3, 1}, {4, 3}, {5, 3}, {6, 1}, {7, 3}, {8, 3}}};

  std::ranges::for_each(lineShiftOrder, [&generator](auto lineShift) {
    for (auto i : std::ranges::iota_view(0, static_cast<int>(generator.size))) {
      auto shiftTo{i + lineShift.second};
      // wrap around
      if (shiftTo >= generator.size) {
        shiftTo -= generator.size;
      }
      auto itPrevRow{generator.values.begin() +
                     (lineShift.first - 1) * generator.size};
      auto itBegin{generator.values.begin() + lineShift.first * generator.size};
      *(itBegin + i) = *(itPrevRow + shiftTo);
    }
  });
  // ShelldokuPrinter::PrintSingleLine(generator.values);
  ShuffleRowsColumns(generator);
  // ShelldokuPrinter::PrintSingleLine(generator.values);
}

void ShuffleRowsColumns(Generator &generator) {
  std::random_device rd;
  std::mt19937 g{rd()};
  const auto sudokuSize{generator.size};
  const int sectionSize{static_cast<int>(generator.sectionSize)};
  // shuffle the lines, but move rows/columns as 1 unit
  // Here assume the first 9 values to be row indexes, the last 9 values to be
  // column indexes.

  // Shufflers containing the index of the row/column,
  // both row and column indexes are stored in a single container
  // These are shuffled later
  std::vector<int> shuffler{};
  shuffler.resize(sudokuSize * 2);
  auto shufflerFiller{[sudokuSize]() {
    static int i{};
    if (i == sudokuSize) {
      i = 0;
    }
    return i++;
  }};
  std::ranges::generate(shuffler, shufflerFiller);
  auto itRowsBegin{shuffler.begin()};
  auto itColumnsBegin{shuffler.begin() + sudokuSize};
  // shuffle in pairs of 3 (don't cross square bariers)
  for (auto sectionIdx : std::ranges::iota_view{0, sectionSize}) {
    auto itRowsSectionBegin{itRowsBegin + sectionSize * sectionIdx};
    auto itColumnsSectionBegin{itColumnsBegin + sectionSize * sectionIdx};
    // shuffle shufflers
    std::shuffle(itRowsSectionBegin, itRowsSectionBegin + sectionSize, g);
    std::shuffle(itColumnsSectionBegin, itColumnsSectionBegin + sectionSize, g);

    // move the rows and columns according to the shufflers
    for (auto shuffleIdx : std::ranges::iota_view{0, sectionSize}) {
      auto rcIdx{sectionIdx * sectionSize + shuffleIdx};

      // optimization: don't move what shouldn't move
      if (itRowsBegin[rcIdx] != rcIdx) {
        // shuffle rows
        auto r1It{generator.values.begin() + rcIdx * sudokuSize};
        auto r2It{generator.values.begin() + itRowsBegin[rcIdx] * sudokuSize};
        std::ranges::swap_ranges(r1It, r1It + sudokuSize, r2It,
                                 r2It + sudokuSize);
      }

      // shuffle columns
      // optimization: don't move what shouldn't move
      if (itColumnsBegin[rcIdx] == rcIdx) {
        continue;
      }
      // Because the column values are not next to eachother in the values
      // container, first get the indexes, copy the values into a new container,
      // reorder the values according to the shuffler, store them back into the
      // values container.

      // the indexes of the values in the values container at the current
      // (incremental) column
      auto cIndexes{GetAllIndexesOfColumn(sudokuSize, rcIdx)};
      // the indexes of the values in the values container at the columns we're
      // swapping with
      auto shufflerIndexes{
          GetAllIndexesOfColumn(sudokuSize, itColumnsBegin[rcIdx])};

      for (auto cItIndxs{cIndexes.begin()},
           itShufflerIndxs{shufflerIndexes.begin()};
           cItIndxs != cIndexes.end() &&
           itShufflerIndxs != shufflerIndexes.end();
           cItIndxs++, itShufflerIndxs++) {
        std::swap(generator.values[*cItIndxs],
                  generator.values[*itShufflerIndxs]);
      }
    }
  }
}

void PokeHoles(Generator &generator, Solver &solver,
               std::function<bool(const Solver &)> sudokuValidator) {
  std::random_device rd;
  std::default_random_engine e{rd()};
  static std::uniform_int_distribution<int> uniformDistr(0, generator.size *
                                                                generator.size);
  int randomIdx{uniformDistr(e)};

  static int cntHoles{};
  auto itV{generator.values.begin() + randomIdx};
  // remove random value
  if (!itV->has_value()) {
    PokeHoles(generator, solver, sudokuValidator);
  }
  auto v{*itV};
  itV->reset();
  // is solvable?
  solver.values = generator.values;
  if (sudokuValidator(solver)) {
    cntHoles++;
  } else {
    *itV = v;
  }

  if (cntHoles < 10) {
    PokeHoles(generator, solver, sudokuValidator);
  }
}
