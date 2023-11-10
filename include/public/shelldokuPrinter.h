#pragma once

#include "ansi.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <vector>

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
  std::size_t currentBigRow{};
  for (std::size_t idx{}; idx < size; idx++) {
    std::size_t blockIdx{(idx % sectionSize) * sectionSize};

    // inner block number counter
    std::size_t counter{};
    // row index of number inside the 3x3 block
    std::size_t currentInnerRow{};
    // row index of the 3x3 blocks
    std::size_t row{static_cast<std::size_t>(std::floor(idx / sectionSize))};

    // Move cursor to the next row of 3x3 blocks when the last grid on a row
    // is complete
    if (currentBigRow != row && row != 0) {
      Ansi::MoveDown(4);
      currentBigRow = row;
    }

    // print 9x9 block
    std::for_each_n(values.begin(), size,
                    [&counter, sectionSize, &currentInnerRow](std::optional<unsigned int> value) {
                      // Column and Row index of number in inner sudoku block
                      std::size_t innerColumn{counter % sectionSize};
                      std::size_t innerRow{static_cast<std::size_t>(
                          std::floor(counter / sectionSize))};

                      // Move cursor down when the number should be on a new row
                      if (currentInnerRow != innerRow && innerRow != 0) {
                        Ansi::MoveDown(1);
                        currentInnerRow = innerRow;
                      }

                      // print the number
                      
                      std::cout << (value.has_value() ? std::to_string(value.value()) : " ");

                      // Divider section, move to right is important for next
                      // 3x3 inner block. Easier to do this for every column,
                      // but not required.
                      if (innerColumn == sectionSize - 1) {
                        Ansi::MoveRight(1);
                      }

                      // Reset cursor to left of inner block when row is filled
                      if (innerColumn == sectionSize - 1) {
                        Ansi::MoveLeft(sectionSize + 1);
                      }

                      counter++;
                    });

    // Set cursor to top left of next block
    Ansi::MoveRight(sectionSize + 1);
    Ansi::MoveUp(sectionSize - 1);

    // Move cursor back to the left when the last grid on a row is complete
    std::size_t column{idx % sectionSize};
    if (column == sectionSize - 1) {
      Ansi::MoveLeft((sectionSize + 1) * 3);
    }
  }

  Ansi::BackToSaved();
  PrintDividers(size);
  Ansi::BackToSaved();
}

// class PrinterLogic final {
// public:
//   PrinterLogic() = delete;
//   PrinterLogic(std::size_t size);
//   ~PrinterLogic() = default;

//   void PrintSingle(const std::string_view &str);
//   const std::size_t SectionSize() const;
  
// private:
//   const std::size_t size;
//   const std::size_t sectionSize;
// };

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
