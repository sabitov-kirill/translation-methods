#include <cstddef>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <sstream>

#include "GrammarAnalyzer.h"
#include "InputLexicalAnalyzer.h"
#include "InputParseContext.h"
#include "ParserGenerator.h"

namespace trg::pg {

void ParserGenerator::generate() {
  InputLexicalAnalyzer lexer(in, context);
  InputParser parser(context, lexer);

  parser.set_debug_level(0);
  if (parser.parse() != 0) {
    throw std::runtime_error("Failed to parse input grammar");
  }

  analyzer.analyze(context);

  {
    for (const auto &rule : context.getRules()) {
      std::cout << rule.nonTerminal << "(" << rule.argument
                << "): " << rule.type << " -> ";
      for (size_t i = 0; i < rule.productions.size(); ++i) {
        for (const auto &symbol : rule.productions[i].symbols) {
          std::cout << "'" << symbol.name << "' ";
        }
        if (rule.productions[i].symbols.empty()) {
          std::cout << "“ε”";
        }
        if (i != rule.productions.size() - 1) {
          std::cout << "| ";
        }
      }
      std::cout << std::endl;
    }

    std::cout << "\nFirst sets:" << std::endl;
    auto setItemRepr = [&](auto i) {
      return i == EPSILON ? "“ε”" : i == EOF_SYMBOL ? "“EOF”" : i;
    };
    for (const auto &[symbol, set] : analyzer.getFirstSets()) {
      std::cout << symbol << ": { ";
      for (const auto &item : set) {
        std::cout << setItemRepr(item) << " ";
      }
      std::cout << "}" << std::endl;
    }

    std::cout << "\nFollow sets:" << std::endl;
    for (const auto &[symbol, set] : analyzer.getFollowSets()) {
      std::cout << symbol << ": { ";
      for (const auto &item : set) {
        std::cout << setItemRepr(item) << " ";
      }
      std::cout << "}" << std::endl;
    }

    std::cout << "\nLeft Recursion in Original Grammar: "
              << analyzer.leftRecursion() << '\n';
  }

  *out << "#pragma once\n\n"
       << "#include <memory>\n"
       << "#include <variant>\n"
       << "#include <sstream>\n"
       << "#include <iostream>\n\n"
       << "#include \"ParserBase.h\"\n"
       << "#include \"LexerBase.h\"\n"
       << context.getHeader() << "\n\n";

  generateTokens();
  generateClass();
}

void ParserGenerator::generateTokens() {
  std::set<std::string> types;
  bool haveEmpty{};

  for (const auto &token : context.getTokens()) {
    if (!token.type.empty()) {
      types.insert(token.type);
      symbolTypes[token.name] = token.type;
    } else {
      haveEmpty = true;
    }
  }
  for (const auto &rule : context.getRules()) {
    if (!rule.type.empty()) {
      types.insert(rule.type);
      symbolTypes[rule.nonTerminal] = rule.type;
    } else {
      haveEmpty = true;
    }
  }

  if (haveEmpty) {
    *out << "namespace internal { class EmptyValue {}; }\n\n";
    types.insert("internal::EmptyValue");
  }

  *out << "\nusing TokenValue = std::variant<"
       << std::accumulate(
              std::next(types.begin()), types.end(), *types.begin(),
              [](const auto &a, const auto &b) { return a + ", " + b; })
       << ">;\n\n";

  *out << "enum class TokenType {\n";
  for (const auto &token : context.getTokens()) {
    *out << "  " << token.name << ",\n";
  }
  *out << "  END\n};\n\n";

  *out << "struct Token {\n"
       << "  using Type = TokenType;\n"
       << "  TokenType type;\n"
       << "  TokenValue value;\n"
       << "};\n\n";

  std::stringstream ss;
  ss << 'a';
  ss.str();

  *out << "inline std::string getTokenRepresentation(Token t) {\n"
       << "  std::stringstream ss;\n"
       << "  switch(t.type) {\n";
  for (const auto &token : context.getTokens()) {
    *out << "  case TokenType::" << token.name << ": ss << \"" << token.name
         << "(\"";
    if (!token.type.empty()) {
      *out << " << std::get<" << token.type << ">(t.value) " << " <<";
    }
    *out << "\")\"; break;\n";
  }
  *out << "  case TokenType::END: ss << \"END\";\n"
       << "  }\n"
       << "  return ss.str();\n"
       << "}\n\n";
}

void ParserGenerator::generateClass() {
  *out << "class Parser : public trg::ParserBase<Token> {\n"
       << "public:\n"
       << "  Parser(trg::LexerMixin<Token>& lexer) : "
          "trg::ParserBase<Token>(lexer) {}\n\n"
       << "  auto parse() {\n";
  if (!context.getRules()[0].type.empty()) {
    *out << "    auto result = ";
  }
  *out << context.getRules()[0].nonTerminal;
  *out << "();\n"
       << "    match(TokenType::END);\n";
  if (!context.getRules()[0].type.empty()) {
    *out << "    return result;\n";
  }
  *out << "  }\n"
       << "private:\n";

  for (const auto &rule : context.getRules()) {
    generateNonTerminal(rule);
  }

  *out << "};\n";
}

std::string
ParserGenerator::replaceTranslationSymbols(const std::string &action) {
  std::string result = action;
  std::regex returnPattern("\\$\\$");
  std::regex dollarPattern("\\$(\\d+)");
  result = std::regex_replace(result, returnPattern, "result");
  result = std::regex_replace(result, dollarPattern, "var$1");
  return result;
}

void ParserGenerator::generateNonTerminal(const GrammarRule &rule) {
  bool hasReturnType = !rule.type.empty();
  *out << "  " << (hasReturnType ? rule.type : "void") << " "
       << rule.nonTerminal << "(" << rule.argument << ") {\n";

  if (hasReturnType) {
    *out << "    " << rule.type << " result{};\n\n";
  }
  *out << "    switch (currentToken.type) {\n";

  std::set<std::string> handledCases;
  for (const auto &prod : rule.productions) {
    std::set<std::string> validTokens;

    if (prod.symbols.empty()) {
      const auto &followSet = analyzer.getFollowSets().at(rule.nonTerminal);
      for (const auto &symbol : followSet) {
        if (symbol == EPSILON)
          continue;
        if (symbol == EOF_SYMBOL) {
          validTokens.insert("END");
        } else {
          validTokens.insert(symbol);
        }
      }
    } else {
      const auto &symbol = prod.symbols[0];
      if (analyzer.isTerminal(symbol.name)) {
        if (symbol.name == EOF_SYMBOL) {
          validTokens.insert("END");
        } else {
          validTokens.insert(symbol.name);
        }
      } else {
        const auto &firstSet = analyzer.getFirstSets().at(symbol.name);
        for (const auto &first : firstSet) {
          if (first == EPSILON)
            continue;
          if (first == EOF_SYMBOL) {
            validTokens.insert("END");
          } else {
            validTokens.insert(first);
          }
        }
      }
    }

    bool hasValidTokens = false;
    for (const auto &token : validTokens) {
      if (handledCases.insert(token).second) {
        *out << "    case TokenType::" << token << ":\n";
        hasValidTokens = true;
      }
    }

    if (hasValidTokens) {
      *out << "    {\n";
      for (size_t i = 0; i < prod.symbols.size(); i++) {
        const auto &symbol = prod.symbols[i];
        if (analyzer.isTerminal(symbol.name)) {
          if (symbolTypes.contains(symbol.name)) {
            *out << "      auto var" << i + 1 << " = std::get<"
                 << symbolTypes[symbol.name] << ">(currentToken.value);\n";
          }
          *out << "      match(TokenType::" << symbol.name << ");\n";
        } else {
          *out << "      auto var" << i + 1 << " = " << symbol.name << "("
               << replaceTranslationSymbols(symbol.argument) << ");\n";
        }
      }

      if (!prod.action.empty()) {
        *out << "\n      " << replaceTranslationSymbols(prod.action) << "\n";
      }
      *out << "      return" << (hasReturnType ? " result" : "") << ";\n";
      *out << "    }\n";
    }
  }

  *out << "    default:\n"
       << "      throw trg::ParserError(std::string(\"Unexpected token: \") + "
          "getTokenRepresentation(currentToken), "
          "lexer.getCurrentLocation());\n"
       << "    }\n";

  *out << "  }\n\n";
}

} // namespace trg::pg