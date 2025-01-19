#pragma once

#include <gtest/gtest.h>

#include <sstream>

#include "Context.h"
#include "LexicalAnalyzer.h"
#include "Parser.tab.h"

inline void checkExpression(const std::string &input,
                            const std::string &expected,
                            bool assumeMain = true) {
  auto context = book::Context{};
  auto stream = std::make_unique<std::istringstream>(input);
  auto lexer = book::LexicalAnalyzer(stream.get(), context);
  auto parser = book::Parser(context, lexer);

  ASSERT_EQ(parser(), 0);
  ASSERT_TRUE(context.getResult());

  EXPECT_EQ(*context.getResult(),
            assumeMain ? "fn main() {\n  " + expected + ";\n}\n" : expected);
}
