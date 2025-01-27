%language "c++"
%locations

%define api.token.constructor
%define api.value.type variant
%define api.namespace {trg::lg}
%define api.parser.class {InputParser}
%parse-param {class InputParseContext &parseContext}
%parse-param {class InputLexicalAnalyzer &lexer}

%code requires {
#include <iostream>

#include "InputParseContext.h"
using Rule = trg::lg::Rule;
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

%token CLASSNAME_DIRECTIVE
%token HEADER_START RULES_START PATTERN_EOF
%token <std::string> CODE PATTERN_CHAR_SEQUENCE PATTERN_REGEX ACTION
%token               END 0

%type <Rule::RegexPattern> regex_pattern
%type <Rule::SpecialPattern> special_pattern 
%type <std::variant<Rule::RegexPattern, Rule::SpecialPattern>> pattern
%type <Rule> rule
%token <std::string> CLASS_NAME

%%

input: directives header_section rules_section
     ;

directives: directives directive
          | /* empty */
          ;

directive: classname_directive
         ;

classname_directive: CLASSNAME_DIRECTIVE CLASS_NAME { parseContext.setClassName($2); }
                  ;

header_section: HEADER_START CODE     { parseContext.setHeader($2); }
              | HEADER_START          { parseContext.setHeader(""); }
              ;

rules_section: RULES_START rules_list
             ;

rules_list: rules_list  rule   { parseContext.addRule(std::move($2)); }
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

void trg::lg::InputParser::error(const location &loc, const std::string &message) {
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
    std::cout << "    " << line << '\n' << "    ";
    if (lineNum == loc.begin.line) {
      for (int i = 1; i < loc.begin.column; ++i) std::cout << ' ';
      for (size_t i = loc.begin.column; i <= line.length(); ++i) std::cout << '~';
      std::cout << '\n';
    } else if (lineNum == loc.end.line) {
      for (int i = 1; i < loc.end.column; ++i) std::cout << '~';
      std::cout << '\n';
    } else {
      for (size_t i = 1; i <= line.length(); ++i) std::cout << '~';
      std::cout << '\n';
    }
  }

  exit(1);
}
