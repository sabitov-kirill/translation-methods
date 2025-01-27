#pragma once

#include "location.hh"
#include <string>
#include <vector>

namespace trg::pg {


/**
 * Grammar representation special symbols:
 * - Empty production (epsilon) is represented by an empty symbols vector
 * - EOF is not represented in the grammar rules directly but is used
 *   in FOLLOW set calculations
 */
 
struct Token {
  std::string name;
  std::string type;
};

struct Production {
  struct Symbol {
    std::string name;
    std::string argument;
  };

  std::vector<Symbol> symbols;
  std::string action;
};

struct GrammarRule {
  std::string nonTerminal;
  std::string type;
  std::string argument;
  std::vector<Production> productions;
};

class InputParseContext {
public:
  location &getLocation() { return parserLocation; }
  const location &getLocation() const { return parserLocation; }

  void setHeader(const std::string &headerCode) { header = headerCode; }
  const std::string &getHeader() const { return header; }

  void addToken(Token &&token) { tokens.push_back(std::move(token)); }
  const std::vector<Token> &getTokens() const { return tokens; }

  void addRule(GrammarRule &&rule) { rules.push_back(std::move(rule)); }
  void setRules(std::vector<GrammarRule> &&rules) {
    this->rules = std::move(rules);
  }
  const std::vector<GrammarRule> &getRules() const { return rules; }

private:
  location parserLocation{};
  std::string header{};
  std::vector<Token> tokens{};
  std::vector<GrammarRule> rules{};
};

} // namespace trg::pg