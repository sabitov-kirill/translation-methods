#include <gtest/gtest.h>

#include "GrammarAnalyzer.h"

namespace trg::pg::tests {

class GrammarFirstFollowTest : public ::testing::Test {
protected:
  void SetUp() override { analyzer = std::make_unique<GrammarAnalyzer>(); }

  void setupGrammar(
      const std::vector<std::string> &tokens,
      const std::vector<
          std::pair<std::string, std::vector<std::vector<std::string>>>>
          &rules) {
    for (const auto &name : tokens) {
      context.addToken({name, ""});
    }
    for (const auto &[nonTerminal, productions] : rules) {
      context.addRule(makeRule(nonTerminal, productions));
    }
    analyzer->analyze(context);
  }

  void verifyFirstAndFollowSets(const SymbolSetsMap &expectedFirst,
                                const SymbolSetsMap &expectedFollow) {
    verifyFirstSets(expectedFirst);
    verifyFollowSets(expectedFollow);
  }

private:
  void verifyFirstSets(const SymbolSetsMap &expectedFirst) {
    const auto &actual = analyzer->getFirstSets();
    EXPECT_EQ(actual.size(), expectedFirst.size());
    for (const auto &[symbol, expectedSet] : expectedFirst) {
      ASSERT_TRUE(actual.contains(symbol));
      EXPECT_EQ(actual.at(symbol), expectedSet);
    }
  }

  void verifyFollowSets(const SymbolSetsMap &expectedFollow) {
    const auto &actual = analyzer->getFollowSets();
    EXPECT_EQ(actual.size(), expectedFollow.size());
    for (const auto &[symbol, expectedSet] : expectedFollow) {
      ASSERT_TRUE(actual.contains(symbol));
      EXPECT_EQ(actual.at(symbol), expectedSet);
    }
  }

  GrammarRule
  makeRule(const std::string &nonTerminal,
           const std::vector<std::vector<std::string>> &productions) {
    GrammarRule rule;
    rule.nonTerminal = nonTerminal;
    rule.type = "void";
    for (const auto &prod : productions) {
      Production p;
      for (const auto &symbol : prod) {
        p.symbols.push_back({symbol});
      }
      rule.productions.push_back(p);
    }
    return rule;
  }

  std::unique_ptr<GrammarAnalyzer> analyzer;
  InputParseContext context;
};

TEST_F(GrammarFirstFollowTest, SimpleTerminalProduction) {
  setupGrammar({"a"}, //
               {{"S", {{"a"}}}});

  verifyFirstAndFollowSets({{"S", {"a"}}}, //
                           {{"S", {EOF_SYMBOL}}});
}

TEST_F(GrammarFirstFollowTest, TwoNonTerminals) {
  setupGrammar({"a"}, //
               {{"S", {{"A"}}}, {"A", {{"a"}}}});

  verifyFirstAndFollowSets({{"S", {"a"}}, {"A", {"a"}}}, //
                           {{"S", {EOF_SYMBOL}}, {"A", {EOF_SYMBOL}}});
}

TEST_F(GrammarFirstFollowTest, TwoNonTerminalsEps) {
  setupGrammar({"a"}, //
               {{"S", {{"A"}, {{}}}}, {"A", {{"a"}}}});

  verifyFirstAndFollowSets({{"S", {"a", EPSILON}}, {"A", {"a"}}}, //
                           {{"S", {EOF_SYMBOL}}, {"A", {EOF_SYMBOL}}});
}

TEST_F(GrammarFirstFollowTest, ComplexGrammarWithEpsilons) {
  setupGrammar({"a", "b"}, // tokens
               {
                   {"S", {{"A", "B"}, {{}}}}, // S -> AB | ε
                   {"A", {{"a"}, {{}}}},      // A -> a | ε
                   {"B", {{"b"}, {{}}}}       // B -> b | ε
               });

  verifyFirstAndFollowSets(
      {{"S", {"a", "b", EPSILON}},
       {"A", {"a", EPSILON}},
       {"B", {"b", EPSILON}}},
      {{"S", {EOF_SYMBOL}}, {"A", {"b", EOF_SYMBOL}}, {"B", {EOF_SYMBOL}}});
}

TEST_F(GrammarFirstFollowTest, ComplexGrammarWithMultipleProductions) {
  setupGrammar({"a", "b", "c"}, // terminals
               {
                   {"S",
                    {{"A", "b", "C"}, // S -> AbC
                     {"B", "C"},      // S -> BC
                     {"C"}}},         // S -> C
                   {"A",
                    {{"a", "b", "C"},   // A -> abC
                     {"B", "C"},        // A -> BC
                     {{}}}},            // A -> ε
                   {"B", {{"b", "c"}}}, // B -> bc
                   {"C",
                    {{"c"}, // C -> c
                     {{}}}} // C -> ε
               });

  verifyFirstAndFollowSets(
      {{"S", {"a", "b", "c", EPSILON}},
       {"A", {"a", "b", EPSILON}},
       {"B", {"b"}},
       {"C", {"c", EPSILON}}},
      {{"S", {EOF_SYMBOL}},
       {"A", {"b"}},
       {"B", {"b", "c", EOF_SYMBOL}},
       {"C", {"b", EOF_SYMBOL}}});
}

TEST_F(GrammarFirstFollowTest, ArithmeticExpressionGrammar) {
  setupGrammar({"+", "*", "(", ")", "a"}, // terminals
               {
                   {"E", {{"T", "E_"}}}, // E  -> T E'
                   {"E_",
                    {{"+", "T", "E_"},   // E' -> + T E'
                     {{}}}},             // E' -> ε
                   {"T", {{"P", "T_"}}}, // T  -> P T'
                   {"T_",
                    {{"*", "P", "T_"}, // T' -> * P T'
                     {{}}}},           // T' -> ε
                   {"P",
                    {{"(", "E", ")"}, // P  -> (E)
                     {"a"}}}          // P  -> a
               });

  verifyFirstAndFollowSets(
      {{"E_", {"+", EPSILON}},
       {"T_", {"*", EPSILON}},
       {"P", {"(", "a"}},
       {"T", {"(", "a"}},
       {"E", {"(", "a"}}},
      {{"E_", {EOF_SYMBOL, ")"}},
       {"T_", {EOF_SYMBOL, "+", ")"}},
       {"P", {EOF_SYMBOL, "+", ")", "*"}},
       {"T", {EOF_SYMBOL, "+", ")"}},
       {"E", {EOF_SYMBOL, ")"}}});
}

} // namespace trg::pg::tests