#pragma once
#include "sudokuDifficulty.h"
#include "sudokuHelpers.h"
#include <vector>

namespace sudokuParser {
void ParseToFile(const std::vector<SudokuValue> &values,
                 sudokuDifficulty::Difficulty, const std::string &file);
void ParseFromFile(std::vector<SudokuValue> &values, const std::string &file);
} // namespace sudokuParser
