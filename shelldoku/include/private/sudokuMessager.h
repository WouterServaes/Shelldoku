#pragma once
#include "ansiUiFramework.h"
#include "listener.h"
#include <cassert>
#include <utility>

// should this message printer and the sudoku printer be objects instead?
// The static variables should be known from wherever
// Maybe some small data (positions) in an object, but general move and print
// function should be generic? relative position should be known to the specific
// messager/field object they should all know the Ansi saved position is 0,0 of
// sudoku (top left) the messager object should be able to hierarchicly move
// through different field (score, difficulty, tries, time, ...)

namespace SudokuMessageField {
static void
PrepareMessageField(std::pair<unsigned int, unsigned int> relativePosition_);
static void CursorToMessageField();
static void CursorToNormalMessage();
static void NormalMessage(const std::string_view &msg);

static void
PrepareMessageField(std::pair<unsigned int, unsigned int> relativePosition_) {
  CursorToMessageField();
  std::cout << "═══════════";
  Ansi::MoveDown();
  Ansi::MoveLeft(10);
  std::cout << "SHELLDOKU";
  Ansi::MoveDown(2);
  Ansi::MoveLeft(10);
  std::cout << "═══════════";
  Ansi::BackToSaved();
}

static void CursorToMessageField() {

  // if (Ansi::GetCursorPosition() != AbsolutePosition) {
  Ansi::BackToSaved();
  // AbsolutePosition = Ansi::GetCursorPosition();
  // }
}

static void CursorToNormalMessage() {
  CursorToMessageField();
  Ansi::MoveDown(2);
  Ansi::MoveRight();
}

static void NormalMessage(const std::string_view &msg) {
  CursorToNormalMessage();
  std::cout << msg;
  Ansi::BackToSaved();
}

} // namespace SudokuMessageField

class SudokuMessager final : public Listener {
public:
  virtual void Notify(EventID eventId);
};
