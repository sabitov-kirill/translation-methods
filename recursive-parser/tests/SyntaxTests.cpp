#include <gtest/gtest.h>

#include <SyntaxAnalyzer.h>

class SyntaxAnalyzerTest : public ::testing::Test {
protected:
    NameASTNode parseExpression(const std::string &input) {
        StringSource source(input);
        LexicalAnalyzer<StringSource> lexer(source);
        StringSyntaxAnalyzer analyzer(lexer);
        return analyzer.parse();
    }

    void verifyNode(const NameASTNode &node, const std::string &expectedData, size_t expectedChildren = 0) {
        EXPECT_EQ(node.data, expectedData);
        EXPECT_EQ(node.children.size(), expectedChildren);
    }
};

// Basic Expression Tests
TEST_F(SyntaxAnalyzerTest, SingleVariable) {
    auto ast = parseExpression("x");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[1], "E'", 0);
}

TEST_F(SyntaxAnalyzerTest, SimpleAnd) {
    auto ast = parseExpression("x and y");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    ASSERT_EQ(ast.children[0].children.size(), 2);
    verifyNode(ast.children[0].children[0], "T", 2);
}

TEST_F(SyntaxAnalyzerTest, SimpleOr) {
    auto ast = parseExpression("x or y");
    verifyNode(ast, "E", 2);
    ASSERT_EQ(ast.children[1].children.size(), 2);
    verifyNode(ast.children[1], "E'", 2);
}

TEST_F(SyntaxAnalyzerTest, SimpleXor) {
    auto ast = parseExpression("x xor y");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    ASSERT_EQ(ast.children[0].children[1].children.size(), 2);
}

// Nested Expression Tests
TEST_F(SyntaxAnalyzerTest, ParenthesizedExpression) {
    auto ast = parseExpression("(x and y)");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[0].children[0], "T", 2);
}

TEST_F(SyntaxAnalyzerTest, ComplexNestedExpression) {
    auto ast = parseExpression("(x and y) or (a xor b)");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[1], "E'", 2);
}

// Not Operator Tests
TEST_F(SyntaxAnalyzerTest, SimpleNot) {
    auto ast = parseExpression("not x");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[0].children[0], "T", 2);
    verifyNode(ast.children[0].children[0].children[0], "N", 1);
}

TEST_F(SyntaxAnalyzerTest, DoubleNot) {
    auto ast = parseExpression("not not x");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[0].children[0], "T", 2);
    verifyNode(ast.children[0].children[0].children[0], "N", 1);
}

// In Operator Tests
TEST_F(SyntaxAnalyzerTest, SimpleIn) {
    auto ast = parseExpression("x in y");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[0].children[0], "T", 2);
}

TEST_F(SyntaxAnalyzerTest, NotIn) {
    auto ast = parseExpression("x not in y");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[0].children[0], "T", 2);
}

// Error Cases
TEST_F(SyntaxAnalyzerTest, UnmatchedParenthesis) {
    EXPECT_THROW(parseExpression("(x and y"), AnalysisException);
    EXPECT_THROW(parseExpression("x and y)"), AnalysisException);
}

TEST_F(SyntaxAnalyzerTest, InvalidOperatorCombinations) {
    EXPECT_THROW(parseExpression("x and or y"), AnalysisException);
    EXPECT_THROW(parseExpression("x xor and y"), AnalysisException);
    EXPECT_THROW(parseExpression("and x"), AnalysisException);
}

TEST_F(SyntaxAnalyzerTest, EmptyExpression) {
    EXPECT_THROW(parseExpression(""), AnalysisException);
}

TEST_F(SyntaxAnalyzerTest, InvalidCharacters) {
    EXPECT_THROW(parseExpression("x $ y"), AnalysisException);
    EXPECT_THROW(parseExpression("@x"), AnalysisException);
}

// Edge Cases
TEST_F(SyntaxAnalyzerTest, LongChainedExpressions) {
    auto ast = parseExpression("a and b and c and d and e");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
}

TEST_F(SyntaxAnalyzerTest, DeeplyNestedParentheses) {
    auto ast = parseExpression("(((x)))");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
}

TEST_F(SyntaxAnalyzerTest, MixedOperatorPrecedence) {
    auto ast = parseExpression("a and b or c xor d and not e");
    verifyNode(ast, "E", 2);
    verifyNode(ast.children[0], "X", 2);
    verifyNode(ast.children[1], "E'", 2);
}

// Helper method tests
TEST_F(SyntaxAnalyzerTest, CurrentTokenIs) {
    StringSource source("x and y");
    LexicalAnalyzer<StringSource> lexer(source);
    StringSyntaxAnalyzer analyzer(lexer);
    EXPECT_NO_THROW(analyzer.parse());
}

TEST_F(SyntaxAnalyzerTest, Match) {
    StringSource source("x and y");
    LexicalAnalyzer<StringSource> lexer(source);
    StringSyntaxAnalyzer analyzer(lexer);
    EXPECT_NO_THROW(analyzer.parse());
}