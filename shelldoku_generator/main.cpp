#include "logger.h"
#include "sudokuGenerator.h"
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>

struct ArgOptions {
  unsigned int size{9};
  unsigned int threads{1};
  unsigned int maxTries{99};
  std::chrono::seconds maxRunTime{100};
};

[[nodiscard]] ArgOptions ParseArgs(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  Log::Debug("hello world");
  const auto options{ParseArgs(argc, argv)};
  Generator generator{options.size, options.size / 3, options.maxRunTime};

  std::thread generationThread([&generator, options]() {
    SudokuGenerator sudokuGenerator{};
    if (sudokuGenerator.Generate(generator)) {
      std::cout << "generation complete";
    } else {
      std::cout << "generaton failed";
    }
    std::cout << " - thread: " << std::this_thread::get_id() << std::endl;
  });

  generationThread.join();
}

ArgOptions ParseArgs(int argc, char *argv[]) {
  int opt{};
  ArgOptions options{};
  static struct option long_options[] = {
      {"help", no_argument, 0, 0},
      {"size", required_argument, 0, 9},
      {"threads", required_argument, 0, 1},
      {"maxTries", required_argument, 0, 99},
      {"maxRunTime", required_argument, 0, 100}};
  int option_index{};
  while ((opt = getopt_long(argc, argv, "hs:j:r:t:", long_options,
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
    case 'r':
      options.maxTries = std::stoi(optarg);
      break;
    case 't':
      options.maxRunTime = std::chrono::seconds(std::stoi(optarg));
      break;
    }
  }

  return options;
}