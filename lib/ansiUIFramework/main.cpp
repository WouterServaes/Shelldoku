#include "ansiUiFramework.h"

int main(int argc, char *argv[]) {
  // clang-format off
  ANSI_UI()->Box("test1", {{1, 1}, {10, 0}});

    ANSI_UI()->Box("test1.1", {{2, 1}, {10, 0}});
    ANSI_UI()->EndBox();
  
    ANSI_UI()->Box("test1.2", {{3, 2}, {}});
      ANSI_UI()->Box("test2", {{}, {}});
      ANSI_UI()->EndBox();
    ANSI_UI()->EndBox();

  ANSI_UI()->EndBox();

  // ANSI_UI()->Box("test throw", {{}, {}});
  // ANSI_UI()->EndBox();
  // clang-format on
  std::cin.get();
}
