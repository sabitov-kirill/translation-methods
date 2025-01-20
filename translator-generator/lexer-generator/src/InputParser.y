%language "c++"
%locations

%define api.token.constructor
%define api.value.type variant
%define api.namespace {trg}
%define api.parser.class {InputParser}
%parse-param {class InputParseContext &context}
%parse-param {class InputLexicalAnalyzer &lexer}

%code requires {
#include <iostream>

#include "InputParseContext.h"
using Rule = trg::Rule;
}

%code {
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "InputLexicalAnalyzer.h"

using namespace std;

#undef yylex
#define yylex lexer.get
}

%token HEADER_START RULES_START PATTERN_EOF
%token <std::string> CODE PATTERN_CHAR_SEQUENCE PATTERN_REGEX ACTION
%token               END 0

%type <Rule::RegexPattern> regex_pattern
%type <Rule::SpecialPattern> special_pattern 
%type <std::variant<Rule::RegexPattern, Rule::SpecialPattern>> pattern
%type <Rule> rule

%%

input: header_section rules_section
     ;

header_section: HEADER_START CODE     { context.setHeader($2); }
              | HEADER_START          { context.setHeader(""); }
              ;

rules_section: RULES_START rules_list
             ;

rules_list: rules_list  rule   { context.addRule(std::move($2)); }
          | /* empty */
          ;

rule: pattern CODE             { $$.pattern = $1; $$.action = $2; }
    ;

pattern: regex_pattern         { $$ = $1; }
       | special_pattern       { $$ = $1; }
       ;

regex_pattern: PATTERN_CHAR_SEQUENCE { $$.body = $1; }
             | PATTERN_REGEX         { $$.body = $1; }
             ;

special_pattern: PATTERN_EOF  { $$ = Rule::SpecialPattern::END_OF_FILE; }
               ;

%%

void trg::InputParser::error(const location &loc, const std::string &message) {
  std::cerr << loc << ": error: " << message << '\n';

  if (!loc.begin.filename) {
    return;
  }
  std::ifstream sourceFile(*loc.begin.filename);
  std::string line;
  std::string msg;
  for (int lineNum = 1; lineNum < loc.end.line; ++lineNum) {
    if (!std::getline(sourceFile, line)) {
      break;
    }

    if (lineNum >= loc.begin.line - 1) {
      std::cerr << "    " << line << '\n';

      if (lineNum == loc.begin.line - 1) {
        const int indent = loc.begin.column - 1;
        const int length = (lineNum == loc.end.line - 1) 
            ? loc.end.column - loc.begin.column - 1
            : line.length() - indent - 1;
        std::cerr 
            << "    "  << std::string(indent, ' ') 
            << '^' << std::string(length, '~') << '\n';
      }
    }
  }

  exit(1);
}
