#include "sudokuParser.h"
#include "sudokuHelpers.h"
#include <fstream>
#include <ios>
#include <limits>
#include <stdexcept>
#include <vector>

namespace sudokuParser {

void ParseToFile(const std::vector<SudokuValue> &values,
                 const std::string &file) {
  auto str = ParseToString(values, ',');
  str += "\n";
  std::ofstream f{file, std::ios::app};
  f.write(str.c_str(), str.size());
  f.close();
}

void ParseFromFile(std::vector<SudokuValue> &values, const std::string &file,
                   unsigned int line) {
  std::ifstream f{file};
  if (!f.is_open()) {
    throw std::runtime_error("file does not exist");
    return;
  }
  for (unsigned int lineIdx{}; lineIdx < line; lineIdx++) {
    f.seekg(std::ios::beg);
    f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::string sudokuLine{};
  f >> sudokuLine;
  f.close();
  ParseFromString(values, sudokuLine);
}
} // namespace sudokuParser
