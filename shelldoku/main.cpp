
#include "eventID.h"
#include "eventQueue.h"
#include "events.h"
#include "listener.h"

#include "logger.h"
#include "sudoku.h"
#include "sudokuHelpers.h"
#include "sudokuMovement.h"

#include "sudokuGenerator.h"
#include "sudokuSolver.h"

#include <chrono>
#include <functional>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <ostream>
#include <pthread.h>
#include <string>

struct ArgOptions {
  unsigned int size{9};
  bool generate{false};
};

[[nodiscard]] ArgOptions ParseArgs(int argc, char *argv[]);
void CreateInputMap(InputHandling::Input &input, Sudoku &sudoku,
                    SudokuMovement &positioner,
                    std::shared_ptr<EventQueue> pEventQueue);

std::shared_ptr<bool> IS_RUNNING{new bool(true)};
void SetIsRunning(std::shared_ptr<bool> pIsRunning, bool running) {
  *pIsRunning = running;
}

int main(int argc, char *argv[]) {

  const auto options{ParseArgs(argc, argv)};
  const unsigned int size{options.size};

  ShelldokuPrinter::PrepareSudokuField(size);

  // create queue object
  std::shared_ptr<EventQueue> pEventQueue{new EventQueue()};

  // input is a dispacher object
  InputHandling::Input input{pEventQueue};
  // Listener test{};
  // test.Listen(&eventQueue, EVENT_ID::SUDOKU_PLACE);

  // Sudoku sudoku(size,
  // {4,{},{},{},{},{},8,{},5,{},3,{},{},{},{},{},{},{},{},{},{},7,{},{},{},{},{},{},2,{},{},{},{},{},6,{},{},{},{},{},8,{},4,{},{},{},{},{},{},1,{},{},{},{},{},{},{},6,{},3,{},7,{},5,{},{},2,{},{},{},{},{},1,{},4,{},{},{},{},{},{}});

  // Sudoku sudoku{size, std::unique_ptr<SudokuSolver_bitmasks>(new
  // SudokuSolver_bitmasks(size, size/3)),{ 3, 0, 6, 5, 0, 8, 4, 0, 0, 5, 2, 0,
  // 0, 0, 0, 0, 0, 0, 0, 8, 7, 0, 0, 0, 0, 3, 1, 0, 0, 3, 0, 1, 0, 0, 8, 0, 9,
  // 0, 0, 8, 6, 3, 0, 0, 5, 0, 5, 0, 0, 9, 0, 6, 0, 0, 1, 3, 0, 0, 0, 0, 2, 5,
  // 0, 0, 0, 0, 0, 0, 0, 0, 7, 4, 0, 0, 5, 2, 0, 6, 3, 0, 0}};
  Sudoku sudoku{Sudoku(size)};
  Solver solver{size, size / 3, SolverTypes::Bitstring};
  if (options.generate) {
    Generator settings{size, sudoku.SectionSize(), std::chrono::seconds(60),
                       GeneratorTypes::Shift};
    sudoku.GenerateSudoku(settings);
  }

  SudokuMovement positioner{static_cast<unsigned int>(sudoku.SectionSize())};

  // create input map
  CreateInputMap(input, sudoku, positioner, pEventQueue);

  ShelldokuPrinter::PrintSudoku(sudoku.GetValues(), size);

  input.Init();
  input.StartInputHandler();

  if (sudoku.IsSolvable()) {
    Log::Debug("can be solved");

  } else {
    Log::Debug("can NOT be solved");
  }
  sudoku.Start();
  ShelldokuPrinter::PrintSudoku(sudoku.GetValues(), size);

  while (IS_RUNNING) {
    // Handle the input events, waits for events to continue
    pEventQueue->HandleQueue(true);
  }

  std::cin.get();
  Ansi::MoveDown(5);
  Ansi::Cleanup();
  input.End();
  return 0;
}

