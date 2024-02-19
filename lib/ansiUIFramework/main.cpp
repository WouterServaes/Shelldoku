#include "ansi.h"
#include "ansiUiFramework.h"
#include <chrono>
#include <ostream>
#include <thread>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // clang-format off
  ANSI_UI()->Box("test1", {0, 0});
  //std::cin.get();
    ANSI_UI()->Box("test1.1", {0, 0});
    //ANSI_UI()->Item("test");
  //std::cin.get();
    ANSI_UI()->EndBox();
  
    ANSI_UI()->Box("test1.2", {50, 0});
      ANSI_UI()->Box("test2", {0, 10});
      ANSI_UI()->EndBox();
    ANSI_UI()->EndBox();

  ANSI_UI()->EndBox();

  // ANSI_UI()->Box("test throw", {{}, {}});
  // ANSI_UI()->EndBox();
  // clang-format on
  std::cin.get();
}
