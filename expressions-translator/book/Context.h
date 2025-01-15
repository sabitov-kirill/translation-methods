#pragma once
#include <optional>
#include <string>
#include <unordered_set>

#include "location.hh"

namespace book {

class Context {
public:
  Context() = default;
  Context(std::string filename) : filename(std::move(filename)) {
    location = book::location(&*this->filename);
    location.lines();
  }

  location &getLocation() { return location; }
  const location &getLocation() const { return location; }

  std::optional<std::string> getResult() const { return result; }
  void setResult(std::string result) { this->result = result; }

  bool hasVariable(const std::string &name) const {
    return variables.find(name) != variables.end();
  }

  void addVariable(const std::string &name) { variables.insert(name); }

private:
  location location{};
  std::optional<std::string> filename;
  std::optional<std::string> result;
  std::unordered_set<std::string> variables;
};

} // namespace book