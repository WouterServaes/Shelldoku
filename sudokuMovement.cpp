#include "include/public/sudokuMovement.h"
#include "include/public/ansi.h"
#include "common/include/public/logger.h"

#include <math.h>
#include <string>

SudokuMovement::SudokuMovement(int sectionSize)
:cursorPosition({0,0}), sectionSize(sectionSize)
{}

// positioning layout:
// section dividers should be skipped, and are not counted as location
// 012│345│678
// 1  │   │
// 2  │   │
// ───┼───┼───
// 3  │   │
// 4  │   │
// 5  │   │
// ───┼───┼───
// 6  │   │
// 7  │   │
// 8  │   │

void SudokuMovement::UpdatePosition(std::pair<int, int> pos)
{
  cursorPosition.first += pos.first;
  cursorPosition.second += pos.second;

  int posX = pos.first;
  int posY = pos.second;

  const std::pair dir{posX > 0, posY > 0};

  posX = std::abs(posX);
  posY = std::abs(posY);
  
  if (posX) {
    if(cursorPosition.first) {
      if(dir.first && !(cursorPosition.first % (sectionSize))) {
        Ansi::MoveRight();
      } else if(!dir.first && !((cursorPosition.first + 1)% (sectionSize))) {
        Ansi::MoveLeft();
      }
    }
    dir.first  ? Ansi::MoveRight(posX) : Ansi::MoveLeft(posX);
  }

  if (posY) {
    if(cursorPosition.second) {
      if(dir.second && !((cursorPosition.second) % (sectionSize))) {
        Ansi::MoveDown();
      } else if(!dir.second && !((cursorPosition.second + 1)% (sectionSize))) {
        Ansi::MoveUp();
      }
    }
    dir.second ? Ansi::MoveDown(posY)  : Ansi::MoveUp(posY);
  }
}