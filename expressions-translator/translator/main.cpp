#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "Context.h"
#include "LexicalAnalyzer.h"
#include "Parser.tab.h"

enum class DataMode { Console, File };

int main(int argc, char *argv[]) {
  const char *program_name = argc > 0 ? argv[0] : "translator";
  if (argc < 1 || argc > 4) {
    std::cerr << "Wrong arguments count. Usage: " << program_name
              << " [input file] [output file]" << std::endl;
    return EXIT_FAILURE;
  }

  auto input_mode = argc >= 2 ? DataMode::File : DataMode::Console;
  auto output_mode = argc == 3 ? DataMode::File : DataMode::Console;

  auto in = input_mode == DataMode::Console
                ? std::unique_ptr<std::ifstream>()
                : std::make_unique<std::ifstream>(argv[1]);

  auto out = output_mode == DataMode::Console
                 ? std::unique_ptr<std::ofstream>()
                 : std::make_unique<std::ofstream>(argv[2]);

  auto context = input_mode == DataMode::Console
                     ? book::Context{}
                     : book::Context{std::filesystem::absolute(argv[1])};
  book::LexicalAnalyzer lexer(in.get(), context);
  book::Parser parser(context, lexer);

  int result = parser();
  if (auto parsed_result = context.getResult(); parsed_result) {
    if (output_mode == DataMode::Console) {
      std::cout << *parsed_result;
    } else {
      *out << *parsed_result;
    }
  }

  return result;
}