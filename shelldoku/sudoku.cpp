#include "sudoku.h"
#include "shelldokuPrinter.h"

#include "logger.h"

#include <chrono>
#include <cstddef>
#include <iterator>
#include <math.h>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// https://norvig.com/sudoku.html

// layout
// X/Y  0 1 2  3 4 5  6 7 8
//     │ │ │  │ │ │  │ │ │
//  0 ─A1A2A3│A4A5A6│A7A8A9
//  1 ─B1B2B3│B4B5B6│B7B8B9   0   1   2
//  2 ─C1C2C3│C4C5C6│C7C8C9
//     ──────┼──────┼──────
//  3 ─D1D2D3│D4D5D6│D7D8D9
//  4 ─E1E2E3│E4E5E6│E7E8E9   3   4   5
//  5 ─F1F2F3│F4F5F6│F7F8F9
//     ──────┼──────┼──────
//  6 ─G1G2G3│G4G5G6│G7G8G9
//  7 ─H1H2H3│H4H5H6│H7H8H9   6   7   8
//  8 ─I1I2I3│I4I5I6│I7I8I9

// container layout:
// Every sudoku section is placed next to eachother. Where section 0 is top left
// section, and value 0 is top left value in section. sudoku value idx:  A1  A2
// A3  A4  A5  A6  A7  A8  A9    B1  B2  B3  B4  B5  B6  B7  B8  B9    C1  C2 C3
// C4  C5  C6  C7  C8  C9    D1  D2  D3  D4  D5  D6  D7  D8  D9    E1  E2  E3 E4
// E5  E6  E7  E8  E9... container idx:     0   1   2   3   4   5   6   7   8 |
// 9   10  11  12  13  14  15  16  17  | 18  19  20  21  22  23  24  25  26  |
// 27  28  29  30  31  32  33  34  35  | 36  37  38  39  40  41  42  43  44...
// XY:                00  10  20  30  40  50  60  70  80    01  11  21  31  41
// 51  61  71  81    02  12  22  32  42  52  62  72  82    03  13  23  33  43 53
// 63  73  83    04  14  24  34  44  54  64  74  84... Stored in Row-major order
// Where X defines the columns index, and Y defines the row index. array[width *
// row + col] = value;

Sudoku::Sudoku(std::size_t _size)
    : size(_size),
      pSudokuSolver(std::unique_ptr<SudokuSolver>(new SudokuSolver())),
      pSudokuGenerator(
          std::unique_ptr<SudokuGenerator>(new SudokuGenerator())) {
  values.resize(size * size);
  const auto sectionSize{SectionSize()};
  // fill the entire sudoku to start with
  for (int idx{}; idx < size; idx++) {
    int counter{1};

    std::for_each_n(values.begin() + (idx * size), size,
                    [&counter, idx, sectionSize](LockableValue &value) {
                      // 1 2 3
                      value.second = ((counter - 1) % sectionSize) + 1;
                      // increase 1 2 3 by 3 according to row
                      value.second.value() += (idx % sectionSize) * sectionSize;
                      counter++;
                      value.first = false;
                    });
  }
  //   for(unsigned int r{}; r < size; r++) {
  //   for(unsigned int c{}; c < size; c++) {
  //     // const auto value = values.at(XYToSudokuPos(size, {c,r}));
  //     // const auto v = (value.second? value.second.value() : 0);
  //     Log::Debug((
  //       "r" + std::to_string(r)
  //     + " c" + std::to_string(c)
  //     + " idx" + std::to_string(XYToSudokuPos(size, {c,r}))
  //     + " xy" + std::to_string(SudokuPosToXY(size, XYToSudokuPos(size,
  //     {c,r})).first)
  //     + "," + std::to_string(SudokuPosToXY(size, XYToSudokuPos(size,
  //     {c,r})).second)
  //     + " sqrIdx" + std::to_string(SudokuPosSquareIndex(size, SectionSize(),
  //     XYToSudokuPos(size, {c,r})).value())
  //     + " sqrIdx" + std::to_string(XYToSquareIndex(size, SectionSize(),
  //     {c,r}).value())
  //     + " sqrP " + std::to_string(GetIndexOfSquare(size, SectionSize(),
  //     XYToSquareIndex(size, SectionSize(), {c,r}).value()).value())

  //     //+ ": "+std::to_string(v))
  //     ).c_str());
  //   }
  // }
}

