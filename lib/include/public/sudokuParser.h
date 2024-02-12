#pragma once
#include "sudokuHelpers.h"

namespace sudokuParser {

void ParseToFile(const std::vector<SudokuValue> &values, std::string_view file);

} // namespace sudokuParser
