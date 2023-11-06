
#pragma once

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

class Sudoku final {
public:
  using ValueLocation = std::pair<std::size_t, std::size_t>;
  Sudoku(std::size_t size);
  ~Sudoku();
  void GenerateSudoku();
  [[nodiscard]] const std::vector<int> &getValues() const;
  bool PlaceValue(ValueLocation location, int value);

private:
  const std::size_t size;
  std::vector<int> values;
};
