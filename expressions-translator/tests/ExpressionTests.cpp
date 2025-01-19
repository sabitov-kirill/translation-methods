#include "common.h"

TEST(ExpressionTests, BasicArithmetic) {
  checkExpression("+ 2 2", "(2 + 2)");
  checkExpression("* 3 4", "(3 * 4)");
  checkExpression("- 10 5", "(10 - 5)");
  checkExpression("/ 15 3", "(15 / 3)"); 
}

TEST(ExpressionTests, ComplexArithmetic) {
  checkExpression("+ * 2 3 4", "((2 * 3) + 4)");
  checkExpression("- / 10 2 3", "((10 / 2) - 3)");
  checkExpression("* + 1 2 - 4 3", "((1 + 2) * (4 - 3))");
}

TEST(ExpressionTests, LogicalOperators) {
  checkExpression("& 1 1", "(1 && 1)");
  checkExpression("| 1 0", "(1 || 0)");
  checkExpression("! 1", "!1");
}

TEST(ExpressionTests, ComparisonOperators) {
  checkExpression("== 5 5", "(5 == 5)");
  checkExpression("!= 3 4", "(3 != 4)");
  checkExpression("> 7 3", "(7 > 3)");
  checkExpression("< 2 6", "(2 < 6)");
  checkExpression(">= 8 8", "(8 >= 8)");
  checkExpression("<= 4 5", "(4 <= 5)");
}

TEST(ExpressionTests, BitwiseOperators) {
  checkExpression("<< 1 2", "(1 << 2)");
  checkExpression(">> 8 1", "(8 >> 1)");
  checkExpression(">>> 16 2", "(16 >>> 2)");
}

TEST(ExpressionTests, UnaryOperators) {
  checkExpression("n 5", "-5");
  checkExpression("! 5", "!5");
  checkExpression("n n 3", "--3");
}

TEST(ExpressionTests, PowerOperator) {
  checkExpression("^ 2 3", "2.pow(3)");
  checkExpression("^ 3 2", "3.pow(2)");
}

TEST(ExpressionTests, ComplexExpressions) {
  checkExpression("& > 5 3 != 2 2", "((5 > 3) && (2 != 2))");
  checkExpression("| <= 4 4 > 7 3", "((4 <= 4) || (7 > 3))");
  checkExpression("+ * 2 ^ 2 3 4", "((2 * 2.pow(3)) + 4)");
}

TEST(ExpressionTests, IdentifiersAndNumbers) {
  checkExpression("+ x 5", "(x + 5)");
  checkExpression("* y z", "(y * z)");
  checkExpression("^ var 2", "var.pow(2)");
}