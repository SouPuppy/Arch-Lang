#pragma once

#include "arch/syntax/token.h"
#include "arch/core/context.h"
#include "arch/core/term.h"
#include "arch/syntax/ast.h"

#include <cstddef>
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
  void pop() {
    if (!stack.empty()) {
      pos = stack.back();
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

  /* Const */
  Ptr<Const> parse_Const();
  Ptr<ConstString> parse_ConstString();
  Ptr<ConstReal> parse_ConstReal();

  /* Term */
  Ptr<Term> parse_Term();
  Ptr<ApplyTerm> parse_ApplyTerm();
  Ptr<AnnotatedTerm> parse_AnnotatedTerm();
  Ptr<BinaryPrefixTerm> parse_BinaryPrefixTerm();
  Ptr<BinaryInfixTerm> parse_BinaryInfixTerm();
  Ptr<TermString> parse_TermString();
  Ptr<TermReal> parse_TermReal();
  Ptr<BaseTerm> parse_BaseTerm();
  Ptr<ConstTerm> parse_ConstTerm();

  /* Type */
  Ptr<Type> parse_Type();

  /* Decl */
  Ptr<Decl> parse_Decl();
  Ptr<DefDecl> parse_DefDecl();
  Ptr<AssignDecl> parse_AssignDecl();
  Ptr<FullDecl> parse_FullDecl();

  /* Instruction */
  Ptr<Instruction> parse_Instruction();

  /* Top Level Item */
  Ptr<TopLevelItem> parse_TopLevelItem();

  /* Identifier */
  Ptr<Identifier> parse_Identifier();
};

/* parsing source code */
Result parse(const std::string &source, const HoTT::TypeContext *ctx = nullptr);

}
