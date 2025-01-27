#include <gtest/gtest.h>

#include <sstream>

#include "lexer.h"
#include "parser.h"

class CalculatorTest : public ::testing::Test {
protected:
  int evaluate(const std::string& expr) {
    std::stringstream ss(expr);
    Lexer lexer(&ss);
    Parser parser(lexer);
    return parser.parse();
  }
};

TEST_F(CalculatorTest, BasicArithmetic) {
  EXPECT_EQ(evaluate("2 + 2"), 4);
  EXPECT_EQ(evaluate("3 - 1"), 2);
  EXPECT_EQ(evaluate("4 * 5"), 20);
  EXPECT_EQ(evaluate("10 / 2"), 5);
}

TEST_F(CalculatorTest, Precedence) {
  EXPECT_EQ(evaluate("2 + 3 * 4"), 14);
  EXPECT_EQ(evaluate("10 - 2 * 3"), 4);
  EXPECT_EQ(evaluate("(2 + 3) * 4"), 20);
  EXPECT_EQ(evaluate("8 / 2 + 3"), 7);
}

TEST_F(CalculatorTest, ComplexExpressions) {
  EXPECT_EQ(evaluate("(5 + 3) * (2 + 1)"), 24);
  EXPECT_EQ(evaluate("10 / (2 + 3)"), 2);
  EXPECT_EQ(evaluate("2 * 3 + 4 * 5"), 26);
  EXPECT_EQ(evaluate("(8 + 2) / 2 - 3"), 2);
}

TEST_F(CalculatorTest, NegativeNumbers) {
  EXPECT_EQ(evaluate("-5 + 3"), -2);
  EXPECT_EQ(evaluate("2 * -3"), -6);
  EXPECT_EQ(evaluate("(-4) * (-5)"), 20);
  EXPECT_EQ(evaluate("-10 / -2"), 5);
}