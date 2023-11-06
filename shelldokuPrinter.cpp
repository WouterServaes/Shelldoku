#include "include/public/shelldokuPrinter.h"

using namespace shelldokuPrinter;

PrinterLogic::PrinterLogic(std::size_t size) : size(size) {}

void PrinterLogic::PrintSingle(const std::string_view &str) {
  // get location of cursor
  // if cursor is not on a divier
  // if cursor is on an empty field
  // if cursos is on a field that can be changed

  // print and move left
  std::cout << str;
  Ansi::MoveLeft(str.length());
};
