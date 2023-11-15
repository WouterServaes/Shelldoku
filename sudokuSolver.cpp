#include "include/public/sudokuSolver.h"
#include "include/public/sudokuHelpers.h"
#include "common/include/public/logger.h"
#include <string>

SudokuSolver::SudokuSolver(const std::size_t size, const std::size_t sectionSize)
  :size(size), sectionSize(sectionSize)
{}

bool SudokuSolver::CanBeSolved(const std::vector<SudokuValue>& values) noexcept{
  std::vector<SudokuValue>& temp = const_cast<std::vector<SudokuValue>&>(values);
  return Solve(temp);
}

//==================
// SudokuSolver_bitmasks
///==============

SudokuSolver_bitmasks::SudokuSolver_bitmasks(const std::size_t size, const std::size_t sectionSize)
  :SudokuSolver(size, sectionSize)
{}


bool SudokuSolver_bitmasks::Solve(std::vector<SudokuValue>& values) noexcept
{
  GenerateBitMasks(values);

  return Solve(values, {0,0});
}

bool SudokuSolver_bitmasks::CanBePlaced(const std::vector<SudokuValue>& values, ValueLocation location, SudokuValue value) const noexcept
{
  if(!value.has_value()
  || !XYToSquareIndex(size, sectionSize, location).has_value()
  || !GeneratedBitMasks()
  || location.first >= size
  || location.second >= size) {
    return false;
  }

  return !((bitMasksColumns[location.first] >> value.value()) & 1)
      && !((bitMasksRows[location.second] >> value.value()) & 1)
      && !((bitMasksSquares[XYToSquareIndex(size, sectionSize, location).value()] >> value.value()) & 1);
}

void SudokuSolver_bitmasks::GenerateBitMasks(const std::vector<SudokuValue>& toGenerateFrom)
{
  if(GeneratedBitMasks()) {
    return;
  }

  bitMasksRows.clear();
  bitMasksColumns.clear();
  bitMasksSquares.clear();

  bitMasksRows.resize(size);
  bitMasksColumns.resize(size);
  bitMasksSquares.resize(size);

  for(unsigned int r{}; r < size; r++) {
    for(unsigned int c{}; c < size; c++) {
      const auto v = toGenerateFrom.at(XYToSudokuPos(size, {c,r}));
      SetBitMasks({c,r}, v);
    }
  }
}

void SudokuSolver_bitmasks::ClearBitMasks()
{
  bitMasksRows.clear();
  bitMasksColumns.clear();
  bitMasksSquares.clear();
}

void SudokuSolver_bitmasks::SetBitMasks(ValueLocation location, SudokuValue value) noexcept
{
  if(!value.has_value()
  || !XYToSquareIndex(size, sectionSize, location).has_value()
  || !GeneratedBitMasks()
  || location.first >= size
  || location.second >= size) {
    return;
  }
  
  const auto v = (value ? value.value() : 0);

  bitMasksColumns[location.first] |= 1 << v;
  bitMasksRows[location.second] |= 1 << v;
  bitMasksSquares[XYToSquareIndex(size, sectionSize, location).value()] |= 1 << v;
}

void SudokuSolver_bitmasks::ResetBitMasks(ValueLocation location, SudokuValue value) noexcept
{
  if(!XYToSquareIndex(size, sectionSize, location).has_value()) {
    return;
  }

  if(!value.has_value()) {
    return;
  }

  bitMasksColumns[location.first] &= ~(1 << value.value());
  bitMasksRows[location.second] &= ~(1 << value.value());
  bitMasksSquares[XYToSquareIndex(size, sectionSize, location).value()] &= ~(1 << value.value());
}

bool SudokuSolver_bitmasks::GeneratedBitMasks() const noexcept
{
  return (!(bitMasksRows.empty() || bitMasksColumns.empty() || bitMasksSquares.empty()))
   && bitMasksRows.size() == size && bitMasksColumns.size() == size && bitMasksSquares.size() == size;
}

bool SudokuSolver_bitmasks::Solve(std::vector<SudokuValue>& values, ValueLocation location)
{
  if(location.first == size - 1 
  && location.second == size) {
    return true;
  }

  if(location.second == size) {
    location.first++;
    location.second = 0;
  }

  if(XYToSudokuPos(size, location) >= values.size()) {
    return false;
  }

  if(values[XYToSudokuPos(size, location)]) {
    return Solve(values, {location.first, location.second + 1});
  }

  for(int value{ 1 }; value <= size; value++) {
    if(CanBePlaced(values, location, value)) {
      values[XYToSudokuPos(size, location)] = value;
      SetBitMasks(location, value);
      if(Solve(values, {location.first, location.second + 1})) {
        return true;
      }
      ResetBitMasks(location, value);
    }
    values[XYToSudokuPos(size, location)].reset();
  }

  return false;
}

bool SudokuSolver_bitmasks::ValidateSudoku(const std::vector<SudokuValue>& values) noexcept 
{
  GenerateBitMasks(values);
  auto cV{values};

  for(std::size_t idx{}; idx < values.size(); idx++) {
    const auto v = values[idx];
    if(!v.has_value()) {
      continue;
    }
    // remove the current value
    cV[idx].reset();
    ClearBitMasks();
    GenerateBitMasks(cV);
    // try to place it
    if(!CanBePlaced(cV, SudokuPosToXY(size, idx), v)) {
      ClearBitMasks();
      return false;
    }
    // set it again
    cV[idx] = v;
    SetBitMasks(SudokuPosToXY(size, idx), v);
  }

  ClearBitMasks();
  return true;
}

bool SudokuSolver_bitmasks::CanBeSolved(const std::vector<SudokuValue>& values) noexcept
{
  const auto r{SudokuSolver::CanBeSolved(values)};
  ClearBitMasks();
  return r;
}
