#pragma once

#include "../../common/include/public/ansi.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include "../../common/include/public/logger.h"
namespace ShelldokuPrinter {

// Fills the terminal with a placeholder sudoku board
static std::size_t FillCout(std::size_t size);
// Prints the line dividers
static void PrintDividers(std::size_t size);
// Prints the sudoku
// None-existing vector values are printed as a space
static void PrintSudoku(const std::vector<std::optional<unsigned int>> &values, std::size_t size);
// Flag indicating if cout is filled, should be replaced by some clean check
static bool CoutFilled{false};
// Prints str and returns cursor to before printed str
static void PrintSingle(const std::string_view &str);
// Prepares the sudoku field, helper function
static void PrepareSudokuField(std::size_t size);

static std::size_t FillCout(std::size_t size) {
  for (int r{}; r++ < size;) {
    for (int c{}; c++ < size;) {
      std::cout << "x";
      if (c < size - 1 && c % (size / 3) == 0) {
        std::cout << "o";
      }
    }
    std::cout << std::endl;
    if (r < size - 1 && r % (size / 3) == 0) {
      for (int div{}; div++ < size;) {
        std::cout << "o";
        if (div < size - 1 && div % (size / 3) == 0) {
          std::cout << "o";
        }
      }

      std::cout << std::endl;
    }
  }
  CoutFilled = true;
  // 
  return size + 2;
}

static void PrintDividers(std::size_t size) {
  if (!CoutFilled)
    FillCout(size);

  for (int r{}; r++ < size;) {
    for (int c{}; c++ < size;) {
      Ansi::MoveRight(1);
      if (c < size - 1 && c % (size / 3) == 0) {
        std::cout << "║";
      }
    }
    Ansi::MoveDown(1);
    Ansi::MoveLeft(size + 3);
    if (r < size - 1 && r % (size / 3) == 0) {
      for (int div{}; div++ < size;) {
        std::cout << "═";
        if (div < size - 1 && div % (size / 3) == 0) {
          std::cout << "╬";
        }
      }

      Ansi::MoveDown(1);
      Ansi::MoveLeft(size + 3);
    }
  }
}

static void PrintSudoku(const std::vector<std::optional<unsigned int>> &values, std::size_t size) {
  const std::size_t sectionSize{size / 3};
  
  for(std::size_t idx{}; idx < values.size(); idx++) {
    // vertical spaces
    if(idx > 0 && !(idx % sectionSize) && (idx % size)) {
      Ansi::MoveRight();
    } 
    // horizontal spacer
    if(idx > 0 && !(idx % (size * sectionSize))) {
      Ansi::MoveDown();
    }
    // enters afters every row
    if(idx > 0 && !(idx % size)) {
      Ansi::MoveDown();
      Ansi::MoveLeft(size + sectionSize);
    }
    // value printing
    const auto v{values[idx]};
    std::cout << (v.has_value() ? std::to_string(v.value()) :" ");
  }

  Ansi::BackToSaved();
  PrintDividers(size);
  Ansi::BackToSaved();
}

static void PrintSingle(const std::string_view &str)
{
  std::cout<<str;
  Ansi::MoveLeft(str.size());
}

static void PrepareSudokuField(std::size_t size)
{
  std::cout << std::endl;
  std::cout << std::endl;
  auto sizeFromTop {FillCout(size)};
  while(sizeFromTop--) {
    Ansi::MoveUp();
  }
  Ansi::SaveCursorPos();
}

}; // namespace ShelldokuPrinter
