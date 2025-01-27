%option c++ noyywrap yyclass="trg::lg::InputLexicalAnalyzer" yylineno
%{
#include "InputParseContext.h"
#include "InputLexicalAnalyzer.h"
#include "InputParser.tab.h"
#include "location.hh"

#undef BEGIN
#define BEGIN(state) \
  ((InputLexicalAnalyzer*)this)->prevStart = (yy_start); \
  (yy_start) = 1 + 2 * (state)
#define YY_USER_ACTION  \
    { \
        location &loc = context.getLexerLocation(); \
        loc.begin.column = loc.end.column; \
        loc.begin.line = yylineno; \
        for(int i = 0; yytext[i] != '\0'; i++) { \
            if(yytext[i] == '\n') { \
                loc.end.line++; \
                loc.end.column = 1; \
            } else { \
                loc.end.column++; \
            } \
        } \
    }
using namespace trg::lg;
%}

%x HEADER_BLOCK_STATE
%x RULES_BLOCK_STATE
%x CODE_BLOCK 
%x RULES_BLOCK
%x CLASSNAME_STATE

%%
%{ location &loc = context.getLexerLocation(); 
%}

<INITIAL>"%header"      { BEGIN(HEADER_BLOCK_STATE); return InputParser::make_HEADER_START(loc); }
<INITIAL>"%rules"       { BEGIN(RULES_BLOCK_STATE); return InputParser::make_RULES_START(loc);  }
<INITIAL>"%classname"   { BEGIN(CLASSNAME_STATE); return InputParser::make_CLASSNAME_DIRECTIVE(loc); }
<<EOF>>                 { return InputParser::make_END(loc);}


<HEADER_BLOCK_STATE>"{" { BEGIN(INITIAL);
                          BEGIN(CODE_BLOCK);
                          currentBlock.clear();
                          prevBraceCount = braceCount++;
                        }

<RULES_BLOCK_STATE>"{"  { BEGIN(RULES_BLOCK); }

<RULES_BLOCK>"\""[^\"]*"\"" { std::string text(yytext + 1, yyleng - 2);
                              return InputParser::make_PATTERN_CHAR_SEQUENCE(text, loc); 
                            }
<RULES_BLOCK>"/"[^/]*"/"    { std::string text(yytext + 1, yyleng - 2); 
                              return InputParser::make_PATTERN_REGEX(text, loc);
                            }
<RULES_BLOCK>"EOF"          { return InputParser::make_PATTERN_EOF(loc); }
<RULES_BLOCK>"{"            { BEGIN(RULES_BLOCK);
                              BEGIN(CODE_BLOCK);
                              currentBlock.clear();
                              prevBraceCount = braceCount++;
                            }
<RULES_BLOCK>"}"            { BEGIN(INITIAL); }

<CODE_BLOCK>"{"     { currentBlock += yytext;
                      braceCount++; 
                    }
<CODE_BLOCK>"}"     { braceCount--;
                      if (braceCount == prevBraceCount) {
                        yy_start = prevStart;
                        return InputParser::make_CODE(currentBlock, loc);
                      }
                      currentBlock += yytext;
                    }
<CODE_BLOCK>[^{}]+  { currentBlock += yytext; }

<CLASSNAME_STATE>[A-Za-z_][A-Za-z0-9_]* { BEGIN(INITIAL); return InputParser::make_CLASS_NAME(yytext, loc); }

[ \t\n]+ ;
.        { throw InputParser::syntax_error(loc, std::string("invalid character: ") + yytext); }

%%