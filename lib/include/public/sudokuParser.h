#pragma once
#include "sudokuHelpers.h"
#include <vector>

namespace sudokuParser {

void ParseToFile(const std::vector<SudokuValue> &values,
                 const std::string &file);
void ParseFromFile(std::vector<SudokuValue> &values, const std::string &file,
                   unsigned int line = 0);
} // namespace sudokuParser
