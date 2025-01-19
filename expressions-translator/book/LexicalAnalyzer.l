%option c++ noyywrap yylineno yyclass="book::LexicalAnalyzer"
%{
#include <iostream>
#include <exception>

#include "Context.h"
#include "LexicalAnalyzer.h"
#include "Parser.tab.h"

using namespace book;

#define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
  location &loc = context.getLocation();
  loc.step();
%}

[ \t]+  loc.step();
[\n]+   loc.lines(yyleng); loc.step();
<<EOF>> return Parser::make_END(loc);

"let"     return Parser::make_LET(loc);
"if"      return Parser::make_IF(loc);
"else"    return Parser::make_ELSE(loc);
"print"   return Parser::make_PRINT(loc);
"println" return Parser::make_PRINTLN(loc);
"read"    return Parser::make_READ(loc);

"+"       return Parser::symbol_type('+', loc);
"-"       return Parser::symbol_type('-', loc);
"*"       return Parser::symbol_type('*', loc);
"/"       return Parser::symbol_type('/', loc);
"&"       return Parser::symbol_type('&', loc);
"|"       return Parser::symbol_type('|', loc);
"="       return Parser::symbol_type('=', loc);
"!"       return Parser::symbol_type('!', loc);
">"       return Parser::symbol_type('>', loc);
"<"       return Parser::symbol_type('<', loc);
"^"       return Parser::symbol_type('^', loc);
"n"       return Parser::symbol_type('n', loc);
"~"       return Parser::symbol_type('~', loc);
";"       return Parser::symbol_type(';', loc);
"("       return Parser::symbol_type('(', loc);
")"       return Parser::symbol_type(')', loc);
"{"       return Parser::symbol_type('{', loc);
"}"       return Parser::symbol_type('}', loc);

[a-zA-Z_][a-zA-Z0-9_]* return Parser::make_ID(yytext, loc);
[0-9]+                  return Parser::make_NUM(yytext, loc);

.         throw Parser::syntax_error(loc, std::string("invalid character: ") + yytext);

%%