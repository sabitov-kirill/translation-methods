#include <exception>
#include <string>

#pragma once

class AnalysisException : public std::exception {
public:
  AnalysisException(std::string unexpected, std::size_t position)
      : _msg(std::string("SyntaxException at position ") +
             std::to_string(position) + ". Unexpected: '" + unexpected + "'") {}

  AnalysisException(std::string expected, std::string got, std::size_t position)
      : _msg(std::string("SyntaxException at position ") +
             std::to_string(position) + ". Expected: '" + expected +
             "', got: '" + got + "'") {}

  const char *what() const throw() { return _msg.c_str(); }

private:
  std::string _msg;
};