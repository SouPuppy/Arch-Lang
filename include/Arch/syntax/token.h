#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include "arch/syntax/span.h"
#include "arch/util/string_utils.h"

namespace Arch::Syntax {

// FIXME - The UTF-8 symbol(s) in comments below may cause column misalignment in monospaced editors.
//         May consider using another method to implement TokenType listing.

enum class TokenKind: uint8_t {
  /* Base Types */
  KEYWORD_REAL,       // ℝ

  /* Numbers */
  INTEGER,            // [0-9][0-9]*
  REAL,               // [0-9][0-9]*.[0-9]*

  /* Meta Instructions */
  INSTR_TYPE,         // #type
  INSTR_INFO,         // #info
  INSTR_EVAL,         // #eval

  /* Keywords */
  IDENTIFIER,         // [a-zA-Z][a-zA-Z]*
  TYPE,               // Type
  UNIVERSE,           // 𝒰
  KEYWORD_DEF,        // def

  /* Symbols */
  COLON,              // :
  HASH,               // #
  PLUS,               // +
  MINUS,              // -
  ASTERISK,           // *
  TIMES,              // ×
  EQUIV,              // ≡
  EQUAL,              // =
  SLASH,              // /
  LEFT_PAREN,         // (
  RIGHT_PAREN,        // )
  LEFT_BRACE,         // {
  RIGHT_BRACE,        // }
  LEFT_BRACKET,       // [
  RIGHT_BRACKET,      // ]
  LESS_THAN,          // <
  GREATER_THAN,       // >
  DOT,                // .

  /* Meta */
  END_OF_FILE,        // EOF
  UNKNOWN             // UNKNOWN Type
};

inline std::string to_string(const TokenKind kind) {
  switch (kind) {
    /* Base Types */
    case TokenKind::KEYWORD_REAL:     return "KEYWORD_REAL";

    /* Numbers */
    case TokenKind::INTEGER:          return "INTEGER";
    case TokenKind::REAL:             return "REAL";

    /* Meta Instructions */
    case TokenKind::INSTR_TYPE:       return "INSTR_TYPE";
    case TokenKind::INSTR_INFO:       return "INSTR_INFO";
    case TokenKind::INSTR_EVAL:       return "INSTR_EVAL";

    /* Keywords */
    case TokenKind::IDENTIFIER:       return "IDENTIFIER";
    case TokenKind::TYPE:             return "TYPE";
    case TokenKind::UNIVERSE:         return "UNIVERSE";
    case TokenKind::KEYWORD_DEF:      return "KEYWORD_DEF";

    /* Symbols */
    case TokenKind::COLON:            return "COLON";
    case TokenKind::HASH:             return "HASH";
    case TokenKind::PLUS:             return "PLUS";
    case TokenKind::MINUS:            return "MINUS";
    case TokenKind::ASTERISK:         return "ASTERISK";
    case TokenKind::TIMES:            return "TIMES";
    case TokenKind::EQUIV:            return "EQUIV";
    case TokenKind::EQUAL:            return "EQUAL";
    case TokenKind::SLASH:            return "SLASH";
    case TokenKind::LEFT_PAREN:       return "LEFT_PAREN";
    case TokenKind::RIGHT_PAREN:      return "RIGHT_PAREN";
    case TokenKind::LEFT_BRACE:       return "LEFT_BRACE";
    case TokenKind::RIGHT_BRACE:      return "RIGHT_BRACE";
    case TokenKind::LEFT_BRACKET:     return "LEFT_BRACKET";
    case TokenKind::RIGHT_BRACKET:    return "RIGHT_BRACKET";
    case TokenKind::LESS_THAN:        return "LESS_THAN";
    case TokenKind::GREATER_THAN:     return "GREATER_THAN";
    case TokenKind::DOT:              return "DOT";

    /* Meta */
    case TokenKind::END_OF_FILE:      return "END_OF_FILE";
    default:                          return "UNKNOWN";
  }
}

struct Token {
  TokenKind type;
  std::u32string lexeme;
  Span span;
  explicit Token(const TokenKind type, std::u32string  lexeme, const Span span) : type(type), lexeme(std::move(lexeme)), span(span) {}
  explicit Token(const TokenKind type, const std::string& lexeme, const Span span) : type(type), lexeme(Arch::Utils::utf8_to_utf32(lexeme)), span(span) {}
  void dump() const {
    const std::string span_str = span.toString();
    const std::string type_str = to_string(type);
    const std::string lexeme_str = Arch::Utils::utf32_to_utf8(lexeme);
    const std::string prefix = span_str + " " + type_str;

    /* add padding before printing lexeme */
    int pad = 40 - static_cast<int>(prefix.size());
    if (pad < 1) pad = 1;

    std::cout << prefix << std::string(pad, ' ') << "'" << lexeme_str << "'" << std::endl;
  }
};

} // namespace Arch
