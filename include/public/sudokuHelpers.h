#pragma once
#include <optional>
#include <tuple>

// X Y location
using ValueLocation = std::pair<std::size_t, std::size_t>;
//Sudoku values can be optional, empty
using SudokuValue = std::optional<unsigned int>;
// Every sudoku value can be locked
using LockableValue = std::pair<bool, SudokuValue>;

// Returns a 1D index given a 2D location
[[nodiscard]] static inline const std::size_t InputToSudokuPos(const std::size_t size, ValueLocation inputPos) noexcept {
  return size * inputPos.first + inputPos.second;
} 
// returns a 2D index given a 1D index
[[nodiscard]] static inline const ValueLocation SudokuPosToInput(const std::size_t size, std::size_t idx) noexcept {
  return {idx / size, idx % size};
}

// Returns the square index of the given index if the given index is valid
[[nodiscard]] static const std::optional<std::size_t> SudokuPosSquareIndex(const std::size_t size, const std::size_t sectionSize, const std::size_t idx) noexcept
{
  if(idx < 0 || idx >= (size * size)) {
    return {};
  }

  return {sectionSize * (SudokuPosToInput(size, idx).first / sectionSize) + (SudokuPosToInput(size, idx).second / sectionSize)};
}

// Returns the square index of the given xy position if the given index is valid
[[nodiscard]] static inline const std::optional<std::size_t> InputToSquareIndex(const std::size_t size, const std::size_t sectionSize, ValueLocation inputPos) noexcept {
  return SudokuPosSquareIndex(size, sectionSize, InputToSudokuPos(size, inputPos));
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
  return {(squareIdx * size) - (((squareIdx % sectionSize) * 2) * sectionSize)};
}

// static void ValidatePositionConverterFunctions() {
//   for(unsigned int r{}; r < size; r++) {
//     for(unsigned int c{}; c < size; c++) {
//       const auto value = values.at(InputToSudokuPos(size, {r, c}));
//       const auto v = (value.second? value.second.value() : 0);
//       Log::Debug(("r" + std::to_string(r) + " c" + std::to_string(c)+ " sp" + std::to_string(InputToSudokuPos(size, {r,c})) +
//       " ip" + std::to_string(SudokuPosToInput(size, InputToSudokuPos(size, {r,c})).first) + "," 
//       + std::to_string(SudokuPosToInput(size, InputToSudokuPos(size, {r,c})).second) 
//       //+ " sqridx" + std::to_string(GetIndexOfSquare(size, SectionSize(), InputToSudokuPos(size, {r,c})).value())
//       + " sqr" + std::to_string(SudokuPosSquareIndex(size, SectionSize(), InputToSudokuPos(size, {r,c})).value())
//       + " sqr" + std::to_string(InputToSquareIndex(size, SectionSize(), {r, c}).value())
//       + " sqrP " + std::to_string(GetIndexOfSquare(size, SectionSize(), InputToSquareIndex(size, SectionSize(), {r, c}).value()).value())
      
//       + ": "+std::to_string(v)).c_str());
//     }
//   }
// }
