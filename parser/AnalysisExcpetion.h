#include <exception>
#include <string>

#include "Token.h"

#pragma once

class AnalysisException : public std::exception {
public:
  AnalysisException(std::string unexpected, std::size_t position)
      : _msg(createMessage(unexpected, position)) {}

  AnalysisException(std::string expected, std::string got, std::size_t position)
      : _msg(std::string("SyntaxException at position ") +
             std::to_string(position) + ". Expected: '" + expected +
             "', got: '" + got + "'") {}

  AnalysisException(Token token, std::size_t position)
      : _msg(createMessage("Token" + std::to_string((int)token), position)) {}

  const char *what() const throw() { return _msg.c_str(); }

private:
  std::string createMessage(std::string unexpected, std::size_t position) {
    return std::string("SyntaxException at position ") +
           std::to_string(position) + ". Unexpected: '" + unexpected + "'";
  }

  std::string _msg;
};