#pragma once

#include "arch/syntax/token.h"
#include "arch/core/context.h"
#include "arch/core/term.h"

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace Arch::Syntax {

enum class ExitCode {
  Success,
  ERROR,
  EXIT,
};

struct Result {
  ExitCode exit_code;

  explicit Result(const ExitCode exit_code)
    : exit_code(exit_code) {
  }

  [[nodiscard]] bool ok() const { return exit_code == ExitCode::Success; }
  [[nodiscard]] bool error() const { return exit_code == ExitCode::ERROR; }
  [[nodiscard]] bool exit() const { return exit_code == ExitCode::EXIT; }
};

struct Parser {
  const std::vector<Token>& tokens;
  HoTT::TypeContext *ctx;
  int pos = 0;
  explicit Parser(const std::vector<Token>& tokens, HoTT::TypeContext *ctx)
    : tokens(tokens), ctx(ctx) {}

  [[nodiscard]] bool isAtEnd() const { return pos >= tokens.size(); }

  /* try to receive (look for) a token of the specified kind */
  [[nodiscard]] std::optional<Token> match(const TokenKind kind) {
    if (const auto token = peek(); token.has_value() && token->type == kind) {
      pos++;
      return token;
    }
    return std::nullopt;
  }

  /* consume a token */
  void consume() {
    pos++;
  }

  std::vector<int> stack;
  void stash() { stack.push_back(pos); }
  void revert() {
    if (!stack.empty()) {
      pos = stack.back();
      stack.pop_back();
    }
  }

  [[nodiscard]] Token previous() const {
    return tokens[pos - 1];
  }

  /* peek at the token at the given offset from the current position without consuming it */
  [[nodiscard]] std::optional<Token> peek(const int offset = 0) const {
    if (pos + offset <= tokens.size()) { return tokens[pos + offset]; }
    return std::nullopt;
  }

  [[nodiscard]] int getLine() const { return tokens[pos].span.begin.line; }

  /* Recursive Descent Parsing */
  Result parse_all();

  /* expect to parse a term that exact stops at the end of the line */
  std::optional<std::shared_ptr<Arch::HoTT::Term>> parse_term();
};

/* parsing source code */
Result parse(const std::string &source, const HoTT::TypeContext *ctx = nullptr);

}
