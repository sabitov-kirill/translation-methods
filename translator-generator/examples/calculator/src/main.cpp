#include <cassert>
#include <iostream>
#include <sstream>

#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " expression\n";
    return 1;
  }
  std::stringstream ss(argv[1]);
  Lexer l(&ss);
  Parser p(l);
  std::cout << p.parse() << '\n';
  return 0;
}