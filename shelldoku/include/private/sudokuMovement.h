#pragma once
#include <utility>

class SudokuMovement final {
public:
  SudokuMovement(unsigned int sectionSize);
  ~SudokuMovement() = default;

  // Updates the position.
  // Takes a direction
  void UpdatePosition(std::pair<int, int> direction);

  // Returns the X, Y position
  [[nodiscard]] const std::pair<int, int> GetPosition() const noexcept;

private:
  std::pair<unsigned int, unsigned int> cursorPosition;
  const int sectionSize;
};