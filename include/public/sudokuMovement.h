#pragma once
#include <tuple>

class SudokuMovement final {
public:
  SudokuMovement(int sectionSize);
  ~SudokuMovement() = default;

  void UpdatePosition(std::pair<int, int> pos);

private:
  std::pair<int, int> cursorPosition;
  const int sectionSize;
};