#include "sudokuGenerator.h"
#include "sudokuHelpers.h"
#include "sudokuParser.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include <vector>

static const std::string FILE_LOCATION = "shelldoku_generator/";
static const std::string FILE_NAME = "sudoku";
static const std::string FILE_EXTENSION = ".txt";
static const std::string FILES_TMP_DIR =
    std::filesystem::temp_directory_path().string() + "/" + FILE_LOCATION;

struct ArgOptions {
  unsigned int size{9};
  unsigned int threads{1};
  std::chrono::seconds maxRunTime{5};
  unsigned int count{1};
};

[[nodiscard]] ArgOptions ParseArgs(int argc, char *argv[]);

std::string getTempFileLoc(std::thread::id threadId);
void tempFilesToMainFile();

int main(int argc, char *argv[]) {
  const auto options{ParseArgs(argc, argv)};
  // thread lambda
  auto generation{[options]() {
    SudokuGenerator sudokuGenerator{};
    Generator generator{options.size, options.size / 3, options.maxRunTime,
                        GeneratorTypes::Shift};
    unsigned int sudokuCount = options.count;
    std::string fileLoc = getTempFileLoc(std::this_thread::get_id());
    for (; sudokuCount; sudokuCount--) {
      std::string msg{"generation "};
      if (sudokuGenerator.Generate(generator)) {
        msg += "complete";
      } else {
        msg += "failed";
      }
      msg += std::string(" - after ") +
             std::to_string(sudokuGenerator.TotalTries()) + " tries";
      std::cout << msg << std::endl;

      sudokuParser::ParseToFile(generator.values, fileLoc);
      sudokuGenerator.Reset();
    }
  }};
  // threads
  std::vector<std::thread> threads;
  threads.resize(options.threads);

  for (auto &thr : threads) {
    thr = std::thread(generation);
  }

  for (auto &thr : threads) {
    thr.join();
  }
  // tmp thread files to single main file
  tempFilesToMainFile();
  // end
  std::cin.get();
}

ArgOptions ParseArgs(int argc, char *argv[]) {
  int opt{};
  ArgOptions options{};
  static struct option long_options[] = {
      {"help", no_argument, 0, 0},
      {"size", required_argument, 0, 9},
      {"threads", required_argument, 0, 1},
      {"maxRunTime", required_argument, 0, 100},
      {"count", required_argument, 0, 1}};
  int option_index{};
  while ((opt = getopt_long(argc, argv, "hs:j:t:c:", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
      std::cout << "help" << std::endl;
      break;
    case 's':
      options.size = std::stoi(optarg);
      break;
    case 'j':
      // options.threads = std::stoi(optarg);
      std::cout << "Mutlithreaded does not work\n";
      break;
    case 't':
      options.maxRunTime = std::chrono::seconds(std::stoi(optarg));
      break;
    case 'c':
      options.count = std::stoi(optarg);
      break;
    }
  }

  return options;
}

std::string getTempFileLoc(std::thread::id threadId) {
  std::stringstream ss;
  ss << threadId;
  ;
  std::filesystem::create_directory(FILES_TMP_DIR);
  return FILES_TMP_DIR + ss.str() + FILE_EXTENSION;
}

void tempFilesToMainFile() {
  static const std::string MAIN_FILE_STR =
      FILE_LOCATION + FILE_NAME + FILE_EXTENSION;
  std::filesystem::create_directory(FILE_LOCATION);
  std::ofstream mainFile;
  mainFile.open(MAIN_FILE_STR, std::ios_base::app);

  // concat main files by all tmp files
  // access all tmp files by using filesystem iterator
  for (const auto &tmpFileStr :
       std::filesystem::directory_iterator(FILES_TMP_DIR)) {
    std::ifstream tmpFile;
    tmpFile.open(tmpFileStr.path());
    mainFile << tmpFile.rdbuf();
    tmpFile.close();
  }
  // clean up tmp files
  std::filesystem::remove_all(FILES_TMP_DIR);
}
