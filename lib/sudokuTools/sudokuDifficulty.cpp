#include "sudokuDifficulty.h"
#include "sudokuHelpers.h"
#include <algorithm>
#include <map>
#include <ranges>

// The difficulty score:
// Add or subtract a score for each condition

// less than 9 missing values total
// only 1 missing value in a square
// only 1 missing value on a line
// only 1 of a specific number missing on all vertical or horizontal lines
// more than 3 missing values in a square
// more than 3 missing values on a line
// a specific value missing only 1
// a specific value missing more than 2

namespace ScoreDelta {
static const int FewMissing = -10;
static const int NoMissingInSquare = -15;
static const int OnlyOneInSquare = -12;
static const int OnlyOneInLine = -5;
static const int OnlyOneOfNumberInAdjLines = -1;
static const int MoreThanThreeInSquare = 13;
static const int MoreThanThreeInLine = 15;
static const int SpecificValueNoMissing = -5;
static const int SpecificValueSingleMissing = -2;
static const int SpecificValueMissingMoreThanTwo = 5;
}; // namespace ScoreDelta

static const int FewMissing = 9;

namespace sudokuDifficulty {
Difficulty GetDifficulty(unsigned int score);

Difficulty CalculateDifficulty(const std::vector<SudokuValue> &sudoku,
                               const std::size_t size,
                               const std::size_t sectionSize) {
  static const SudokuValue EMPTY_VALUE = SudokuValue();

  auto score(static_cast<int>(Difficulty::normal));

  // total
  {
    const auto missingCount{
        std::count(sudoku.begin(), sudoku.end(), EMPTY_VALUE)};
    if (missingCount < FewMissing) {
      score += ScoreDelta::FewMissing;
    }
  }

  // specific numbers
  {
    for (auto valueIdx : std::ranges::iota_view(0, static_cast<int>(size))) {
      const auto missingCount =
          std::count(sudoku.begin(), sudoku.end(), SudokuValue(valueIdx));
      if (missingCount == 0) {
        score += ScoreDelta::SpecificValueNoMissing;
      } else if (missingCount == 1) {
        score += ScoreDelta::SpecificValueSingleMissing;
      } else {
        score += ScoreDelta::SpecificValueMissingMoreThanTwo;
      }
    }
  }

  // square
  {
    for (auto squareIdx : std::ranges::iota_view(0, static_cast<int>(size))) {
      auto allIndexesOfSqr{GetAllIndexesOfSquare(size, sectionSize, squareIdx)};
      unsigned int missingCount{0};
      for (auto idx : allIndexesOfSqr) {
        if (sudoku.at(idx) == EMPTY_VALUE) {
          missingCount++;
        }
      }

      if (missingCount == 0) {
        score += ScoreDelta::NoMissingInSquare;
      } else if (missingCount == 1) {
        score += ScoreDelta::OnlyOneInSquare;
      } else if (missingCount >= 3) {
        score += ScoreDelta::MoreThanThreeInSquare;
      }
    }
  }

  // columns
  {
    std::map<unsigned int, unsigned int> columnValuesCount{};
    for (auto columnIdx : std::ranges::iota_view(0, static_cast<int>(size))) {
      auto allIndexesOfColumn{GetAllIndexesOfColumn(size, columnIdx)};
      unsigned int missingCount{0};
      for (auto idx : allIndexesOfColumn) {
        if (sudoku.at(idx) == EMPTY_VALUE) {
          missingCount++;
        } else {
          columnValuesCount[sudoku.at(idx).value()]++;
        }
      }

      if (missingCount == 1) {
        score += ScoreDelta::OnlyOneInLine;
      } else if (missingCount >= 3) {
        score += ScoreDelta::MoreThanThreeInLine;
      }
    }

    for (auto valueCount : columnValuesCount) {
      if (valueCount.second == size - 1) {
        score += ScoreDelta::OnlyOneOfNumberInAdjLines;
      }
    }
  }

  // rows
  {
    std::map<unsigned int, unsigned int> rowValuesCount{};
    for (auto lineIdx : std::ranges::iota_view(0, static_cast<int>(size))) {
      auto allIndexesOfRow{GetAllIndexesOfRow(size, lineIdx)};
      unsigned int missingCount{0};
      for (auto idx : allIndexesOfRow) {
        if (sudoku.at(idx) == EMPTY_VALUE) {
          missingCount++;
        } else {
          rowValuesCount[sudoku.at(idx).value()]++;
        }
      }

      if (missingCount == 1) {
        score += ScoreDelta::OnlyOneInLine;
      } else if (missingCount >= 3) {
        score += ScoreDelta::MoreThanThreeInLine;
      }
    }
    for (auto valueCount : rowValuesCount) {
      if (valueCount.second == size - 1) {
        score += ScoreDelta::OnlyOneOfNumberInAdjLines;
      }
    }
  }

  return GetDifficulty(score < 0 ? 0 : score);
}

Difficulty GetDifficulty(unsigned int score) {
  // 0 - 25 => easy
  if (score < (static_cast<unsigned int>(Difficulty::normal) -
               static_cast<unsigned int>(Difficulty::easy)) /
                  2) {
    return Difficulty::easy;
  }

  // 25 - 75 => normal
  if (score < static_cast<unsigned int>(Difficulty::normal) +
                  ((static_cast<unsigned int>(Difficulty::hard) -
                    static_cast<unsigned int>(Difficulty::normal)) /
                   2)) {
    return Difficulty::normal;
  }

  // 75+ => hard
  return Difficulty::hard;
}
} // namespace sudokuDifficulty