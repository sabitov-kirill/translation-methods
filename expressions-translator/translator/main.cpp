#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "Context.h"
#include "LexicalAnalyzer.h"
#include "Parser.tab.h"

enum class InputMode { Console, File };

int main(int argc, char *argv[]) {
  const char *program_name = argc > 0 ? argv[0] : "translator";
  if (argc < 1 || argc > 2) {
    std::cerr << "Wrong arguments count. Usage: " << program_name
              << "[input file]" << std::endl;
    return EXIT_FAILURE;
  }

  auto mode = argc == 2 ? InputMode::File : InputMode::Console;
  auto in = mode == InputMode::Console
                ? std::unique_ptr<std::ifstream>()
                : std::make_unique<std::ifstream>(argv[1]);

  auto context = mode == InputMode::Console
                     ? book::Context{}
                     : book::Context{std::filesystem::absolute(argv[1])};
  book::LexicalAnalyzer lexer(in.get(), context);
  book::Parser parser(context, lexer);

  int result = parser();
  if (auto result = context.getResult(); result) {
    std::cout << *result;
  }

  return result;
}