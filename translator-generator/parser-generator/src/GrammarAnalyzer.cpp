#include "GrammarAnalyzer.h"
#include "InputParseContext.h"

namespace trg::pg {

void GrammarAnalyzer::analyze(const InputParseContext &context) {
  for (const auto &token : context.getTokens()) {
    terminals[token.name] = token.type;
  }
  for (const auto &rule : context.getRules()) {
    nonTerminals[rule.nonTerminal] = rule.type;
  }

  calculateFirstSets(context);
  calculateFollowSets(context);
  calculateIsLeftRecursion(context);
  calculateIsLeftFactoring(context);
}

void GrammarAnalyzer::calculateFirstSets(const InputParseContext &context) {
  bool changed;
  for (const auto &[nonTerm, _] : nonTerminals) {
    firstSets[nonTerm] = SymbolSet();
  }

  do {
    changed = false;
    for (const auto &rule : context.getRules()) {
      const auto &nonTerm = rule.nonTerminal;

      for (const auto &prod : rule.productions) {
        if (prod.symbols.empty()) {
          auto [_, inserted] = firstSets[nonTerm].insert(EPSILON);
          changed |= inserted;
          continue;
        }

        bool allCanBeEmpty = true;
        for (const auto &symbol : prod.symbols) {
          if (terminals.count(symbol.name) ||
              !nonTerminals.count(symbol.name)) {
            auto [_, inserted] = firstSets[nonTerm].insert(symbol.name);
            changed |= inserted;
            allCanBeEmpty = false;
            break;
          } else {
            const auto &symbolFirst = firstSets[symbol.name];
            for (const auto &f : symbolFirst) {
              if (f != EPSILON) {
                auto [_, inserted] = firstSets[nonTerm].insert(f);
                changed |= inserted;
              }
            }

            if (!symbolFirst.count(EPSILON)) {
              allCanBeEmpty = false;
              break;
            }
          }
        }

        if (allCanBeEmpty) {
          auto [_, inserted] = firstSets[nonTerm].insert(EPSILON);
          changed |= inserted;
        }
      }
    }
  } while (changed);
}

void GrammarAnalyzer::calculateFollowSets(const InputParseContext &context) {
  bool changed;
  for (const auto &rule : context.getRules()) {
    followSets[rule.nonTerminal] = SymbolSet();
  }

  if (!context.getRules().empty()) {
    followSets[context.getRules()[0].nonTerminal].insert(EOF_SYMBOL);
  }

  do {
    changed = false;

    for (const auto &rule : context.getRules()) {
      const auto &nonTerm = rule.nonTerminal;
      for (const auto &prod : rule.productions) {
        for (size_t i = 0; i < prod.symbols.size(); i++) {
          const auto &symbol = prod.symbols[i];

          if (!nonTerminals.count(symbol.name))
            continue;

          bool restCanBeEmpty = true;
          if (i < prod.symbols.size() - 1) {
            for (size_t j = i + 1; j < prod.symbols.size(); j++) {
              const auto &next = prod.symbols[j];
              if (terminals.count(next.name) ||
                  !nonTerminals.count(next.name)) {
                followSets[symbol.name].insert(next.name);
                restCanBeEmpty = false;
                break;
              } else {
                const auto &nextFirst = firstSets[next.name];
                for (const auto &f : nextFirst) {
                  if (f != EPSILON) {
                    followSets[symbol.name].insert(f);
                  }
                }
                if (!nextFirst.count(EPSILON)) {
                  restCanBeEmpty = false;
                  break;
                }
              }
            }
          }

          if (restCanBeEmpty) {
            size_t sizeBefore = followSets[symbol.name].size();
            followSets[symbol.name].insert(followSets[nonTerm].begin(),
                                           followSets[nonTerm].end());
            changed |= (followSets[symbol.name].size() != sizeBefore);
          }
        }
      }
    }
  } while (changed);
}

void GrammarAnalyzer::calculateIsLeftRecursion(
    const InputParseContext &context) {
  isLeftRecursion = false;

  for (const auto &rule : context.getRules()) {
    const auto &nonTerm = rule.nonTerminal;
    for (const auto &prod : rule.productions) {
      if (!prod.symbols.empty() && prod.symbols[0].name == nonTerm) {
        isLeftRecursion = true;
        return;
      }
    }
  }
}

void GrammarAnalyzer::calculateIsLeftFactoring(const InputParseContext &context) {
    isLeftFactoring = false;
    
    for (const auto &rule : context.getRules()) {
        const auto &productions = rule.productions;
        for (size_t i = 0; i < productions.size(); ++i) {
            for (size_t j = i + 1; j < productions.size(); ++j) {
                const auto &prod1 = productions[i];
                const auto &prod2 = productions[j];
                size_t commonPrefixLen = 0;
                while (commonPrefixLen < prod1.symbols.size() && 
                       commonPrefixLen < prod2.symbols.size() && 
                       prod1.symbols[commonPrefixLen].name == prod2.symbols[commonPrefixLen].name) {
                    commonPrefixLen++;
                }
                
                if (commonPrefixLen > 0) {
                    isLeftFactoring = true;
                    return;
                }
            }
        }
    }
}

const std::string &GrammarAnalyzer::getSymbolType(const std::string &symbol) {
  static const std::string empty;
  auto terminalIt = terminals.find(symbol);
  if (terminalIt != terminals.end()) {
    return terminalIt->second;
  }
  auto nonterminalIt = nonTerminals.find(symbol);
  if (nonterminalIt != nonTerminals.end()) {
    return nonterminalIt->second;
  }
  return empty;
}

} // namespace trg::pg