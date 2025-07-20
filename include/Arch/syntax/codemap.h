#pragma once

#include <string>
#include <iomanip>
#include <sstream>

namespace Arch::Syntax {

/* Byte Position */
struct BytePos {
  int line;
  int column;
  explicit BytePos(const int line, const int column) : line(line), column(column) {}
  [[nodiscard]] std::string toString() const {
    std::ostringstream oss;
    oss << std::left << std::setfill(' ') << std::setw(2) << line
        << ":" << std::left << std::setfill(' ') << std::setw(2) << column;
    return oss.str();
  }
};

struct Span {
  BytePos begin;
  BytePos end;

  /* Constructor for a zero-length span at a single position */
  explicit Span(const BytePos &pos) : begin(pos), end(BytePos(pos.line, pos.column + 1)) {}

  /* Constructor for a span from begin to end positions */
  explicit Span(const BytePos &begin, const BytePos &end) : begin(begin), end(end) {}

  [[nodiscard]] std::string toString() const { return "[" + begin.toString() + "] - [" + end.toString() + "]"; }
};

}
