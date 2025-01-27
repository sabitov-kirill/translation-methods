#pragma once

#include <istream>
#include <ostream>
#include <string>

namespace trg::lg {

class LexerGenerator {
public: /* Constructors and Destructors */
  LexerGenerator(std::istream *in, std::ostream *out,
                 const std::string &filepath = "")
      : in(in), out(out), filepath(filepath) {}

public: /* Public API */
  void generate();

private: /* Private Data */
  std::istream *in;
  std::ostream *out;
  std::string filepath;
};

} // namespace trg