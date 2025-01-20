#pragma once

#include <filesystem>
#include <optional>

namespace trg {

template <typename SourceFileT = std::filesystem::file_type> class Position {
public: /* Common types */
  using SourceFile = SourceFileT;
  using Index = std::size_t;

public: /* Constructor and destructor */
  Position() = default;
  Position(Index line, Index column, std::optional<SourceFile> f = {})
      : line(line), column(column), sourceFile(std::move(f)) {}

public: /* Manipulation API */
  void advance(Index columns = 1) { column += columns; }
  void newLine() {
    ++line;
    column = 0;
  }

public: /* Getters and Setters */
  [[nodiscard]] Index getLine() const { return line; }
  [[nodiscard]] Index getColumn() const { return column; }
  [[nodiscard]] const std::optional<SourceFile> &getSourceFile() const {
    return sourceFile;
  }

private: /* Private Data */
  Index line{};
  Index column{};
  std::optional<SourceFile> sourceFile{};
};

template <typename SourceFileT = std::filesystem::file_type> class Location {
public: /* Constructor and destructor */
  Location() = default;
  Location(Position<SourceFileT> start, Position<SourceFileT> end)
      : start_(start), end_(end) {}

public: /* Manipulation API */
  void extend(const Location &other) { end_ = other.end_; }
  void advance(Position<>::Index columns = 1) {
    start_.advance(columns);
    end_ = start_;
  }
  void newLine() {
    start_.newLine();
    end_ = start_;
  }

public: /* Getters */
  [[nodiscard]] const Position<> &start() const { return start_; }
  [[nodiscard]] const Position<> &end() const { return end_; }

private: /* Private Data */
  Position<SourceFileT> start_{0, 0, std::nullopt};
  Position<SourceFileT> end_{0, 0, std::nullopt};
};

} // namespace trg