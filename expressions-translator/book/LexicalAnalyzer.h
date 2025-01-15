#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
#undef YY_DECL
#define YY_DECL book::Parser::symbol_type book::LexicalAnalyzer::get()

#include "Parser.tab.h"

#pragma once

namespace book {

class LexicalAnalyzer : public yyFlexLexer {
public: /* Constructor and destructor */
  LexicalAnalyzer(std::istream *in, class Context &context)
      : yyFlexLexer(in), context(context){};

  virtual ~LexicalAnalyzer() = default;

public: /* Public API */
  Parser::symbol_type get();

private: /* Data */
  Context &context;
};

} // namespace book