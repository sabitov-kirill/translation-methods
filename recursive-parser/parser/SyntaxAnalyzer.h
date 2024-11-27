#include <string>
#include <utility>
#include <vector>

#include "LexicalAnalyzer.h"
#include "Token.h"

#pragma once

template <typename Meta = std::string> struct ASTNode {
  using Children = std::vector<ASTNode>;

  Meta data;
  Children children;
};
using NameASTNode = ASTNode<>;

template <char_source CS> class SyntaxAnalyzer {
public:
  SyntaxAnalyzer(LexicalAnalyzer<CS> lexer) : _lexer(std::move(lexer)) {
    _lexer.nextToken();
  }

  NameASTNode parse() {
    auto result = parseE();
    if (!currentTokenIs<Token::END>()) {
      error();
    }
    return result;
  }

private:
  NameASTNode parseE() {
    if (currentTokenIs<Token::NOT_OPERATOR, Token::LP, Token::VARIABLE>()) {
      auto X = parseX();
      auto Ep = parseEPrime();
      return {"E", {X, Ep}};
    }

    error();
  }

  NameASTNode parseEPrime() {
    if (match<Token::OR_OPERATOR>()) {
      return {"E'", {parseX(), parseEPrime()}};
    } else if (currentTokenIs<Token::RP, Token::END>()) {
      return {"E'"};
    }

    error();
  }

  NameASTNode parseX() {
    if (currentTokenIs<Token::NOT_OPERATOR, Token::LP, Token::VARIABLE>()) {
      auto T = parseT();
      auto Xp = parseXPrime();
      return {"X", {T, Xp}};
    }

    error();
  }

  NameASTNode parseXPrime() {
    if (match<Token::XOR_OPERATOR>()) {
      return {"X'", {parseT(), parseXPrime()}};
    } else if (currentTokenIs<Token::OR_OPERATOR, Token::RP, Token::END>()) {
      return {"X'"};
    }

    error();
  }

  NameASTNode parseT() {
    if (currentTokenIs<Token::NOT_OPERATOR, Token::LP, Token::VARIABLE>()) {
      auto N = parseN();
      auto Tp = parseTPrime();
      return {"T", {N, Tp}};
    }

    error();
  }

  NameASTNode parseTPrime() {
    if (match<Token::AND_OPERATOR>()) {
      return {"E'", {parseN(), parseTPrime()}};
    } else if (currentTokenIs<Token::XOR_OPERATOR, Token::OR_OPERATOR,
                              Token::RP, Token::END>()) {
      return {"E'"};
    }

    error();
  }

  NameASTNode parseN() {
    if (match<Token::NOT_OPERATOR>()) {
      return {"N", {parseN()}};
    } else if (currentTokenIs<Token::LP, Token::VARIABLE>()) {
      return {"N", {parseM()}};
    }

    error();
  }

  NameASTNode parseM() {
    if (currentTokenIs<Token::LP, Token::VARIABLE>()) {
      return {"M", {parseF(), parseMPrime()}};
    }

    error();
  }

  NameASTNode parseMPrime() {
    if (match<Token::IN_OPERATOR>()) {
      return {"M'", {parseS(), parseMPrime()}};
    } else if (match<Token::NOT_OPERATOR>()) {
      if (match<Token::IN_OPERATOR>()) {
        return {"M'", {parseS(), parseMPrime()}};
      }
      error();
    } else if (currentTokenIs<Token::NOT_OPERATOR, Token::AND_OPERATOR,
                              Token::XOR_OPERATOR, Token ::OR_OPERATOR,
                              Token::RP, Token::END>()) {
      return {"M'"};
    }

    error();
  }

  NameASTNode parseF() {
    if (match<Token::LP>()) {
      auto E = parseE();
      if (!match<Token::RP>()) {
        error();
      }
      return {"F", {E}};
    } else if (match<Token::VARIABLE>()) {
      return {"F"};
    }

    error();
  }

  NameASTNode parseS() {
    if (_lexer.currentToken() == Token::VARIABLE) {
      _lexer.nextToken();
      return {"S"};
    }

    error();
  }

  template <Token... Either> bool currentTokenIs() noexcept {
    return ((_lexer.currentToken() == Either) || ...);
  }

  template <Token... Either> bool match() noexcept {
    if (currentTokenIs<Either...>()) {
      _lexer.nextToken();
      return true;
    }
    return false;
  }

  [[noreturn]] void error() const noexcept(false) {
    throw AnalysisException(_lexer.currentToken(), _lexer.pos());
  }

  LexicalAnalyzer<CS> _lexer;
};

using StringSyntaxAnalyzer = SyntaxAnalyzer<StringSource>;
using StreamSyntaxAnalyzer = SyntaxAnalyzer<StreamSource>;
