#include "sudokuSolver.h"
#include "logger.h"
#include "sudokuSolver_.h"

#include "sudokuHelpers.h"

//================
// public library functions
///================

SudokuSolver::SudokuSolver() : pSudokuSolver(nullptr) {}

SudokuSolver::~SudokuSolver() {
  if (pSudokuSolver) {
    delete pSudokuSolver;
  }
}

bool SudokuSolver::CanBeSolved(const Solver &solver) {
  if (solver.values.empty()) {
    Log::Debug("Empty sudoku can't be solved...");
    return false;
  }
  InitiateSolver(solver.solvertType);
  return pSudokuSolver->CanBeSolved(solver);
}

bool SudokuSolver::CanBePlaced(const Solver &solver, ValueLocation location,
                               SudokuValue value) {
  if (solver.values.empty()) {
    Log::Debug("Can't place on an empty grid...");
    return false;
  }
  InitiateSolver(solver.solvertType);
  return pSudokuSolver->CanBePlaced(solver, location, value);
}

bool SudokuSolver::ValidateSudoku(const Solver &solver) {
  if (solver.values.empty()) {
    Log::Debug("Solver can't validate empty sudoku...");
    return false;
  }
  InitiateSolver(solver.solvertType);
  return pSudokuSolver->ValidateSudoku(solver);
}

void SudokuSolver::InitiateSolver(const SolverTypes solverType) {
  if (pSudokuSolver == nullptr) {
    pSudokuSolver = new SudokuSolver_bitmasks();
  }

  switch (solverType) {
  default:
  case SolverTypes::Bitstring:
  case SolverTypes::None:
    break;
  }
}

bool SudokuSolver::Solve(Solver &solver) {
  if (solver.values.empty()) {
    Log::Debug("Solver solved empty sudoku...");
    return true;
  }
  InitiateSolver(solver.solvertType);
  return pSudokuSolver->Solve(solver);
}

//================
// private library functions
///================

bool SudokuSolver_::CanBeSolved(const Solver &solver) noexcept {
  auto solverCopy = solver;
  return Solve(solverCopy);
}

bool SudokuSolver_bitmasks::Solve(Solver &solver) noexcept {
  GenerateBitMasks(solver);

  return Solve(solver, {0, 0});
}

bool SudokuSolver_bitmasks::CanBePlaced(const Solver &solver,
                                        ValueLocation location,
                                        SudokuValue value) const noexcept {
  if (!value.has_value() ||
      !XYToSquareIndex(solver.size, solver.sectionSize, location).has_value() ||
      !GeneratedBitMasks(solver) || location.first >= solver.size ||
      location.second >= solver.size) {
    return false;
  }

  return !((bitMasksColumns[location.first] >> value.value()) & 1) &&
         !((bitMasksRows[location.second] >> value.value()) & 1) &&
         !((bitMasksSquares[XYToSquareIndex(solver.size, solver.sectionSize,
                                            location)
                                .value()] >>
            value.value()) &
           1);
}

void SudokuSolver_bitmasks::PrepareSudoku(const Solver &solver) noexcept {
  ClearBitMasks();
  GenerateBitMasks(solver);
}

void SudokuSolver_bitmasks::GenerateBitMasks(const Solver &solver) {
  ClearBitMasks();

  bitMasksRows.resize(solver.size);
  bitMasksColumns.resize(solver.size);
  bitMasksSquares.resize(solver.size);

  for (unsigned int r{}; r < solver.size; r++) {
    for (unsigned int c{}; c < solver.size; c++) {
      const auto v = solver.values.at(XYToSudokuPos(solver.size, {c, r}));
      SetBitMasks(solver, {c, r}, v);
    }
  }
}

void SudokuSolver_bitmasks::ClearBitMasks() {
  bitMasksRows.clear();
  bitMasksColumns.clear();
  bitMasksSquares.clear();
}

void SudokuSolver_bitmasks::SetBitMasks(const Solver &solver,
                                        ValueLocation location,
                                        SudokuValue value) noexcept {
  if (!value.has_value() ||
      !XYToSquareIndex(solver.size, solver.sectionSize, location).has_value() ||
      !GeneratedBitMasks(solver) || location.first >= solver.size ||
      location.second >= solver.size) {
    return;
  }

  const auto v = (value ? value.value() : 0);

  bitMasksColumns[location.first] |= 1 << v;
  bitMasksRows[location.second] |= 1 << v;
  bitMasksSquares[XYToSquareIndex(solver.size, solver.sectionSize, location)
                      .value()] |= 1 << v;
}

void SudokuSolver_bitmasks::ResetBitMasks(const Solver &solver,
                                          ValueLocation location,
                                          SudokuValue value) noexcept {
  if (!XYToSquareIndex(solver.size, solver.sectionSize, location).has_value()) {
    return;
  }

  if (!value.has_value()) {
    return;
  }

  bitMasksColumns[location.first] &= ~(1 << value.value());
  bitMasksRows[location.second] &= ~(1 << value.value());
  bitMasksSquares[XYToSquareIndex(solver.size, solver.sectionSize, location)
                      .value()] &= ~(1 << value.value());
}

bool SudokuSolver_bitmasks::GeneratedBitMasks(
    const Solver &solver) const noexcept {
  return (!(bitMasksRows.empty() || bitMasksColumns.empty() ||
            bitMasksSquares.empty())) &&
         bitMasksRows.size() == solver.size &&
         bitMasksColumns.size() == solver.size &&
         bitMasksSquares.size() == solver.size;
}

bool SudokuSolver_bitmasks::Solve(Solver &solver, ValueLocation location) {
  if (location.first == solver.size - 1 && location.second == solver.size) {
    return true;
  }

  if (location.second == solver.size) {
    location.first++;
    location.second = 0;
  }

  if (XYToSudokuPos(solver.size, location) >= solver.values.size()) {
    return false;
  }

  if (solver.values[XYToSudokuPos(solver.size, location)]) {
    return Solve(solver, {location.first, location.second + 1});
  }

  for (int value{1}; value <= solver.size; value++) {
    if (CanBePlaced(solver, location, value)) {
      solver.values[XYToSudokuPos(solver.size, location)] = {value};
      SetBitMasks(solver, location, value);
      if (Solve(solver, {location.first, location.second + 1})) {
        return true;
      }
      ResetBitMasks(solver, location, value);
    }
    solver.values[XYToSudokuPos(solver.size, location)].reset();
  }

  return false;
}

bool SudokuSolver_bitmasks::ValidateSudoku(const Solver &solver) noexcept {
  ClearBitMasks();
  GenerateBitMasks(solver);
  auto solverCopy{solver};

  for (std::size_t idx{}; idx < solver.values.size(); idx++) {
    const auto v = solver.values[idx];
    if (!v.has_value()) {
      continue;
    }
    // remove the current value
    solverCopy.values[idx].reset();
    ClearBitMasks();
    GenerateBitMasks(solverCopy);
    // try to place it
    if (!CanBePlaced(solverCopy, SudokuPosToXY(solverCopy.size, idx), v)) {
      ClearBitMasks();
      return false;
    }
    // set it again
    solverCopy.values[idx] = v;
    SetBitMasks(solver, SudokuPosToXY(solverCopy.size, idx), v);
  }

  ClearBitMasks();
  return true;
}

bool SudokuSolver_bitmasks::CanBeSolved(const Solver &solver) noexcept {
  const auto r{SudokuSolver_::CanBeSolved(solver)};
  ClearBitMasks();
  return r;
}
