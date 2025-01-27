#pragma once

#include <filesystem>
#include <istream>
#include <iterator>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

#include <Location.h>

namespace trg {

class LexerError : public std::runtime_error {
public:
  LexerError(const std::string &message, const Location<> &location)
      : std::runtime_error(message), location_(location) {}

  const Location<> &getLocation() const { return location_; }

private:
  Location<> location_;
};

template <typename TokenType, typename SourceFileT = std::filesystem::file_type>
class LexerMixin {
public: /* Constructors and Destructors */
  LexerMixin(std::istream *in, std::optional<SourceFileT> f = {})
      : in(in), currentLocation(f) {
    // Read entire input stream into buffer
    if (in) {
      std::stringstream buffer;
      buffer << in->rdbuf();
      inputBuffer = buffer.str();
      currentPosition = 0;
    }
  }

public: /* Public API */
  virtual TokenType nextToken() = 0;

protected: /* Protected helpers */
  std::string_view getRemainingInput() const {
    return std::string_view(inputBuffer).substr(currentPosition);
  }

  void advance(size_t length) { currentPosition += length; }

  bool isEof() const { return !in || currentPosition >= inputBuffer.length(); }

  [[noreturn]] void lexerError(const std::string &message) {
    throw LexerError(message, currentLocation);
  }

  struct MatchResult {
    std::string matched;
    std::iterator_traits<std::string_view::const_iterator>::difference_type
        length;
    bool success;
  };

  MatchResult matchPattern(const std::regex &pattern) {
    std::string_view input = getRemainingInput();
    std::match_results<std::string_view::const_iterator> match;

    if (!std::regex_search(input.begin(), input.end(), match, pattern)) {
      return {std::string(), 0, false};
    }

    return {std::string(match[0]), match[0].length(), true};
  }

  void updateLocation(std::string_view matched) {
    size_t newlines = 0;
    size_t last_newline_pos = 0;

    for (size_t i = 0; i < matched.length(); ++i) {
      if (matched[i] == '\n') {
        newlines++;
        last_newline_pos = i;
      }
    }

    advance(matched.length());

    if (newlines > 0) {
      size_t final_column = matched.length() - last_newline_pos - 1;
      for (size_t i = 0; i < newlines; ++i) {
        currentLocation.newLine();
      }
      currentLocation.advance(final_column);
    } else {
      currentLocation.advance(matched.length());
    }
  }

public: /* Getters and Setters */
  const Location<> &getCurrentLocation() const { return currentLocation; }

private: /* Private Data */
  std::istream *in;
  Location<> currentLocation;
  std::string inputBuffer;
  size_t currentPosition{0};
};

} // namespace trg