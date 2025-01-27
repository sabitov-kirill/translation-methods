%language "c++"
%locations

%define api.token.constructor
%define api.value.type variant
%define api.namespace {trg::pg}
%define api.parser.class {InputParser}
%parse-param {class InputParseContext& parseContext}
%parse-param {class InputLexicalAnalyzer& lexer}

%code requires {
    #include "InputParseContext.h"
}

%code {
    #include "InputLexicalAnalyzer.h"
    #define yylex lexer.get
}

%token HEADER_START TOKENS_START GRAMMAR_START LPAREN RPAREN COLON SEMICOLON ARROW OR COMMA
%token <std::string> CODE ID ARGUMENT
%token END 0

%type <Token> token_decl
%type <GrammarRule> grammar_rule
%type <Production> production
%type <std::vector<Production>> production_list
%type <std::vector<Production::Symbol>> symbols

%%

input: header_section tokens_section grammar_section
     ;

header_section: HEADER_START CODE { parseContext.setHeader($2); };

tokens_section: TOKENS_START token_list;

token_list: token_list token_decl { parseContext.addToken(std::move($2)); }
          | /* empty */
          ;

token_decl: ID COLON ID SEMICOLON { $$.name = $1; $$.type = $3; }
          | ID SEMICOLON          { $$.name = $1; }
          ;

grammar_section: GRAMMAR_START grammar_rules;

grammar_rules: grammar_rules grammar_rule { parseContext.addRule(std::move($2)); }
             | /* empty */
             ;

grammar_rule: ID ARGUMENT COLON ID ARROW production_list SEMICOLON
            {
                $$.nonTerminal = $1;
                $$.argument = std::move($2);
                $$.type = $4;
                $$.productions = std::move($6);
            }
            | ID ARGUMENT ARROW production_list SEMICOLON
            {
                $$.nonTerminal = $1;
                $$.productions = std::move($4);
            }
            ;

production_list: production_list OR production
               {
                   $$ = std::move($1);
                   $$.push_back(std::move($3));
               }
               | production
               {
                   $$.push_back(std::move($1));
               }
               ;

production: symbols CODE
          {
              $$.symbols = std::move($1);
              $$.action = $2;
          }
          | symbols
          {
              $$.symbols = std::move($1);
          }
          ;

symbols: symbols ID ARGUMENT
       {
           $$ = std::move($1);
           $$.push_back({$2, $3});
       }
       | symbols ID
       {
           $$ = std::move($1);
           $$.push_back({$2});
       }
       | /* empty */
       {
           $$ = {};
       }
       ;

%%

void trg::pg::InputParser::error(const location& loc, const std::string& msg) {
    std::cerr << loc << ": " << msg << std::endl;
}