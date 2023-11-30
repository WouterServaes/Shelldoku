#pragma once
#include <optional>
#include <tuple>
#include <vector>

enum class SolverTypes {
  None = 0,
  Bitstring = 1
};

// X Y location
using X = std::size_t;
using Y = std::size_t;
using ValueLocation = std::pair<X, Y>;
//Sudoku values can be optional, empty
using SudokuValue = std::optional<unsigned int>;
// Every sudoku value can be locked
using LockableValue = std::pair<bool, SudokuValue>;

// Returns a 1D index given a 2D location
[[nodiscard]] static inline const std::size_t XYToSudokuPos(const std::size_t size, ValueLocation position) noexcept {
  return size * position.second + position.first;
} 
// returns a 2D index given a 1D index
[[nodiscard]] static inline const ValueLocation SudokuPosToXY(const std::size_t size, std::size_t idx) noexcept {
  return {idx % size, idx / size};
}

// Returns the square index of the given index if the given index is valid
[[nodiscard]] static const std::optional<std::size_t> SudokuPosSquareIndex(const std::size_t size, const std::size_t sectionSize, const std::size_t idx) noexcept
{
  if(idx < 0 || idx >= (size * size)) {
    return {};
  }

  // 3 * <square nr on Y> + <square nr on X>
  return {sectionSize * (SudokuPosToXY(size, idx).second / sectionSize) + (SudokuPosToXY(size, idx).first / sectionSize)};
}

// Returns the square index of the given xy position if the given index is valid
[[nodiscard]] static inline const std::optional<std::size_t> XYToSquareIndex(const std::size_t size, const std::size_t sectionSize, ValueLocation position) noexcept {
  return SudokuPosSquareIndex(size, sectionSize, XYToSudokuPos(size, position));
}

// Returns the first value index of the given square if the given square index is valid
[[nodiscard]] static const std::optional<std::size_t> GetIndexOfSquare(const std::size_t size, const std::size_t sectionSize, const std::size_t squareIdx) noexcept
{
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
  // <first square of "squareIdx" row from top> - (<corresponds to the amount of square to backtrack to reach the first in the square> * <2:because this is how it works> * <section size, to actually reach the first value in the section/square>) 
  return {(squareIdx * size) - (((squareIdx % sectionSize) * 2) * sectionSize)};
}

[[nodiscard]] static const std::vector<std::size_t> GetAllIndexesOfSquare(const std::size_t size, const std::size_t sectionSize, const std::size_t squareIdx)
{
  auto idxOfSqr{GetIndexOfSquare(size, sectionSize, squareIdx)};
  if(!idxOfSqr.has_value()) {
    return {};
  }
  std::vector<std::size_t> allIndexes;
  allIndexes.resize(size);

  int r{0};
  for(int idx{}; idx < size; idx++) {
    // base index
    allIndexes[idx] = idxOfSqr.value();
    // move column
    allIndexes[idx] += (idx % sectionSize);
    // move row
    if(idx && !(idx % sectionSize)) {
      r++;
    }
    allIndexes[idx] +=  r * size;
  }
  return allIndexes;
}

// static void ValidatePositionConverterFunctions() {
//   for(unsigned int r{}; r < size; r++) {
//     for(unsigned int c{}; c < size; c++) {
//       const auto value = values.at(XYToSudokuPos(size, {c,r}));
//       const auto v = (value.second? value.second.value() : 0);
//       Log::Debug((
//         "r" + std::to_string(r) 
//       + " c" + std::to_string(c)
//       + " idx" + std::to_string(XYToSudokuPos(size, {c,r})) 
//       + " xy" + std::to_string(SudokuPosToXY(size, XYToSudokuPos(size, {c,r})).first) 
//       + "," + std::to_string(SudokuPosToXY(size, XYToSudokuPos(size, {c,r})).second) 
//       + " sqrIdx" + std::to_string(SudokuPosSquareIndex(size, SectionSize(), XYToSudokuPos(size, {c,r})).value())
//       + " sqrIdx" + std::to_string(XYToSquareIndex(size, SectionSize(), {c,r}).value())
//       + " sqrP " + std::to_string(GetIndexOfSquare(size, SectionSize(), XYToSquareIndex(size, SectionSize(), {c,r}).value()).value())
      
//       + ": "+std::to_string(v)).c_str());
//     }
//   }
// }
