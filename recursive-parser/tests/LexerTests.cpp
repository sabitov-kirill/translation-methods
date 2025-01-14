#include <gtest/gtest.h>

#include <AnalysisExcpetion.h>
#include <LexicalAnalyzer.h>

class LexerTest : public ::testing::Test {};

namespace {

LexicalAnalyzer<StringSource> getLexer(std::string str) {
  StringSource ss(std::move(str));
  return {ss};
}

} // namespace

TEST_F(LexerTest, BasicTokens) {
  auto lexer = getLexer("or xor and not in ( )");

  EXPECT_EQ(lexer.nextToken(), Token::OR_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::XOR_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::AND_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::NOT_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::IN_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::LP);
  EXPECT_EQ(lexer.nextToken(), Token::RP);
  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, CurrentToken) {
  auto lexer = getLexer("   \t\n    a xor   s in");

  EXPECT_EQ(lexer.nextToken(), Token::VARIABLE);
  EXPECT_EQ(lexer.currentToken(), Token::VARIABLE);
  EXPECT_EQ(lexer.currentToken(), Token::VARIABLE);
  EXPECT_EQ(lexer.currentToken(), Token::VARIABLE);

  EXPECT_EQ(lexer.nextToken(), Token::XOR_OPERATOR);
  EXPECT_EQ(lexer.currentToken(), Token::XOR_OPERATOR);
  EXPECT_EQ(lexer.currentToken(), Token::XOR_OPERATOR);

  EXPECT_EQ(lexer.nextToken(), Token::VARIABLE);
  EXPECT_EQ(lexer.currentToken(), Token::VARIABLE);

  EXPECT_EQ(lexer.nextToken(), Token::IN_OPERATOR);
  EXPECT_EQ(lexer.currentToken(), Token::IN_OPERATOR);

  EXPECT_EQ(lexer.nextToken(), Token::END);
  EXPECT_EQ(lexer.currentToken(), Token::END);
  EXPECT_EQ(lexer.currentToken(), Token::END);
}

TEST_F(LexerTest, WhitespaceHandling) {
  auto lexer = getLexer("   or\t\n  and   ");

  EXPECT_EQ(lexer.nextToken(), Token::OR_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::AND_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, InvalidToken) {
  auto lexer = getLexer("invalid");

  EXPECT_THROW(lexer.nextToken(), AnalysisException);
}

TEST_F(LexerTest, PartialKeyword) {
  auto lexer = getLexer("an");

  EXPECT_THROW(lexer.nextToken(), AnalysisException);
}

TEST_F(LexerTest, Variables) {
  for (char ch = 'a'; ch <=  'z'; ++ch) {
    auto lexer = getLexer(std::string(1, ch));
    EXPECT_EQ(lexer.nextToken(), Token::VARIABLE);
    EXPECT_EQ(lexer.nextToken(), Token::END);
  }
}

TEST_F(LexerTest, EmptyInput) {
  auto lexer = getLexer("");

  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, ParenthesesSimple) {
  auto lexer = getLexer("()");

  EXPECT_EQ(lexer.nextToken(), Token::LP);
  EXPECT_EQ(lexer.nextToken(), Token::RP);
  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, Parentheses) {
  auto lexer = getLexer("((a) or b)");

  EXPECT_EQ(lexer.nextToken(), Token::LP);
  EXPECT_EQ(lexer.nextToken(), Token::LP);
  EXPECT_EQ(lexer.nextToken(), Token::VARIABLE);
  EXPECT_EQ(lexer.nextToken(), Token::RP);
  EXPECT_EQ(lexer.nextToken(), Token::OR_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::VARIABLE);
  EXPECT_EQ(lexer.nextToken(), Token::RP);
  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, End) {
  auto lexer = getLexer("or");
  lexer.nextToken();
  EXPECT_EQ(lexer.nextToken(), Token::END);


  lexer = getLexer("or and          \n\t");
  lexer.nextToken();
  lexer.nextToken();
  EXPECT_EQ(lexer.nextToken(), Token::END);
}