#include "include/public/sudoku.h"
#include "include/public/shelldokuPrinter.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <random>
#include <math.h>
#include <string>
//https://norvig.com/sudoku.html

// layout
//X/Y  0 1 2  3 4 5  6 7 8
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
// Every sudoku section is placed next to eachother. Where section 0 is top left section, and value 0 is top left value in section. 
// sudoku value idx:  A1  A2  A3  A4  A5  A6  A7  A8  A9    B1  B2  B3  B4  B5  B6  B7  B8  B9    C1  C2  C3  C4  C5  C6  C7  C8  C9    D1  D2  D3  D4  D5  D6  D7  D8  D9    E1  E2  E3  E4  E5  E6  E7  E8  E9...
// container idx:     0   1   2   3   4   5   6   7   8   | 9   10  11  12  13  14  15  16  17  | 18  19  20  21  22  23  24  25  26  | 27  28  29  30  31  32  33  34  35  | 36  37  38  39  40  41  42  43  44...
// XY:                00  10  20  30  40  50  60  70  80    01  11  21  31  41  51  61  71  81    02  12  22  32  42  52  62  72  82    03  13  23  33  43  53  63  73  83    04  14  24  34  44  54  64  74  84...
// => XY to container:


Sudoku::Sudoku(std::size_t _size) : size(_size) { values.reserve(size * size); }

Sudoku::~Sudoku() {}

void Sudoku::GenerateSudoku() {
  values.resize(size * size);

  std::random_device rd;
  std::mt19937_64 g(rd());
  for (int idx{}; idx < size; idx++) {
    int counter{1};

    std::for_each_n(values.begin() + (idx * size), size,
                    [&counter](LockableValue &value) { value.second = counter++; value.first = false;});
    // std::shuffle(values.begin() + (idx * size),
    //              (values.begin() + (idx * size) + size), g);
  }

  for(int idx{}; idx < values.size(); idx++) {
    const auto p{GetDirectPeers(idx)};
    if(p.has_value()) {
      Log::Debug((std::to_string(idx) + " " 
      + std::to_string(p.value()[0]) + " "
      + std::to_string(p.value()[1]) + " "
      + std::to_string(p.value()[2]) + " " 
      + std::to_string(p.value()[3]) + " "
      + std::to_string(p.value()[4]) + " "
      + std::to_string(p.value()[5]) + " "
      + std::to_string(p.value()[6]) + " "
      + std::to_string(p.value()[7]) + " "
      ).c_str());
    }
  }
}

const std::vector<Sudoku::SudokuValue> Sudoku::getValues() const 
{
      std::vector<SudokuValue> c;
      c.resize(values.size());
      auto cIt{c.begin()};
      auto vIt{values.begin()};
      for(;cIt != c.end() && vIt != values.end();cIt++, vIt++) {
        *cIt = vIt->second;
      }
      return c;
}

bool Sudoku::PlaceValue(ValueLocation location, SudokuValue value) {
  // Only place it if the value is not locked
  if(!values.at(InputToSudokuPos(location)).first) {
    values.at(InputToSudokuPos(location)).second = value;
    Log::Debug(std::to_string(InputToSudokuPos(location)).c_str());
    return true;
  }
  return false;
}

void Sudoku::SolveSudoku()
{
   
}

const std::optional<std::vector<std::size_t>> Sudoku::GetPeers(const std::size_t idx) const noexcept
{
  if(idx > 0 && idx < values.size()) {
    const auto xy{SudokuPosToInput(idx)};
    const auto directPeers{GetDirectPeers(idx)};
    // for 9x9 sudoku: 8 horizontal, 8 vertical, 4 left in the square excluding the hor/vert
    const std::size_t peersCount{((size - 1) * 2) + (size - 1 - 4)};
    
  }

  return {};
}

const std::optional<std::vector<std::size_t>> Sudoku::GetDirectPeers(const std::size_t idx) const noexcept
{
  if(idx < 0 || idx >= values.size()) {
    return {};
  }
  const auto sqrIdx{GetSquareIndex(idx)};
  if(!sqrIdx.has_value()) {
    return {};
  }
  const auto indexOfSquare{GetIndexOfSquare(sqrIdx.value())};
  if(!indexOfSquare.has_value()) {
    return {};
  }

  std::vector<std::size_t> r{};
  // includes all but the given square indexes of a square
  r.resize(size - 1);

  // r[peer] = indexOfSquare + (size * (peer/section)) + (peer % SectionSize())
  // r[0] = indexOfSquare + (9 * (0 / 3)) + (0 % 3)
  // r[3] = indexOfSquare + (9 * (3 / 3)) + (3 % 3)
  // r[4] = indexOfSquare + (9 * (4 / 3)) + (4 % 3)
  // r[8] = indexOfSquare + (9 * (8 / 3)) + (8 % 3)
  // use separate peer counter for skipping a value while going over all the values in a square (9) and while returning a size 8 array
  std::size_t peer{};
  for(std::size_t peerIdx{}; peerIdx <= size; peerIdx++) {
    const auto rowMultiplier{static_cast<std::size_t>(peerIdx / SectionSize())};
    const auto columnMultiplier{static_cast<std::size_t>(peerIdx % SectionSize())};
    const auto v{indexOfSquare.value() + (size * rowMultiplier) + columnMultiplier};

    // if the given index is the calculated index, skip it
    if(idx == v) {
      continue;
    }
    // not given index
    r[peer] = v;
    peer++;
  }
  
  return r;
    
  
}

const std::optional<std::size_t> Sudoku::GetSquareIndex(const std::size_t idx) const noexcept
{
  if(idx < 0 || idx >= values.size()) {
    return {};
  }

  return {SectionSize() * (SudokuPosToInput(idx).second / SectionSize()) + (SudokuPosToInput(idx).first / SectionSize())};
}

const std::optional<std::size_t> Sudoku::GetIndexOfSquare(const std::size_t squareIdx) const noexcept {
  if(squareIdx < 0 || squareIdx > size - 1) {
    return {};
  }

  // example:
  // for square 5, result should be 33

  // find first value index of row corresponding to squareIdx (eg the 5th row)
  // (squareIdx * size) = 45
  // (half of) the amount of squares to backtrack
  // (squareIdx % 3) = 2
  // backtrack from 45 to 33
  // 45 - (2 * 2) * 3
  // = 45 - 4 * 3 
  // = 45 - 12
  // = 33
  return {(squareIdx * size) - (((squareIdx % SectionSize()) * 2) * SectionSize())};
}

