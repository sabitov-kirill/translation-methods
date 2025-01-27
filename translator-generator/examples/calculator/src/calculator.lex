%header {
#include <string>
#include <iostream>

#include "parser.h"

class Lexer : public trg::LexerMixin<Token> {
private:
    Token makeToken(TokenType type, const std::string& value = "") {
        Token token = value.empty() ? Token(type) : Token(type, std::stod(value));
        std::cout << getTokenRepresentation(token) << '\n';
        return token;
    }

public:
    Lexer(std::istream *in) : trg::LexerMixin<Token>(in) {}
    Token nextToken() override;
};

}

%rules {
    /^[ \t\n\r]+/ {}

    "^\+" { return makeToken(TokenType::PLUS); }
    "^-" { return makeToken(TokenType::MINUS); }
    "^\*" { return makeToken(TokenType::MULT); }
    "^\/" { return makeToken(TokenType::DIV); }
    "^%" { return makeToken(TokenType::MOD); }
    /^[0-9]+(\.[0-9]+)?/ { return makeToken(TokenType::NUMBER, match.matched); }

    "^\(" { return makeToken(TokenType::LPAREN); }
    "^\)" { return makeToken(TokenType::RPAREN); }

    EOF { return makeToken(TokenType::END); }
}