#pragma once

#include <filesystem>
#include <istream>
#include <optional>

#include <Location.h>

namespace trg {

template <typename TokenType, typename SourceFileT = std::filesystem::file_type>
class LexerMixin {
public: /* Constructors and Destructors */
  LexerMixin(std::istream *in, std::optional<SourceFileT> f = {})
      : in(in), currentLocation(f) {}

public: /* Public API */
  TokenType nextToken();

public: /* Getters and Setters */
  const Location<> &getCurrentLocation() const { return currentLocation; }

private: /* Private Data */
  std::istream *in;
  Location<> currentLocation;
};

} // namespace trg