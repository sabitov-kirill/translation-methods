#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include "InputParseContext.h"

namespace trg::pg {

/**
 * Special symbols used in grammar analysis:
 * - Empty string (epsilon) is represented as "" (empty string)
 * - End of input (EOF) is represented as "\0"
 */

using SymbolSet = std::set<std::string>;
using SymbolSetsMap = std::unordered_map<std::string, SymbolSet>;

// Constants for special symbols
constexpr const char *EPSILON = "";
constexpr const char *EOF_SYMBOL = "END";

class GrammarAnalyzer {
public:
  virtual ~GrammarAnalyzer() = default;
  void analyze(const InputParseContext &context);

  const SymbolSetsMap &getFirstSets() const { return firstSets; }
  const SymbolSetsMap &getFollowSets() const { return followSets; }
  bool leftRecursion() const { return isLeftRecursion; }
  bool leftFactoring() const { return isLeftFactoring; }

  bool isTerminal(const std::string &symbol) const { return terminals.contains(symbol); }
  const std::string &getSymbolType(const std::string &symbol);

private:
  void calculateFirstSets(const InputParseContext &context);
  void calculateFollowSets(const InputParseContext &context);
  void calculateIsLeftRecursion(const InputParseContext &context);
  void calculateIsLeftFactoring(const InputParseContext &context);

  std::unordered_map<std::string, std::string> nonTerminals;
  std::unordered_map<std::string, std::string> terminals;
  SymbolSetsMap firstSets;
  SymbolSetsMap followSets;
  bool isLeftRecursion;
  bool isLeftFactoring;
};

} // namespace trg::pg
