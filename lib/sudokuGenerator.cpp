#include "include/public/sudokuGenerator.h"
#include "include/private/sudokuGenerator_.h"
#include <memory>

SudokuGenerator::SudokuGenerator()
:pSudokuGenerator(std::unique_ptr<SudokuGenerator_>(new SudokuGenerator_()))
{}

bool SudokuGenerator::Generate(Generator& generator)
{
  return pSudokuGenerator->Generate(generator);
}
