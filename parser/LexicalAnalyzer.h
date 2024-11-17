#include <cctype>
#include <cstddef>
#include <istream>
#include <optional>
#include <string>
#include <vector>

#include "AnalysisExcpetion.h"
#include "Token.h"

#pragma once

using char_t = std::char_traits<char>::int_type;
constexpr auto END_CHAR = std::char_traits<char>::eof();

template <typename T>
concept char_source = requires(T cs) {
  { cs.pos() } -> std::same_as<std::size_t>;
  { cs.next() } -> std::same_as<char_t>;
  { cs.setAnchor() } -> std::same_as<void>;
  { cs.resetToAnchor() } -> std::same_as<void>;
};

class StringSource {
public:
  StringSource(std::string str) : _str(std::move(str)) {}

  char_t next() {
    if (_pos >= _str.length()) {
      return END_CHAR;
    }
    return _str[_pos++];
  }

  void setAnchor() { _anchor = _pos; }
  void resetToAnchor() { _pos = _anchor; }
  std::size_t pos() const noexcept { return _pos; }

private:
  std::string _str;
  std::size_t _pos{};
  std::size_t _anchor{};
};

class StreamSource {
public:
  StreamSource(std::istream *input) : _input(input) {}

  char_t next() {
    if (_pos >= _buffer.size()) {
      auto char_or_eof = _input->get();
      if (char_or_eof == END_CHAR) {
        return char_or_eof;
      }

      char ch = static_cast<char>(char_or_eof);
      _buffer.push_back(ch);
      return ch;
    }

    return _buffer[_pos++];
  }

  void setAnchor() {
    if (_pos > 0) {
      _buffer.erase(_buffer.begin(), _buffer.begin() + _pos);
      _pos = 0;
    }
    _anchor = _pos;
  }

  void resetToAnchor() { _pos = _anchor; }
  std::size_t pos() const noexcept { return _pos; }

private:
  std::istream *_input{};

  std::vector<char> _buffer{};
  std::size_t _pos{};
  std::size_t _anchor{};
};

static_assert(char_source<StringSource>);
static_assert(char_source<StreamSource>);

template <char_source CS> class LexicalAnalyzer {
public: // Public interface
  LexicalAnalyzer(CS cs) noexcept : _cs(std::move(cs)) { take(); }

  Token nextToken() {
    skipSpaces();

    if (testIsEnd()) {
      _currentToken = Token::END;
    } else if (auto tokenResult = testTokens()) {
      _currentToken = *tokenResult;
    } else {
      _currentToken = Token::VARIABLE;
      take();
    }

    testTokenEnd();
    return currentToken();
  }

  Token currentToken() const noexcept { return _currentToken; }

  std::size_t pos() const noexcept { return _cs.pos(); }

private: // Helper method
  std::optional<Token> testTokens() {
    // TODO: Convert to compile-time hashmap
    for (auto &&[tokenString, token] : stringToToken) {
      auto &&[success, _] = test(tokenString);
      if (success) {
        take(tokenString);
        return token;
      }
    }
    return {};
  }

private: // Common methods
  char_t take() {
    auto taken = _currentChar;
    _currentChar = _cs.next();
    return taken;
  }

  void take(const std::string &expected) {
    auto &&[success, result] = test(expected, false);
    if (!success) {
      throw AnalysisException(expected, result, _cs.pos());
    }
  }

  void testTokenEnd() {
    if (_currentToken == Token::LP || _currentToken == Token::RP) {
      return;
    }

    auto saveChar = _currentChar;
    auto success = testIsSpace() || testIsEnd() || test('(') || test(')');
    if (!success) {
      throw AnalysisException(std::string(1, ' '), std::string(1, saveChar),
                              _cs.pos());
    }
  }

  bool testIsEnd() const noexcept { return _currentChar == END_CHAR; }
  bool testIsSpace() const noexcept { return std::isspace(_currentChar); }
  bool test(char expected) const noexcept { return expected == _currentChar; }

  std::pair<bool, std::string> test(const std::string &expected,
                                    bool useAnchor = true) {
    auto anchorChar = _currentChar;
    if (useAnchor) {
      _cs.setAnchor();
    }

    std::string result;
    bool success = true;
    for (std::size_t i = 0; i < expected.size(); ++i) {
      success &= test(expected[i]);
      result += _currentChar;
      take();
    }

    if (useAnchor) {
      _currentChar = anchorChar;
      _cs.resetToAnchor();
    }

    return {success, result};
  }

  void skipSpaces() {
    while (!testIsEnd() && testIsSpace()) {
      take();
    }
  }

private: // Data
  CS _cs;
  char_t _currentChar;
  Token _currentToken;

  static constexpr std::pair<std::string, Token> stringToToken[] = {
      {"or", Token::OR_OPERATOR},
      {"xor", Token::XOR_OPERATOR},
      {"and", Token::AND_OPERATOR},
      {"not", Token::NOT_OPERATOR},
      {"in", Token::IN_OPERATOR},
      {"(", Token::LP},
      {")", Token::RP}};
};