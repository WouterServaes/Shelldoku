#include <vector>
#include <memory>

struct Generator {
  std::vector<unsigned int> values{};
  const std::size_t size;
  const std::size_t sectionSize;
};

class SudokuGenerator_;

class SudokuGenerator {
  public:
    SudokuGenerator();
    ~SudokuGenerator() = default;
    SudokuGenerator(const SudokuGenerator&) = delete;
    SudokuGenerator(SudokuGenerator&&) = delete;
    SudokuGenerator& operator=(const SudokuGenerator&) = delete;
    SudokuGenerator& operator=(SudokuGenerator&&) = delete;


    [[nodiscard]] bool Generate(Generator& generator);

  private:
    std::unique_ptr<SudokuGenerator_> pSudokuGenerator; 
};
