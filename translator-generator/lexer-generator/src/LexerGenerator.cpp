#include "LexerGenerator.h"
#include "InputLexicalAnalyzer.h"
#include "InputParseContext.h"
#include "InputParser.tab.h"

#include <stdexcept>

using namespace trg;

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

  *out << "#include <regex>\n"
       << "#include <string>\n"
       << "#include <vector>\n\n"
       << "#include <LexerBase.h>\n\n";
  if (!header.empty()) {
    *out << header << "\n\n";
  }

  *out << "enum class Token {\n"
       << "    ERROR,\n"
       << "    END,\n";

  size_t tokenId = 2;
  for (const Rule &rule : rules) {
    *out << "    TOKEN_" << tokenId++ << ",\n";
  }
  *out << "};\n\n";

  *out << "class GeneratedLexer : public trg::LexerMixin<Token> {\n"
       << "public:\n"
       << "    using LexerMixin::LexerMixin;\n\n"
       << "    Token nextToken() override {\n"
       << "        if (!in) return Token::END;\n\n"
       << "        std::string line;\n"
       << "        if (!std::getline(*in, line)) return Token::END;\n\n"
       << "        currentLocation.lines(1);\n\n";

  for (const Rule &rule : rules) {
    *out << "        {\n";
    if (std::holds_alternative<Rule::RegexPattern>(rule.pattern)) {
      const auto &pattern = std::get<Rule::RegexPattern>(rule.pattern);
      *out << "            static const std::regex pattern(R\"(" << pattern.body
           << ")\");\n"
           << "            std::smatch match;\n"
           << "            if (std::regex_search(line, match, pattern)) {\n"
           << "                " << rule.action << "\n"
           << "            }\n";
    } else if (std::holds_alternative<Rule::SpecialPattern>(rule.pattern)) {
      const auto &special = std::get<Rule::SpecialPattern>(rule.pattern);
      if (special == Rule::SpecialPattern::END_OF_FILE) {
        *out << "            if (in->eof()) {\n"
             << "                " << rule.action << "\n"
             << "            }\n";
      }
    }
    *out << "        }\n";
  }

  *out << "        return Token::ERROR;\n"
       << "    }\n"
       << "};\n";
}