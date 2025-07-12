#pragma once

#include "arch/libsyntax/token.h"
#include "arch/libcore/context.h"
#include "arch/libcore/term.h"

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

  /* expect to have a specific token type, otherwise record an error */
  [[nodiscard]] std::optional<Token> expect(const TokenKind kind) {
    if (const auto token = peek(); token.has_value() && token->type == kind) {
      pos++;
      return token;
    }
    // TODO - Record an error
    return std::nullopt;
  }

  /* peek at the token at the given offset from the current position without consuming it */
  [[nodiscard]] std::optional<Token> peek(const int offset = 0) const {
    if (pos + offset <= tokens.size()) { return tokens[pos + offset]; }
    return std::nullopt;
  }

  [[nodiscard]] int getLine() const { return tokens[pos].span.begin.line; }

  /* Recursive Descent Parsing */
  Result parse();

  /* expect to parse a term that exact stops at the end of the line */
  std::optional<std::shared_ptr<HoTT::Term>> parse_term();
  std::optional<std::shared_ptr<HoTT::Term>> parse_type();
};

/* parsing source code */
Result parse(const std::string &source, const HoTT::TypeContext *ctx = nullptr);

}
