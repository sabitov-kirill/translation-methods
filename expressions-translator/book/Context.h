#pragma once
#include "location.hh"
#include <optional>
#include <string>

namespace book {

class Context {
public:
  Context() = default;
  Context(std::string filename) : filename(std::move(filename)) {
    location = book::location(&*this->filename);
  }

  location &getLocation() { return location; }
  const location &getLocation() const { return location; }

  std::optional<std::string> getResult() const { return result; }
  void setResult(std::string result) { this->result = result; }

private:
  location location{};
  std::optional<std::string> filename;
  std::optional<std::string> result;
};

} // namespace book