%language "c++"
%locations

%define api.token.constructor
%define api.value.type variant
%define api.namespace {book}
%define api.parser.class {Parser}
%parse-param {class Context &context}
%parse-param {class LexicalAnalyzer &lexer}

%code {
#include <iostream>
#include <fstream>

#include "Context.h"
#include "LexicalAnalyzer.h"

using namespace std;

#undef yylex
#define yylex lexer.get
}

%token <std::string> ID
%token <std::string> NUM
%token               LET IF ELSE PRINT PRINTLN READ
%token               END 0 "end of file"

%type <std::string> start program stmt_list stmt expr primary

%%
start: program                     { context.setResult($1); }
     ;

program:
    stmt_list                      { $$ = "fn main() {\n" + $1 + "}\n"; }
    ;

stmt_list:
    stmt                           { $$ = $1; }
    | stmt_list stmt               { $$ = $1 + $2; }
    ;

stmt:
    expr                        { $$ = $1 + ";\n"; }
    | LET ID '=' expr         { $$ = "let mut " + $2 + " = " + $4 + ";\n"; }
    | ID '=' expr             { $$ = $1 + " = " + $3 + ";\n"; }
    | PRINT expr               { $$ = "print!(\"{}\", " + $2 + ");\n"; }
    | PRINTLN expr             { $$ = "println!(\"{}\", " + $2 + ");\n"; }
    | READ ID                  { $$ = "let mut line = String::new();\n"
                                        "std::io::stdin().read_line(&mut line).unwrap();\n"
                                        "let " + $2 + " = line.trim().parse().unwrap();\n"; }
    | IF expr '{' stmt_list '}'   { $$ = "if " + $2 + " {\n" + $4 + "}\n"; }
    | IF expr '{' stmt_list '}' 
      ELSE '{' stmt_list '}'      { $$ = "if " + $2 + " {\n" + $4 + "} else {\n" + $8 + "}\n"; }
    ;


expr: 
  primary                       { $$ = $1; }
  | '+' expr expr               { $$ = "(" + $2 + " + " + $3 + ")"; }
  | '-' expr expr               { $$ = "(" + $2 + " - " + $3 + ")"; }
  | '*' expr expr               { $$ = "(" + $2 + " * " + $3 + ")"; }
  | '/' expr expr               { $$ = "(" + $2 + " / " + $3 + ")"; }
  | '^' expr expr               { $$ = $2 + ".pow(" + $3 + ")"; }
  | '&' expr expr               { $$ = "(" + $2 + " && " + $3 + ")"; }
  | '|' expr expr               { $$ = "(" + $2 + " || " + $3 + ")"; }
  | '=''=' expr expr            { $$ = "(" + $3 + " == " + $4 + ")"; }
  | '!''=' expr expr            { $$ = "(" + $3 + " != " + $4 + ")"; }
  | '>' expr expr               { $$ = "(" + $2 + " > " + $3 + ")"; }
  | '<' expr expr               { $$ = "(" + $2 + " < " + $3 + ")"; }
  | '>''=' expr expr            { $$ = "(" + $3 + " >= " + $4 + ")"; }
  | '<''=' expr expr            { $$ = "(" + $3 + " <= " + $4 + ")"; }
  | '<''<' expr expr            { $$ = "(" + $3 + " << " + $4 + ")"; }
  | '>''>' expr expr            { $$ = "(" + $3 + " >> " + $4 + ")"; }
  | '>''>''>' expr expr         { $$ = "(" + $4 + " >>> " + $5 + ")"; }
  | '!' expr                    { $$ = "!" + $2; }
  | '~' expr                    { $$ = "!" + $2; }
  | 'n' expr                    { $$ = "-" + $2; }
  ;

primary:
    NUM                           { $$ = $1; }
    | ID                          { $$ = $1; }
    ;
%%

void book::Parser::error(const location &loc, const std::string &message) {
  std::cerr << loc << ": error: " << message << '\n';

  if (!loc.begin.filename) {
    return;
  }
  std::ifstream sourceFile(*loc.begin.filename);
  std::string line;
  for (int lineNum = 1; lineNum <= loc.end.line; ++lineNum) {
    if (!std::getline(sourceFile, line)) {
      break;
    }

    if (lineNum >= loc.begin.line) {
      std::cerr << "    " << line << '\n';
      
      if (lineNum == loc.begin.line) {
        const int indent = loc.begin.column - 1;
        std::cerr 
            << "    "  << std::string(indent, ' ') 
            << '^' << std::string(line.length() - indent - 1, '~') << '\n';
      }
    }
  }
}