ArgOptions ParseArgs(int argc, char *argv[]) {
  int opt{};
  ArgOptions settings{};
  static struct option long_options[] = {{"help", no_argument, 0, 0},
                                         {"size", required_argument, 0, 9},
                                         {"generate", no_argument, 0, false}};
  int option_index{};
  while ((opt = getopt_long(argc, argv, "hs:g", long_options, &option_index)) !=
         -1) {
    switch (opt) {
    case 'h':
      std::cout << "HELP" << std::endl;
      break;
    case 's':
      std::cout << " -S <> NOT USED" << std::endl;
      settings.size = std::stoi(optarg);

      break;
    case 'g':
      settings.generate = true;
    }
  }

  return settings;
}

void CreateInputMap(InputHandling::Input &input, Sudoku &sudoku,
                    SudokuMovement &positioner,
                    std::shared_ptr<EventQueue> pEventQueue) {

  using intintArg = std::pair<int, int>;
  using intintFunction = FunctionEvent<intintArg>;
  using sudokuFunction = FunctionEvent<int>;
  // clang-format off
  auto positionerFunction{std::bind(&SudokuMovement::UpdatePosition, &positioner, std::placeholders::_1)};
  input.AddKey(Ansi::ANSI_UP,     {KEY_UP,    std::make_shared<intintFunction>(intintFunction( EVENT_ID::MOVE,positionerFunction ,intintArg{0, -1}))});
  input.AddKey(Ansi::ANSI_DOWN,   {KEY_DOWN,  std::make_shared<intintFunction>(intintFunction( EVENT_ID::MOVE, positionerFunction,intintArg{0,1}))});
  input.AddKey(Ansi::ANSI_RIGHT,  {KEY_RIGHT, std::make_shared<intintFunction>(intintFunction( EVENT_ID::MOVE, positionerFunction,intintArg{1, 0}))});
  input.AddKey(Ansi::ANSI_LEFT,   {KEY_LEFT,  std::make_shared<intintFunction>(intintFunction( EVENT_ID::MOVE, positionerFunction,intintArg{-1, 0}))});

  auto quitFunction{std::make_shared<FunctionEvent<std::shared_ptr<bool>, bool>>(FunctionEvent<std::shared_ptr<bool>, bool>(EVENT_ID::STOP, &SetIsRunning, {IS_RUNNING, false}))};
  input.AddKey(Ansi::ANSI_ESCAPE, {KEY_ESC, quitFunction});
  input.AddKey("Q", {KEY_Q, quitFunction});

  // use std::bind to create a function ptr to a member function
  // PrintSingle should be called from pPrinterLogic, with a single argument
  //auto sudokuPlace = std::bind(&Sudoku::PlaceValue, pPrinterLogic, std::placeholders::_1);

  auto placeOnPosition{[&sudoku, &positioner, pEventQueue](unsigned int value){
    Log::Debug(std::string("pos: " + std::to_string(positioner.GetPosition().first) + std::string(" ") + std::to_string(positioner.GetPosition().second)));
    if(sudoku.PlaceValue(positioner.GetPosition(), value)) {
      Dispatcher dis(pEventQueue);
      ShelldokuPrinter::PrintSingle(std::to_string(value));
      dis.DispatchEvent(EVENT_ID::PRINT);
    }
  }};

  input.AddKey("1", {KEY_1, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 1))});
  input.AddKey("2", {KEY_2, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 2))});
  input.AddKey("3", {KEY_3, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 3))});
  input.AddKey("4", {KEY_4, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 4))});
  input.AddKey("5", {KEY_5, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 5))});
  input.AddKey("6", {KEY_6, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 6))});
  input.AddKey("7", {KEY_7, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 7))});
  input.AddKey("8", {KEY_8, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 8))});
  input.AddKey("9", {KEY_9, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 9))});
  input.AddKey("0", {KEY_0, std::make_shared<sudokuFunction>(sudokuFunction(EVENT_ID::SUDOKU_PLACE, placeOnPosition, 0))});
  
  auto ready{[&sudoku, pEventQueue](){
    Dispatcher dis(pEventQueue);
    if(sudoku.IsSolved()) {
      dis.DispatchEvent(EVENT_ID::SUDOKU_SOLVED);
      sudoku.Stop();
    } else {
      dis.DispatchEvent(EVENT_ID::SUDOKU_FAIL);
    }
  }};

  input.AddKey("R", {KEY_R, std::make_shared<FunctionEvent<>>(FunctionEvent<>(EVENT_ID::READY, ready))});

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
