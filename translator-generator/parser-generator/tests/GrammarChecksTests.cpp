#include <gtest/gtest.h>

#include "GrammarAnalyzer.h"
#include "InputParseContext.h"

using namespace trg::pg;

namespace {

Production makeProduction(
    std::initializer_list<std::pair<std::string, std::string>> symbolPairs) {
  Production prod;
  for (const auto &pair : symbolPairs) {
    prod.symbols.push_back({pair.first, pair.second});
  }
  return prod;
}

} // namespace

TEST(GrammarChecks, DirectLeftRecursion) {
  InputParseContext context;
  GrammarRule rule;
  rule.nonTerminal = "E";

  rule.productions.push_back(makeProduction({{"E", ""}, {"+", ""}, {"T", ""}}));
  rule.productions.push_back(makeProduction({{"T", ""}}));

  context.addRule(std::move(rule));

  GrammarAnalyzer analyzer;
  analyzer.analyze(context);
  EXPECT_TRUE(analyzer.leftRecursion());
}

TEST(GrammarChecks, DetectsDirectLeftRecursion) {
  GrammarAnalyzer analyzer;
  InputParseContext context;

  // A → A b | c
  GrammarRule rule;
  rule.nonTerminal = "A";

  Production p1;
  p1.symbols = {{"A"}, {"b"}};

  Production p2;
  p2.symbols = {{"c"}};

  rule.productions = {p1, p2};
  context.setRules({rule});

  analyzer.analyze(context);
  EXPECT_TRUE(analyzer.leftRecursion());
}

TEST(GrammarChecks, NoLeftRecursion) {
  GrammarAnalyzer analyzer;
  InputParseContext context;

  // A → b A | c
  GrammarRule rule;
  rule.nonTerminal = "A";

  Production p1;
  p1.symbols = {{"b"}, {"A"}};

  Production p2;
  p2.symbols = {{"c"}};

  rule.productions = {p1, p2};
  context.setRules({rule});

  analyzer.analyze(context);
  EXPECT_FALSE(analyzer.leftRecursion());
}

TEST(GrammarChecks, DetectsLeftFactoring) {
  GrammarAnalyzer analyzer;
  InputParseContext context;

  // A → abc d | abc e
  GrammarRule rule;
  rule.nonTerminal = "A";

  Production p1;
  p1.symbols = {{"abc"}, {"d"}};

  Production p2;
  p2.symbols = {{"abc"}, {"e"}};

  rule.productions = {p1, p2};
  context.setRules({rule});

  analyzer.analyze(context);
  EXPECT_TRUE(analyzer.leftFactoring());
}

TEST(GrammarChecks, NoLeftFactoring) {
  GrammarAnalyzer analyzer;
  InputParseContext context;

  // A → abc | def
  GrammarRule rule;
  rule.nonTerminal = "A";

  Production p1;
  p1.symbols = {{"abc"}};

  Production p2;
  p2.symbols = {{"def"}};

  rule.productions = {p1, p2};
  context.setRules({rule});

  analyzer.analyze(context);
  EXPECT_FALSE(analyzer.leftFactoring());
}

TEST(GrammarChecks, EmptyProductionsNoFactoring) {
  GrammarAnalyzer analyzer;
  InputParseContext context;

  // A → ε | b
  GrammarRule rule;
  rule.nonTerminal = "A";

  Production p1;
  Production p2;
  p2.symbols = {{"b"}};

  rule.productions = {p1, p2};
  context.setRules({rule});

  analyzer.analyze(context);
  EXPECT_FALSE(analyzer.leftFactoring());
}

TEST(GrammarChecks, MultipleRulesLeftRecursion) {
  GrammarAnalyzer analyzer;
  InputParseContext context;

  // A → A b | c
  // B → B a | d
  GrammarRule rule1, rule2;
  rule1.nonTerminal = "A";
  rule2.nonTerminal = "B";

  Production p1, p2, p3, p4;
  p1.symbols = {{"A"}, {"b"}};
  p2.symbols = {{"c"}};
  p3.symbols = {{"B"}, {"a"}};
  p4.symbols = {{"d"}};

  rule1.productions = {p1, p2};
  rule2.productions = {p3, p4};
  context.setRules({rule1, rule2});

  analyzer.analyze(context);
  EXPECT_TRUE(analyzer.leftRecursion());
}

