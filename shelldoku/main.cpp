
#include "eventID.h"
#include "eventQueue.h"
#include "events.h"
#include "listener.h"

#include "logger.h"
#include "sudoku.h"
#include "sudokuHelpers.h"
#include "sudokuMovement.h"
#include "sudokuParser.h"

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
  bool generate{true};
  std::string file;
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

  InputHandling::Input input{pEventQueue};

  Sudoku sudoku;
  Solver solver{size, size / 3, SolverTypes::Bitstring};
  if (options.generate) {
    sudoku = Sudoku(size);
    Generator settings{size, sudoku.SectionSize(), std::chrono::seconds(60),
                       GeneratorTypes::Shift};
    sudoku.GenerateSudoku(settings);
  } else {
    std::vector<SudokuValue> pregeneratedValues{};
    sudokuParser::ParseFromFile(pregeneratedValues, options.file);
    sudoku = Sudoku(size, pregeneratedValues);
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
  while (*IS_RUNNING) {
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
                                         {"file", no_argument, 0, false}};
  int option_index{};
  while ((opt = getopt_long(argc, argv, "hs:f:", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
      std::cout << "HELP" << std::endl;
      break;
    case 's':
      std::cout << " -S <> NOT USED" << std::endl;
      settings.size = std::stoi(optarg);

      break;
    case 'f':
      settings.generate = false;
      settings.file = optarg;
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

  auto placeOnPosition{[&sudoku, &positioner, pEventQueue](unsigned int value){
    Log::Debug(std::string("pos: " + std::to_string(positioner.GetPosition().first) + std::string(" ") + std::to_string(positioner.GetPosition().second)));
    if(sudoku.PlaceValue(positioner.GetPosition(), value)) {
      Dispatcher dis(pEventQueue);
      ShelldokuPrinter::PrintSingle(value ? std::to_string(value) : " ");
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

}
