%option c++ noyywrap yyclass="trg::pg::InputLexicalAnalyzer" yylineno debug 

%{
#include "InputParseContext.h"
#include "InputLexicalAnalyzer.h"
#include "InputParser.tab.h"
#include "location.hh"

#undef BEGIN
#define BEGIN(state) \
  ((InputLexicalAnalyzer*)this)->prevStart = (yy_start); \
  (yy_start) = 1 + 2 * (state)
#define YY_USER_ACTION { \
    location& loc = context.getLocation(); \
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

using namespace trg::pg;
%}

%x HEADER_BLOCK_STATE
%x TOKENS_BLOCK_STATE
%x GRAMMAR_BLOCK_STATE
%x CODE_BLOCK
%x TOKENS_BLOCK
%x GRAMMAR_BLOCK
%x ARGUMENT_BLOCK

%%
%{ location &loc = context.getLocation(); 
std::cerr << "Current state: " << yy_start << '\n';
%}

<INITIAL>"%header"        { BEGIN(HEADER_BLOCK_STATE); return InputParser::make_HEADER_START(loc); }
<INITIAL>"%token"         { BEGIN(TOKENS_BLOCK_STATE); return InputParser::make_TOKENS_START(loc); }
<INITIAL>"%grammar"       { BEGIN(GRAMMAR_BLOCK_STATE); return InputParser::make_GRAMMAR_START(loc); }
<<EOF>>                   { return InputParser::make_END(loc);}

<HEADER_BLOCK_STATE>"{"   { BEGIN(INITIAL);
                            BEGIN(CODE_BLOCK);
                            currentBlock.clear();
                            prevBraceCount = braceCount++;
                          }
<TOKENS_BLOCK_STATE>"{"   { BEGIN(TOKENS_BLOCK); }
<GRAMMAR_BLOCK_STATE>"{"  { BEGIN(GRAMMAR_BLOCK); }

<CODE_BLOCK>"{"           { std::cerr << "CODE '{': " << braceCount << "\n"; currentBlock += yytext; braceCount++; }
<CODE_BLOCK>"}"           { std::cerr << "CODE '}': " << braceCount << "\n";
                            braceCount--;
                            if (braceCount == prevBraceCount) {
                              yy_start = prevStart;
                              return InputParser::make_CODE(currentBlock, loc);
                            }
                            currentBlock += yytext;
                          }
<CODE_BLOCK>[^{}]+        { currentBlock += yytext; }

<TOKENS_BLOCK,GRAMMAR_BLOCK>"}" { BEGIN(INITIAL); }
<TOKENS_BLOCK,GRAMMAR_BLOCK>":" { return InputParser::make_COLON(loc); }
<TOKENS_BLOCK,GRAMMAR_BLOCK>";" { return InputParser::make_SEMICOLON(loc); }
<TOKENS_BLOCK,GRAMMAR_BLOCK>(([a-zA-Z][a-zA-Z0-9<>_]*)|([a-zA-Z][a-zA-Z0-9_]*::[a-zA-Z0-9<>_]*)) { 
  return InputParser::make_ID(yytext, loc); 
}

<GRAMMAR_BLOCK>"("                    { BEGIN(ARGUMENT_BLOCK);
                                        currentBlock.clear();
                                        prevParenthesisCount = parenthesisCount++;
                                      }
<GRAMMAR_BLOCK>","                    { return InputParser::make_COMMA(loc); }
<GRAMMAR_BLOCK>"->"                   { return InputParser::make_ARROW(loc); }
<GRAMMAR_BLOCK>"|"                    { return InputParser::make_OR(loc); }
<GRAMMAR_BLOCK>"{"                    { BEGIN(GRAMMAR_BLOCK);
                                        BEGIN(CODE_BLOCK);
                                        currentBlock.clear();
                                        prevBraceCount = braceCount++;
                                      }

<ARGUMENT_BLOCK>"("     { currentBlock += yytext; parenthesisCount++; }
<ARGUMENT_BLOCK>")"     { parenthesisCount--;
                          if (parenthesisCount == prevBraceCount) {
                            // We know that ARGUMENT_BLOCK can appear only in grammar block,
                            // so we just jump back to grammar block, unlike in CODE
                            // which can be called from multiple places
                            BEGIN(GRAMMAR_BLOCK);
                            return InputParser::make_ARGUMENT(currentBlock, loc);
                          }
                          currentBlock += yytext;
                        }
<ARGUMENT_BLOCK>[^()]+  { currentBlock += yytext; }


[ \t\r\n]+;
.         { throw InputParser::syntax_error(loc, "Invalid character"); }

%%