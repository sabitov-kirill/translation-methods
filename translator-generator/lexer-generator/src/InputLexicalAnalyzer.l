%option c++ noyywrap yyclass="trg::InputLexicalAnalyzer"
%{
#include "InputParseContext.h"
#include "InputLexicalAnalyzer.h"
#include "InputParser.tab.h"
#include "location.hh"

#define YY_USER_ACTION loc.columns(yyleng);

using namespace trg;

%}

%x CODE_BLOCK
%x PATTERN_BLOCK
%x RULES_STATE

%%
%{
  location &loc = context.getLexerLocation();
  loc.step();
%}

"%header"            { BEGIN(PATTERN_BLOCK); return InputParser::make_HEADER_START(loc); }
"%rules"             { BEGIN(RULES_STATE); return InputParser::make_RULES_START(loc);  }
"EOF"                { return InputParser::make_PATTERN_EOF(loc); }
<<EOF>>              { return InputParser::make_END(loc);}

<PATTERN_BLOCK>"{"  { BEGIN(CODE_BLOCK);
                      currentBlock.clear();
                      braceCount = 1;
                    }

<CODE_BLOCK>"{"     { currentBlock += yytext;
                      braceCount++; 
                    }

<CODE_BLOCK>"}"     { braceCount--;
                      if (braceCount == 0) {
                        BEGIN(wasRules ? RULES_STATE : INITIAL);
                        return InputParser::make_CODE(currentBlock, loc);
                      }
                      currentBlock += yytext;
                    }

<CODE_BLOCK>[^{}]+   { currentBlock += yytext; }

<RULES_STATE>"\""[^\"]*"\"" { std::string text(yytext + 1, yyleng - 2);
                              return InputParser::make_PATTERN_CHAR_SEQUENCE(text, loc); 
                            }

<RULES_STATE>"/"[^/]*"/"    { std::string text(yytext + 1, yyleng - 2); 
                              return InputParser::make_PATTERN_REGEX(text, loc);
                            }

<RULES_STATE>"EOF"          { return InputParser::make_PATTERN_EOF(loc); }

<RULES_STATE>"{"            { BEGIN(CODE_BLOCK);
                              currentBlock.clear();
                              braceCount = wasRules = 1;
                            }

[ \t]+              { loc.step(); }
[\n]+               { loc.lines(yyleng); loc.step(); }

.                   { throw InputParser::syntax_error(loc, std::string("invalid character: ") + yytext); }

%%