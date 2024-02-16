#include "sudokuMessager.h"
#include "sudoku.h"

void SudokuMessager::Notify(EventID eventID) {
  switch (eventID) {
  case EVENT_ID::SUDOKU_PLACE:
    SudokuMessageField::NormalMessage("placed");
    break;
  case EVENT_ID::SUDOKU_FAIL:
    SudokuMessageField::NormalMessage("failed");
    break;
  case EVENT_ID::SUDOKU_SOLVED:
    SudokuMessageField::NormalMessage("solved");
    break;
  default:
    break;
  }
}
