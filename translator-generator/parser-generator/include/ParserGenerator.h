#pragma once

#include <string>
#include <unordered_map>

#include "GrammarAnalyzer.h"
#include "InputParseContext.h"

namespace trg::pg {

class ParserGenerator {
public:
  ParserGenerator(std::istream *in, std::ostream *out, std::string inputPath)
      : in(in), out(out) {}

  void generate();

private:
  void generateTokens();
  void generateClass();
  void generateNonTerminal(const GrammarRule &rule);
  std::string replaceTranslationSymbols(const std::string &action);

  std::istream *in;
  std::ostream *out;
  InputParseContext context;
  GrammarAnalyzer analyzer;

  std::unordered_map<std::string, std::string> symbolTypes;
};

} // namespace trg::pg