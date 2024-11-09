#include <sstream>

#include <gtest/gtest.h>

#include <AnalysisExcpetion.h>
#include <LexicalAnalyzer.h>

class LexerTest : public ::testing::Test {};

TEST_F(LexerTest, BasicTokens) {
  std::istringstream input("or and not in ( )");
  LexicalAnalyzer lexer(&input);

  EXPECT_EQ(lexer.nextToken(), Token::OR_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::AND_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::NOT_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::IN_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::LP);
  EXPECT_EQ(lexer.nextToken(), Token::RP);
  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, WhitespaceHandling) {
  std::istringstream input("   or\t\n  and   ");
  LexicalAnalyzer lexer(&input);

  EXPECT_EQ(lexer.nextToken(), Token::OR_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::AND_OPERATOR);
  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, InvalidToken) {
  std::istringstream input("invalid");
  LexicalAnalyzer lexer(&input);

  EXPECT_THROW(lexer.nextToken(), AnalysisException);
}

TEST_F(LexerTest, PartialKeyword) {
  std::istringstream input("an");
  LexicalAnalyzer lexer(&input);

  EXPECT_THROW(lexer.nextToken(), AnalysisException);
}

TEST_F(LexerTest, EmptyInput) {
  std::istringstream input("");
  LexicalAnalyzer lexer(&input);

  EXPECT_EQ(lexer.nextToken(), Token::END);
}

TEST_F(LexerTest, CurrentToken) {
  std::istringstream input("or");
  LexicalAnalyzer lexer(&input);

  lexer.nextToken();
  EXPECT_EQ(lexer.currentToken(), Token::OR_OPERATOR);
}

TEST_F(LexerTest, End) {
  std::istringstream input("or");
  LexicalAnalyzer lexer(&input);

  lexer.nextToken();
  lexer.nextToken();
  EXPECT_EQ(lexer.currentToken(), Token::END);
}