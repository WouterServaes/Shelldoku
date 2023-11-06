
#include "events/include/public/eventQueue.h"
#include "events/include/public/events.h"
#include "include/public/ansi.h"
#include "include/public/input.h"
#include "include/public/shelldokuPrinter.h"
#include "include/public/sudoku.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <pthread.h>
#include <random>
#include <thread>
#include <vector>

void ParseArgs(int argc, char *argv[]);
void CreateInputMap(InputHandling::Input *pInput,
                    shelldokuPrinter::PrinterLogic *pPrinterLogic);

bool IS_RUNNING{true};
void SetIsRunning(bool *pIsRunning, bool running) { *pIsRunning = running; }

int main(int argc, char *argv[]) {

  ParseArgs(argc, argv);
  const int size{9};
  const int sectionSize{size / 3};

  std::cout << std::endl;
  std::cout << std::endl;

  Ansi::SaveCursorPos();

  // create queue object
  EventQueue eventQueue{};

  shelldokuPrinter::PrinterLogic printerLogic(static_cast<std::size_t>(size));

  // input is a dispacher object
  InputHandling::Input input{&eventQueue};

  // create input map
  CreateInputMap(&input, &printerLogic);

  Sudoku sudoku(size);
  sudoku.GenerateSudoku();

  shelldokuPrinter::PrintSudoku(sudoku.getValues(), size);

  input.Init();
  input.StartInputHandler();

  while (IS_RUNNING) {

    eventQueue.HandleQueue();
  }

  Ansi::MoveDown(5);
  Ansi::Cleanup();
  input.End();
  return 0;
}

void ParseArgs(int argc, char *argv[]) {
  int opt{};

  static struct option long_options[] = {{"help", no_argument, 0, 0},
                                         {"size", required_argument, 0, 0}};
  int option_index{};
  while ((opt = getopt_long(argc, argv, "hs:", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'h':
      std::cout << "HELP" << std::endl;
      break;
    case 's':
      std::cout << optarg << " NOT USED" << std::endl;
      break;
    }
  }
}

void CreateInputMap(InputHandling::Input *pInput,
                    shelldokuPrinter::PrinterLogic *pPrinterLogic) {

  using intFunction = FunctionEvent<int>;
  using strViewFunction = FunctionEvent<std::string_view>;
  using sudokuFunction = FunctionEvent<Sudoku::ValueLocation, int>;
  // clang-format off
  pInput->AddKey(Ansi::ANSI_UP,     {KEY_UP,    std::make_shared<intFunction>(intFunction( EVENT_ID::MOVE, &Ansi::MoveUp,1))});
  pInput->AddKey(Ansi::ANSI_DOWN,   {KEY_DOWN,  std::make_shared<intFunction>(intFunction( EVENT_ID::MOVE, &Ansi::MoveDown,1))});
  pInput->AddKey(Ansi::ANSI_RIGHT,  {KEY_RIGHT, std::make_shared<intFunction>(intFunction( EVENT_ID::MOVE, &Ansi::MoveRight,1))});
  pInput->AddKey(Ansi::ANSI_LEFT,   {KEY_LEFT,  std::make_shared<intFunction>(intFunction( EVENT_ID::MOVE, &Ansi::MoveLeft,1))});

  pInput->AddKey(Ansi::ANSI_ESCAPE, {KEY_ESC, std::make_shared<FunctionEvent<bool*, bool>>(FunctionEvent<bool*, bool>(EVENT_ID::STOP, &SetIsRunning, {&IS_RUNNING, false}))});

  // use std::bind to create a function ptr to a member function
  // PrintSingle should be called from pPrinterLogic, with a single argument
  auto printLogicFunction = std::bind(&shelldokuPrinter::PrinterLogic::PrintSingle, pPrinterLogic, std::placeholders::_1);
  pInput->AddKey("1", {KEY_1, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "1"))});
  pInput->AddKey("2", {KEY_2, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "2"))});
  pInput->AddKey("3", {KEY_3, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "3"))});
  pInput->AddKey("4", {KEY_4, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "4"))});
  pInput->AddKey("5", {KEY_5, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "5"))});
  pInput->AddKey("6", {KEY_6, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "6"))});
  pInput->AddKey("7", {KEY_7, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "7"))});
  pInput->AddKey("8", {KEY_8, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "8"))});
  pInput->AddKey("9", {KEY_9, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "9"))});
  pInput->AddKey("0", {KEY_0, std::make_shared<strViewFunction>(strViewFunction(EVENT_ID::PRINT, printLogicFunction, "0"))});
  // clang-format on

  // a Key should be able to have multiple events assigned;
  // When a key is executed, all it's keys should be added to an event queue
  // All events in the queue are executed on the main thread.
  // Only the input and adding the events to the queue is done on a seperate
  // thread.

  // When a value key is pressed, the value is send to Sudoku::PlaceValue,
  // it is evaluated (can it be placed?), and placed in the values container.
  // If it is placed, a redraw event event is added to the shelldokuPrinter
  // event handler.

  // Now, input handling, game logic, and graphics are all done in seperate
  // threads
}

// 4 nov 2023:
// Multiple ways of event handling:
//
// 1. attach the function to be called to the event itself (current
// implementation),
//    let the eventQueue execute these in PopEvent(). The
//    EventQueue::HandleQueue() calls the PopEvent(), This handle function runs
//    on the main thread, directly called in the main run loop in main().

// 2. The events are just strings ("eventId"). The event Queue HandleQueue() is
// called in main() loop,
//    This handler function notifies the listeners of this event.
//    The listener object should then call the actual function for the event.
//
//
// 3. Both: events hold ids and functionality. If functionality exists for the
// event,
//    The event handler executes it. Event always holds an id, listeners can
//    still listen for event. This makes it so simple event functions are easily
//    implemented (see CreateInputMap), and gives other objects still the
//    possibility to react to events as listener. Event functions are always
//    executed before the event notification

// Position and placing
//
// Input object handles input, fires events on key inputs
//
// Ansi.h holds the move and place functions
//
// Sudoku.h holds the sudoku values
//
// To place a sudoku value, the cursor position should be translated to sudoku
// array position Eg screen pos to world pos...
//
//  -> The 0,0 position is the saved ANSI position.
//  -> ANSI could request the cursor position.
//      The Sudoku pos would be the saved cursor position - current position

//  -> also possible to just ++ or -- the position var whenever a move function
//  is called...
