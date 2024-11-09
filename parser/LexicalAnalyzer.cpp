#include <cctype>
#include <istream>

#include "AnalysisExcpetion.h"
#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(std::istream *input) noexcept : _input(input) { nextChar(); }

Token LexicalAnalyzer::nextToken() {
  skipSpaces();
  if (_isEOF) {
    _currentToken = Token::END;
    return _currentToken;
  }

  switch (_currentChar) {
  case 'o':
    take("or");
    _currentToken = Token::OR_OPERATOR;
    break;
  case 'a':
    take("and");
    _currentToken = Token::AND_OPERATOR;
    break;
  case 'n':
    take("not");
    _currentToken = Token::NOT_OPERATOR;
    break;
  case 'i':
    take("in");
    _currentToken = Token::IN_OPERATOR;
    break;
  case '(':
    nextChar();
    _currentToken = Token::LP;
    break;
  case ')':
    nextChar();
    _currentToken = Token::RP;
    break;
  default:
    throw AnalysisException(std::string(1, _currentChar), _position);
  }

  if (!_isEOF && !isSpace()) {
    throw AnalysisException("End-of-token", std::string(1, _currentChar),
                            _position);
  }
  return _currentToken;
}

Token LexicalAnalyzer::currentToken() const noexcept { return _currentToken; }

bool LexicalAnalyzer::isSpace() const noexcept { return std::isspace(_currentChar); }

void LexicalAnalyzer::skipSpaces() {
  while (!_isEOF && isSpace()) {
    nextChar();
  }
}

void LexicalAnalyzer::nextChar() {
  auto newChar = _input->get();
  if (newChar != std::char_traits<char>::eof()) {
    ++_position;
    _currentChar = static_cast<char>(newChar);
  } else {
    _isEOF = true;
  }
}

void LexicalAnalyzer::take(const std::string &expected) {
  std::string result;
  bool failed;
  for (std::size_t i = 0; i < expected.size(); ++i) {
    failed |= expected[i] != _currentChar;
    result += _currentChar;
    nextChar();
  }

  if (failed) {
    throw AnalysisException(expected, result, _position);
  }
}
