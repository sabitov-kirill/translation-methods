%header {
#include <string>

#include "parser.h"

class Lexer : public trg::LexerMixin<Token> {
public:
  Lexer(std::istream *in) : trg::LexerMixin<Token>(in) {}
  Token nextToken() override;
};

}

%rules {
    /^[ \t\n\r]+/ {}
    
    "^or"  { return Token(TokenType::OR); }
    "^xor" { return Token(TokenType::XOR); }
    "^and" { return Token(TokenType::AND); }
    "^not" { return Token(TokenType::NOT); }
    "^in"  { return Token(TokenType::IN); }
    /^[a-zA-Z_]/ { return Token(TokenType::ID, match.matched); }

    "\(" { return Token(TokenType::LPAREN); }
    "\)" { return Token(TokenType::RPAREN); }

    EOF { return Token(TokenType::END); }
}