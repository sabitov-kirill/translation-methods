#include <LexerGenerator.h>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // Check arguments
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input-file> <output-file>"
              << std::endl;
    return 1;
  }

  // Open input file
  std::string inputPath(argv[1]);
  std::ifstream input(inputPath);
  if (!input.is_open()) {
    std::cerr << "Error: Could not open input file '" << inputPath << "'"
              << std::endl;
    return 1;
  }

  // Open output file
  std::string outputPath(argv[2]);
  std::ofstream output(outputPath);
  if (!output.is_open()) {
    std::cerr << "Error: Could not open output file '" << outputPath << "'"
              << std::endl;
    return 1;
  }

  // Generate lexer
  trg::LexerGenerator generator(&input, &output, inputPath);
  generator.generate();

  return 0;
}