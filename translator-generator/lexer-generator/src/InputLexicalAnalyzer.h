#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
#undef YY_DECL
#define YY_DECL trg::lg::InputParser::symbol_type trg::lg::InputLexicalAnalyzer::get()

#include "InputParser.tab.h"

namespace trg::lg {

class InputLexicalAnalyzer : public yyFlexLexer {
  friend class yyFlexLexer;

public: /* Constructor and destructor */
  InputLexicalAnalyzer(std::istream *in, class InputParseContext &context)
      : yyFlexLexer(in), context(context) {yy_flex_debug = 1;}

  virtual ~InputLexicalAnalyzer() = default;

public: /* Public API */
  InputParser::symbol_type get();

private: /* Data */
  InputParseContext &context;

  std::string currentBlock{};
  int braceCount{};
  int prevBraceCount{};
  int prevStart{1};
};

} // namespace trg