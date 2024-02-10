#include "shelldokuPrinter.h"
#include "sudokuGenerator.h"
#include "sudokuHelpers.h"
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

struct ArgOptions {
  unsigned int size{9};
  unsigned int threads{1};
  std::chrono::seconds maxRunTime{5};
};

[[nodiscard]] ArgOptions ParseArgs(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  const auto options{ParseArgs(argc, argv)};

  auto generation{[options]() {
    SudokuGenerator sudokuGenerator{};
    Generator generator{options.size, options.size / 3, options.maxRunTime,
                        GeneratorTypes::Shift};
    std::string msg{"generation "};
    if (sudokuGenerator.Generate(generator)) {
      msg += "complete";
    } else {
      msg += "failed";
    }
    msg += std::string(" - after ") +
           std::to_string(sudokuGenerator.TotalTries()) + " tries";
    std::cout << msg << std::endl;
    // ShelldokuPrinter::PrintSingleLine(generator.values);
    // Ansi::SaveCursorPos();
    // ShelldokuPrinter::FillCout(generator.size);
    // Ansi::BackToSaved();
    // ShelldokuPrinter::PrintSudoku(generator.values, generator.size);
  }};

  std::vector<std::thread> threads;
  threads.resize(options.threads);
  for (auto &thr : threads) {
    thr = std::thread(generation);
  }

  for (auto &thr : threads) {
    thr.join();
  }
  std::cin.get();
}

ArgOptions ParseArgs(int argc, char *argv[]) {
  int opt{};
  ArgOptions options{};
  static struct option long_options[] = {
      {"help", no_argument, 0, 0},
      {"size", required_argument, 0, 9},
      {"threads", required_argument, 0, 1},
      {"maxRunTime", required_argument, 0, 100}};
  int option_index{};
  while ((opt = getopt_long(argc, argv, "hs:j:t:", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
      std::cout << "help" << std::endl;
      break;
    case 's':
      options.size = std::stoi(optarg);
      break;
    case 'j':
      options.threads = std::stoi(optarg);
      break;
    case 't':
      options.maxRunTime = std::chrono::seconds(std::stoi(optarg));
      break;
    }
  }

  return options;
}