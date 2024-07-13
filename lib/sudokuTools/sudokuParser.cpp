#include "sudokuParser.h"
#include "sudokuDifficulty.h"
#include "sudokuHelpers.h"
#include <algorithm>
#include <fstream>
#include <ios>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace sudokuParser {

int LinesOfFile(std::ifstream file) {
  return std::count(std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>(), '\n');
}

void ParseToFile(const std::vector<SudokuValue> &values,
                 sudokuDifficulty::Difficulty difficulty,
                 const std::string &file) {
  auto str = std::to_string(static_cast<unsigned int>(difficulty)) + "-" +
             ParseToString(values, ',');
  str += "\n";
  std::ofstream f{file, std::ios::app};
  f.write(str.c_str(), str.size());
  f.close();
}

void ParseFromFile(std::vector<SudokuValue> &values, const std::string &file) {
  std::ifstream f{file};
  if (!f.is_open()) {
    throw std::runtime_error("file does not exist");
    return;
  }

  // for (unsigned int lineIdx{}; lineIdx < line; lineIdx++) {
  //   f.seekg(std::ios::beg);
  //   f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  // }

  std::string sudokuLine{};
  f >> sudokuLine;
  f.close();
  // grab difficulty
  const auto difficulty = sudokuLine.substr(0, sudokuLine.find("-"));
  // remove difficulty part from string
  sudokuLine = sudokuLine.substr(sudokuLine.find("-") + 1, sudokuLine.size());
  ParseFromString(values, sudokuLine);
}
} // namespace sudokuParser
