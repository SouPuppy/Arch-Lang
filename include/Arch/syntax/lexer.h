#pragma once

#include "arch/util/string_utils.h"

namespace Arch::Syntax::Lexer {

/* lexer strategies implement with DFA */
struct LexerStrategy {
  using State = int;
  int matched_len = 0;
  /* return the token type this lexer returns */
  virtual TokenKind resolveTokenKind() = 0;
  /* reset the automata to initial state */
  virtual void reset() = 0;
  /* perform a state transition based on the input symbol */
  virtual void transit(char32_t input) = 0;
  /* returns true if the automaton is currently in an accepting (final) state */
  virtual bool accepted() = 0;
  virtual bool rejected() = 0;
  virtual ~LexerStrategy() = default;
};

}

namespace Arch::Syntax {

struct Scanner {
  std::u32string source;
  explicit Scanner(const std::string& source_utf8) : source(Arch::Utils::utf8_to_utf32(source_utf8)) {}
  bool _newLine = true;
  [[nodiscard]] bool newLine() const { return _newLine == true; }
  int pos = 0;
  int line = 1, column = 1;

  [[nodiscard]] bool isAtEnd() const;
  [[nodiscard]] char32_t peek(int offset = 0) const;
  void skipWhitespace();

  /* advances the current position by one character */
  char32_t advance();
  [[nodiscard]] BytePos getLineInfo() const;
  [[nodiscard]] Token nextToken();
  [[nodiscard]] int getPos() const { return pos; }
  [[nodiscard]] std::u32string substr(int start_pos, int length) const {
    if (start_pos < 0) start_pos = 0;
    if (start_pos > source.size()) return U"";
    if (start_pos + length > source.size()) length = static_cast<int>(source.size()) - start_pos;
    return source.substr(start_pos, length);
  }

  /* consume len chars */
  void consume(int len);

  [[nodiscard]] bool match(const std::u32string_view text) {
    for (int i = 0; i < text.length(); i++) {
      if (text[i] != peek(i)) return false;
    }
    // consume the tokens if matchs
    consume(static_cast<int>(text.length()));
    return true;
  }
};

std::vector<Token> tokenize(const std::string& source);

}