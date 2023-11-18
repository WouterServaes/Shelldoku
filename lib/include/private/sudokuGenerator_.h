#pragma once

struct Generator;

class SudokuGenerator_ {
  public:
    SudokuGenerator_() = default;
    ~SudokuGenerator_() = default;
    SudokuGenerator_(const SudokuGenerator_&) = delete;
    SudokuGenerator_(SudokuGenerator_&&) = delete;
    SudokuGenerator_& operator=(const SudokuGenerator_&) = delete;
    SudokuGenerator_& operator=(SudokuGenerator_&&) = delete;


    [[nodiscard]] bool Generate(Generator& generator);
};