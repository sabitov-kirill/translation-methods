#pragma once

#include <string>
#include <variant>
#include <vector>

#include "location.hh"

namespace trg {

struct Rule {
  struct RegexPattern {
    std::string body;
  };

  enum class SpecialPattern { END_OF_FILE };

  std::variant<RegexPattern, SpecialPattern> pattern;
  std::string action;
};

class InputParseContext {
public: /* Constructors and Destructors */
  InputParseContext() : lexerLocation((YY_NULLPTR), 1, 1) {}

public: /* Getters and Setters */
  location &getLexerLocation() { return lexerLocation; }
  const location &getLexerLocation() const { return lexerLocation; }
  const std::string &getHeader() const { return header; }
  const std::vector<Rule> &getRules() const { return rules; }

  void setHeader(const std::string &headerCode) { header = headerCode; }
  void addRule(Rule &&rule) { rules.push_back(std::move(rule)); }

private: /* Private Data */
  location lexerLocation;

  std::string header{};
  std::vector<Rule> rules{};
};

} // namespace trg