Sudoku::Sudoku(std::size_t _size, std::vector<SudokuValue> _values)
    : size(_size),
      pSudokuSolver(std::unique_ptr<SudokuSolver>(new SudokuSolver())),
      pSudokuGenerator(
          std::unique_ptr<SudokuGenerator>(new SudokuGenerator())) {
  for (auto v : _values) {
    if (v.value() == 0) {
      v.reset();
    }
    values.emplace_back(LockableValue{v.has_value(), v});
  }
}

Sudoku::~Sudoku() {}

void Sudoku::GenerateSudoku(Generator &generator) {
  generator.values = GetValues();
  if (!pSudokuGenerator->Generate(generator)) {
    Log::Debug("Unable to generate");
    return;
  }
  SetValues(generator.values);
}

const std::vector<SudokuValue> Sudoku::GetValues() const {
  std::vector<SudokuValue> c;
  c.resize(values.size());
  auto cIt{c.begin()};
  auto vIt{values.begin()};
  for (; cIt != c.end() && vIt != values.end(); cIt++, vIt++) {
    *cIt = vIt->second;
  }
  return c;
}

void Sudoku::SetValues(const std::vector<SudokuValue> _values) {
  auto cIt{_values.begin()};
  auto vIt{values.begin()};
  for (; cIt != _values.end() && vIt != values.end(); cIt++, vIt++) {
    vIt->second = *cIt;
    vIt->first = (*cIt).has_value();
  }
}

bool Sudoku::PlaceValue(ValueLocation location, SudokuValue value) {
  if (CanPlaceValue(values, location, value)) {
    values.at(XYToSudokuPos(size, location)).second = value;
    return true;
  }

  return false;
}

bool Sudoku::IsSolvable() noexcept {
  Solver solver{size, SectionSize(), SolverTypes::Bitstring};
  solver.values = GetValues();
  return pSudokuSolver->ValidateSudoku(solver) &&
         pSudokuSolver->CanBeSolved(solver);
}

bool Sudoku::CanPlaceValue(const std::vector<LockableValue> &toPlaceOn,
                           ValueLocation location,
                           SudokuValue value) const noexcept {
  if (toPlaceOn.at(XYToSudokuPos(size, location)).first) {
    return false;
  }

  if (!value.has_value() ||
      !XYToSquareIndex(size, SectionSize(), location).has_value()) {
    return false;
  }

  // Allow placement on all empty tiles
  return true;
}

bool Sudoku::IsSolved(
    const std::vector<LockableValue> &toCheck) const noexcept {
  Solver solver{size, SectionSize(), SolverTypes::Bitstring};
  solver.values = GetValues();
  return std::find_if(toCheck.begin(), toCheck.end(),
                      [](LockableValue lv) {
                        return !lv.second.has_value();
                      }) == toCheck.end() &&
         pSudokuSolver->ValidateSudoku(solver);
}

void Sudoku::Solve() {
  Solver solver{size, SectionSize(), SolverTypes::Bitstring};
  solver.values = GetValues();
  Log::Debug("trying to solve...");
  if (pSudokuSolver->Solve(solver)) {
    SetValues(solver.values);
    Log::Debug("solved");
  } else {
    Log::Debug("can't be solved");
  }
}

void Sudoku::Start() {}

void Sudoku::Stop() {
  // lock all values
  std::for_each(values.begin(), values.end(),
                [](LockableValue &lv) { lv.first = true; });
}

bool Sudoku::IsSolved() const noexcept { return IsSolved(values); }
