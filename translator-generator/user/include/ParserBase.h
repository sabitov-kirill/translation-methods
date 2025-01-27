#pragma once
#include <stdexcept>
#include <string>

#include "LexerBase.h"

namespace trg {

class ParserError : public std::runtime_error {
public:
  ParserError(const std::string &message, const Location<> &location)
      : std::runtime_error(message), location_(location) {}

  const Location<> &getLocation() const { return location_; }

private:
  Location<> location_;
};

template <typename TokenType> class ParserBase {
public:
  auto parse(this auto &self) { return self.parse(); }

protected:
  LexerMixin<TokenType> &lexer;
  TokenType currentToken;

  ParserBase(LexerMixin<TokenType> &lexer) : lexer(lexer) { nextToken(); }

  void nextToken() { currentToken = lexer.nextToken(); }

  void match(typename TokenType::Type expected) {
    if (currentToken.type != expected) {
      throw ParserError(
          "Expected " + std::to_string(static_cast<int>(expected)) + ", got " +
              std::to_string(static_cast<int>(currentToken.type)),
          lexer.getCurrentLocation());
    }
    nextToken();
  }
};

} // namespace trg