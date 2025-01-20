#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
#undef YY_DECL
#define YY_DECL trg::InputParser::symbol_type trg::InputLexicalAnalyzer::get()

#include "InputParser.tab.h"

namespace trg {

class InputLexicalAnalyzer : public yyFlexLexer {
public: /* Constructor and destructor */
  InputLexicalAnalyzer(std::istream *in, class InputParseContext &context)
      : yyFlexLexer(in), context(context) {}

  virtual ~InputLexicalAnalyzer() = default;

public: /* Public API */
  InputParser::symbol_type get();

private: /* Data */
  InputParseContext &context;

  std::string currentBlock;
  int braceCount;
  bool wasRules{};
};

} // namespace trg