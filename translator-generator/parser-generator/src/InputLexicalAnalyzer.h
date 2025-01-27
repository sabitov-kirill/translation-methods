#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL                                                                \
  trg::pg::InputParser::symbol_type trg::pg::InputLexicalAnalyzer::get()

#include "InputParser.tab.h"

namespace trg::pg {

class InputLexicalAnalyzer : public yyFlexLexer {
  friend class yyFlexLexer;

public:
  InputLexicalAnalyzer(std::istream *in, InputParseContext &context)
      : yyFlexLexer(in), context(context) {}

  virtual ~InputLexicalAnalyzer() = default;
  InputParser::symbol_type get();

private:
  InputParseContext &context;

  std::string currentBlock{};
  int braceCount{}; // For tracking CODE state
  int prevBraceCount{};
  int parenthesisCount{}; // For tracking ARGUMENT state
  int prevParenthesisCount{};

  // Start is flex internal's jump position for state handling. So we are doing
  // a bit dirty hack here.
  int prevStart{1};
};

} // namespace trg::pg