#pragma once
#include "sudokuHelpers.h"
#include <vector>

class SudokuSolver
{
  public:
    SudokuSolver(const std::size_t size, const std::size_t sectionSize);
    virtual ~SudokuSolver() = default;

    // Returns true if sudoku can be solved, solves sudoku in values
    virtual bool Solve(std::vector<SudokuValue>& values) noexcept = 0;
    // Returns true if the sudoku can be solved
    [[nodiscard]] virtual bool CanBeSolved(const std::vector<SudokuValue>& values) noexcept;
    // Returns true if the values can be placed on a location in values
    [[nodiscard]] virtual bool CanBePlaced(const std::vector<SudokuValue>& values, ValueLocation location, SudokuValue value) const noexcept = 0;
    // Valides the correctness of the sudoku values
    [[nodiscard]] virtual bool ValidateSudoku(const std::vector<SudokuValue>& values) noexcept = 0;
  protected:
    const std::size_t size;
    const std::size_t sectionSize;
};

class SudokuSolver_bitmasks final: public SudokuSolver
{
  public:
    SudokuSolver_bitmasks(const std::size_t size, const std::size_t sectionSize);
    ~SudokuSolver_bitmasks() = default;

    [[nodiscard]] bool Solve(std::vector<SudokuValue>& values) noexcept override;
    [[nodiscard]] bool CanBePlaced(const std::vector<SudokuValue>& values, ValueLocation location, SudokuValue value) const noexcept override;
    [[nodiscard]] bool ValidateSudoku(const std::vector<SudokuValue>& values) noexcept override;
    [[nodiscard]] bool CanBeSolved(const std::vector<SudokuValue>& values) noexcept override;
  private:
    [[nodiscard]] bool Solve(std::vector<SudokuValue>& values, ValueLocation location);
    // Generates the initial bitmask value according to given values
    void GenerateBitMasks(const std::vector<SudokuValue>& toGenerateFrom);
    // Clears all bit masks
    void ClearBitMasks();
    // Helper to set all 3 bitmasks for location/value
    void SetBitMasks(ValueLocation location, SudokuValue value) noexcept;
    // Helper to reset all 3 bitmasks for location/value
    void ResetBitMasks(ValueLocation location, SudokuValue value) noexcept;
    // Returns true if bitmasks are generates
    [[nodiscard]] bool GeneratedBitMasks() const noexcept;
    // bitmasks for row, column, box
    std::vector<unsigned int> bitMasksRows{};
    std::vector<unsigned int> bitMasksColumns{};
    std::vector<unsigned int> bitMasksSquares{};
};