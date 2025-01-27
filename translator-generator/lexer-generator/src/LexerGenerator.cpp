#include <stdexcept>

#include "InputLexicalAnalyzer.h"
#include "InputParseContext.h"
#include "InputParser.tab.h"
#include "LexerGenerator.h"

using namespace trg::lg;

void LexerGenerator::generate() {
  InputParseContext context;
  if (!filepath.empty()) {
    context.getLexerLocation().initialize(&filepath);
  }
  InputLexicalAnalyzer lexer(in, context);
  InputParser parser(context, lexer);

  if (parser.parse() != 0) {
    throw std::runtime_error("Failed to parse input");
  }

  const std::string &header = context.getHeader();
  const std::vector<Rule> &rules = context.getRules();

  *out << "#pragma once\n\n"
       << "#include <regex>\n"
       << "#include <string>\n"
       << "#include <string_view>\n\n";
  if (!header.empty()) {
    *out << header << "\n\n";
  }

  *out << "Token " << context.getClassName() << "::nextToken() {\n"
       << "  if (isEof()) {\n    return Token(TokenType::END);\n  }\n\n";

  for (const Rule &rule : rules) {
    *out << "  {\n";
    if (std::holds_alternative<Rule::RegexPattern>(rule.pattern)) {
      const auto &pattern = std::get<Rule::RegexPattern>(rule.pattern);
      *out << "    static const std::regex pattern(R\"(" << pattern.body
           << ")\", std::regex::multiline);\n"
           << "    auto match = matchPattern(pattern);\n"
           << "    if (match.success) {\n"
           << "      updateLocation(match.matched);\n"
           << "      " << rule.action << "\n"
           << "    }\n";
    } else if (std::holds_alternative<Rule::SpecialPattern>(rule.pattern)) {
      const auto &special = std::get<Rule::SpecialPattern>(rule.pattern);
      if (special == Rule::SpecialPattern::END_OF_FILE) {
        *out << "    if (isEof()) {\n"
             << "      " << rule.action << "\n"
             << "    }\n";
      }
    }
    *out << "  }\n";
  }

  *out << "  lexerError(\"Unexpected character: \" + std::string(1, "
          "getRemainingInput()[0]));\n"
       << "}\n";
}
