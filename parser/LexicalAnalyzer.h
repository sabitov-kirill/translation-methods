#include <istream>

#pragma once

enum class Token {
  OR_OPERATOR,
  AND_OPERATOR,
  NOT_OPERATOR,
  IN_OPERATOR,
  LP,
  RP,
  VARIABLE,
  END
};

class LexicalAnalyzer {
public:
  LexicalAnalyzer(std::istream *input) noexcept;

  Token nextToken();
  Token currentToken() const noexcept;

private:
  bool isSpace() const noexcept;
  void skipSpaces();
  void nextChar();
  void take(const std::string &expected);

  std::istream *_input{};

  bool _isEOF{false};
  std::size_t _position{};
  char _currentChar{};
  Token _currentToken{};
};