#pragma once
#include "sudokuHelpers.h"

namespace sudokuDifficulty {

enum class Difficulty : unsigned int { easy = 0, normal = 50, hard = 100 };

[[nodiscard]] Difficulty
CalculateDifficulty(const std::vector<SudokuValue> &sudoku,
                    const std::size_t size, const std::size_t sectionSize);

}; // namespace sudokuDifficulty