TEST(GrammarChecks, BooleanAlgebraWithLeftRecursion) {
  InputParseContext context;

  GrammarRule ruleE;
  ruleE.nonTerminal = "E";
  ruleE.productions.push_back(
      makeProduction({{"E", ""}, {"or", ""}, {"X", ""}}));  // E -> E or X
  ruleE.productions.push_back(makeProduction({{"X", ""}})); // E -> X

  GrammarRule ruleX;
  ruleX.nonTerminal = "X";
  ruleX.productions.push_back(
      makeProduction({{"X", ""}, {"xor", ""}, {"T", ""}})); // X -> X xor T
  ruleX.productions.push_back(makeProduction({{"T", ""}})); // X -> T

  GrammarRule ruleT;
  ruleT.nonTerminal = "T";
  ruleT.productions.push_back(
      makeProduction({{"T", ""}, {"and", ""}, {"N", ""}})); // T -> T and N
  ruleT.productions.push_back(makeProduction({{"N", ""}})); // T -> N

  GrammarRule ruleN;
  ruleN.nonTerminal = "N";
  ruleN.productions.push_back(
      makeProduction({{"not", ""}, {"N", ""}}));            // N -> not N
  ruleN.productions.push_back(makeProduction({{"M", ""}})); // N -> M

  GrammarRule ruleM;
  ruleM.nonTerminal = "M";
  ruleM.productions.push_back(
      makeProduction({{"M", ""}, {"in", ""}, {"S", ""}})); // M -> M in S
  ruleM.productions.push_back(makeProduction(
      {{"M", ""}, {"not in", ""}, {"S", ""}}));             // M -> M not in S
  ruleM.productions.push_back(makeProduction({{"F", ""}})); // M -> F

  context.addRule(std::move(ruleE));
  context.addRule(std::move(ruleX));
  context.addRule(std::move(ruleT));
  context.addRule(std::move(ruleN));
  context.addRule(std::move(ruleM));

  GrammarAnalyzer analyzer;
  analyzer.analyze(context);
  EXPECT_TRUE(analyzer.leftRecursion());
}

TEST(GrammarChecks, BooleanAlgebraWithoutLeftRecursion) {
  InputParseContext context;

  GrammarRule ruleE;
  ruleE.nonTerminal = "E";
  ruleE.productions.push_back(
      makeProduction({{"X", ""}, {"E'", ""}})); // E -> X E'

  GrammarRule ruleEPrime;
  ruleEPrime.nonTerminal = "E'";
  ruleEPrime.productions.push_back(
      makeProduction({{"or", ""}, {"X", ""}, {"E'", ""}})); // E' -> or X E'
  ruleEPrime.productions.push_back(makeProduction({}));     // E' -> ε

  GrammarRule ruleX;
  ruleX.nonTerminal = "X";
  ruleX.productions.push_back(
      makeProduction({{"T", ""}, {"X'", ""}})); // X -> T X'

  GrammarRule ruleXPrime;
  ruleXPrime.nonTerminal = "X'";
  ruleXPrime.productions.push_back(
      makeProduction({{"xor", ""}, {"T", ""}, {"X'", ""}})); // X' -> xor T X'
  ruleXPrime.productions.push_back(makeProduction({}));      // X' -> ε

  context.addRule(std::move(ruleE));
  context.addRule(std::move(ruleEPrime));
  context.addRule(std::move(ruleX));
  context.addRule(std::move(ruleXPrime));

  GrammarAnalyzer analyzer;
  analyzer.analyze(context);
  EXPECT_FALSE(analyzer.leftRecursion());
}

TEST(GrammarChecks, SymbolTypesCheck) {
  InputParseContext context;

  Token tokenOr;
  tokenOr.name = "or";
  tokenOr.type = "OR_OP";

  Token tokenXor;
  tokenXor.name = "xor";
  tokenXor.type = "XOR_OP";

  Token tokenNum;
  tokenNum.name = "num";
  tokenNum.type = "NUMBER";

  context.addToken(std::move(tokenOr));
  context.addToken(std::move(tokenXor));
  context.addToken(std::move(tokenNum));

  GrammarRule ruleE;
  ruleE.nonTerminal = "E";
  ruleE.type = "Expression";
  ruleE.productions.push_back(makeProduction({{"X", ""}, {"E'", ""}}));

  GrammarRule ruleX;
  ruleX.nonTerminal = "X";
  ruleX.type = "Term";
  ruleX.productions.push_back(makeProduction({{"num", ""}}));

  context.addRule(std::move(ruleE));
  context.addRule(std::move(ruleX));

  GrammarAnalyzer analyzer;
  analyzer.analyze(context);

  EXPECT_EQ(analyzer.getSymbolType("or"), "OR_OP");
  EXPECT_EQ(analyzer.getSymbolType("xor"), "XOR_OP");
  EXPECT_EQ(analyzer.getSymbolType("num"), "NUMBER");
  EXPECT_EQ(analyzer.getSymbolType("E"), "Expression");
  EXPECT_EQ(analyzer.getSymbolType("X"), "Term");
  EXPECT_EQ(analyzer.getSymbolType("invalid"), "");
}