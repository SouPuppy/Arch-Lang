#include "arch/syntax/parser.h"
#include "arch/syntax/token.h"
#include "arch/syntax/lexer.h"
#include "arch/util/char_utils.h"

#include <memory>
#include <optional>

/* different lexers */
namespace  Arch::Syntax::Lexer {

/* lexer for identifier */
struct IdentifierLexer final : LexerStrategy {
  enum class State {
    START,
    IDENTIFIER_BODY,
    ACCEPT,
    REJECT,
  } state = State::START;

  TokenKind resolveTokenKind() override { return TokenKind::IDENTIFIER; }
  void reset() override { matched_len = 0; state = State::START; }
  void transit(const char32_t input) override {
    switch (state) {
      case State::START:
        if (is_Alpha(input)) { matched_len++; state = State::IDENTIFIER_BODY; }
        else { state = State::REJECT; }
        break;
      case State::IDENTIFIER_BODY:
        if (is_Alpha(input)) { matched_len++; state = State::IDENTIFIER_BODY; }
        else { state = State::ACCEPT; }
        break;
      default: state = State::REJECT; break;
    }
  }
  bool accepted() override { return state == State::ACCEPT; }
  bool rejected() override { return state == State::REJECT; }
};

/* lexer for Integer */
struct IntegerLexer final : LexerStrategy {
  enum class State {
    START,
    INTEGER_BODY,
    ACCEPT,
    REJECT,
  } state = State::START;

  TokenKind resolveTokenKind() override { return TokenKind::INTEGER; }
  void reset() override { matched_len = 0; state = State::START; }
  void transit(const char32_t input) override {
    switch (state) {
      case State::START:
        if (is_Number(input)) { matched_len++; state = State::INTEGER_BODY; }
        else { state = State::REJECT; }
        break;
      case State::INTEGER_BODY:
        if (is_Number(input)) { matched_len++; state = State::INTEGER_BODY; }
        else { state = State::ACCEPT; }
        break;
      default: state = State::REJECT; break;
    }
  }
  bool accepted() override { return state == State::ACCEPT; }
  bool rejected() override { return state == State::REJECT; }
};

/* lexer for Real */
struct RealNumberLexer final : LexerStrategy {
  enum class State {
    START,
    INTEGER_PART,
    DOT,
    FRACTION_PART,
    ACCEPT,
    REJECT,
  } state = State::START;

  TokenKind resolveTokenKind() override { return TokenKind::REAL; }
  void reset() override { matched_len = 0; state = State::START; }

  void transit(const char32_t input) override {
    switch (state) {
      case State::START:
        if (is_Number(input)) { matched_len++; state = State::INTEGER_PART; }
        else { state = State::REJECT; }
        break;
      case State::INTEGER_PART:
        if (is_Number(input)) { matched_len++;}
        else if (input == U'.') { matched_len++; state = State::DOT; }
        else { state = State::REJECT;}
        break;
      case State::DOT:
        if (is_Number(input)) { matched_len++; state = State::FRACTION_PART; }
        else { state = State::REJECT;}
        break;
      case State::FRACTION_PART:
        if (is_Number(input)) { matched_len++; }
        else { state = State::ACCEPT; }
        break;
      default: state = State::REJECT; break;
    }
  }

  bool accepted() override { return state == State::ACCEPT; }
  bool rejected() override { return state == State::REJECT; }
};

}

namespace Arch::Syntax {

/* DFA (Deterministic Finite Automaton) to match tokens */
struct MatchList {
  std::vector<std::unique_ptr<Lexer::LexerStrategy>> strategies;
  MatchList() {
    strategies.push_back(std::make_unique<Lexer::IdentifierLexer>());
    strategies.push_back(std::make_unique<Lexer::RealNumberLexer>()); // Real number should go before integer
    strategies.push_back(std::make_unique<Lexer::IntegerLexer>());
  }

  std::optional<Token> match(Scanner &scanner) const {
    const BytePos lineinfo_start = scanner.getLineInfo();
    const int startPos = scanner.getPos();
    for (auto &strategy : strategies) {
      strategy->reset();
      while (!scanner.isAtEnd()) {
        strategy->transit(scanner.peek(strategy->matched_len));
        if (strategy->accepted()) {
          const std::u32string lexeme = scanner.substr(startPos, strategy->matched_len);
          scanner.consume(strategy->matched_len);
          return Token(strategy->resolveTokenKind(), lexeme, Span(lineinfo_start, scanner.getLineInfo()));
        }
        if (strategy->rejected()) break;
      }
    }
    return std::nullopt;
  }
};

bool Scanner::isAtEnd() const {
  return pos == source.size();
}

char32_t Scanner::peek(const int offset) const {
  if (pos + offset >= source.size()) return U'\0';
  return source[pos + offset];
}

void Scanner::skipWhitespace() {
  while (!isAtEnd()) {
    if (const char32_t c = peek(); c == U' ' || c == U'\t' || c == U'\r' || c == U'\n') { advance(); } else break;
  }
}

char32_t Scanner::advance() {
  if (pos >= source.size()) return U'\0';
  const char32_t c = source[pos++];

  /* update lineinfo after advance */
  if (c == U'\n') { line++; column = 1; _newLine = true; }
  else { column++; _newLine = false; }

  return c;
}

BytePos Scanner::getLineInfo() const {
  return BytePos(line, column);
}

Token Scanner::nextToken() {
  skipWhitespace();
  const BytePos lineinfo_start = getLineInfo();

  /* scanner meets end */
  if (isAtEnd()) return Token(TokenKind::END_OF_FILE, "\\0", Span(lineinfo_start));

  /* Meta Instruction */
  if (newLine() == true) {
    // Meta Instructions should be the first token of a new line
    if (match(U"#type")) return Token(TokenKind::INSTR_TYPE              , U"#type", Span(lineinfo_start, getLineInfo()));
    if (match(U"#info")) return Token(TokenKind::INSTR_INFO              , U"#info", Span(lineinfo_start, getLineInfo()));
    if (match(U"#eval")) return Token(TokenKind::INSTR_EVAL              , U"#eval", Span(lineinfo_start, getLineInfo()));
  }  

  /* match keywords */
  if (match(U"ℝ"))          return Token(TokenKind::KEYWORD_REAL        , U"ℝ", Span(lineinfo_start, getLineInfo()));
  if (match(U"def"))        return Token(TokenKind::KEYWORD_DEF         , U"def", Span(lineinfo_start, getLineInfo()));
  if (match(U"using"))      return Token(TokenKind::KEYWORD_USING       , U"using", Span(lineinfo_start, getLineInfo()));
  if (match(U"import"))     return Token(TokenKind::KEYWORD_IMPORT      , U"import", Span(lineinfo_start, getLineInfo()));
  if (match(U"namespace"))  return Token(TokenKind::KEYWORD_NAMESPACE   , U"namespace", Span(lineinfo_start, getLineInfo()));
  if (match(U"export"))     return Token(TokenKind::KEYWORD_EXPORT      , U"export", Span(lineinfo_start, getLineInfo()));
  if (match(U"(+)"))        return Token(TokenKind::KEYWORD_PLUS_TERM   , U"(+)", Span(lineinfo_start, getLineInfo()));
  if (match(U"(×)"))        return Token(TokenKind::KEYWORD_TIMES_TERM  , U"(×)", Span(lineinfo_start, getLineInfo()));
  
  /* single utf8 symbol */
  switch (const char32_t c = peek()) {
    /* Keywords */
    case U'𝒰': advance(); return Token(TokenKind::UNIVERSE              , U"𝒰", Span(lineinfo_start));

    /* Symbols */
    case U':':  advance(); return Token(TokenKind::COLON                 , U":",  Span(lineinfo_start));
    case U'#':  advance(); return Token(TokenKind::HASH                  , U"#",  Span(lineinfo_start));
    case U'+':  advance(); return Token(TokenKind::PLUS                  , U"+",  Span(lineinfo_start));
    case U'-':  advance(); return Token(TokenKind::MINUS                 , U"-",  Span(lineinfo_start));
    case U'*':  advance(); return Token(TokenKind::ASTERISK              , U"*",  Span(lineinfo_start));
    case U'×':  advance(); return Token(TokenKind::TIMES                 , U"×",  Span(lineinfo_start));
    case U'≡':  advance(); return Token(TokenKind::EQUIV                 , U"≡",  Span(lineinfo_start));
    case U'=':  advance(); return Token(TokenKind::EQUAL                 , U"=",  Span(lineinfo_start));
    case U'/':  advance(); return Token(TokenKind::SLASH                 , U"/",  Span(lineinfo_start));
    case U'(':  advance(); return Token(TokenKind::LEFT_PAREN            , U"(",  Span(lineinfo_start));
    case U')':  advance(); return Token(TokenKind::RIGHT_PAREN           , U")",  Span(lineinfo_start));
    case U'{':  advance(); return Token(TokenKind::LEFT_BRACE            , U"{",  Span(lineinfo_start));
    case U'}':  advance(); return Token(TokenKind::RIGHT_BRACE           , U"}",  Span(lineinfo_start));
    case U'[':  advance(); return Token(TokenKind::LEFT_BRACKET          , U"[",  Span(lineinfo_start));
    case U']':  advance(); return Token(TokenKind::RIGHT_BRACKET         , U"]",  Span(lineinfo_start));
    case U'<':  advance(); return Token(TokenKind::LESS_THAN             , U"<",  Span(lineinfo_start));
    case U'>':  advance(); return Token(TokenKind::GREATER_THAN          , U">",  Span(lineinfo_start));
    case U'.':  advance(); return Token(TokenKind::DOT                   , U".",  Span(lineinfo_start));
    case U'\"': advance(); return Token(TokenKind::DOUBLE_QUOTE          , U"\"", Span(lineinfo_start));
    case U'@':  advance(); return Token(TokenKind::AT                    , U"@",  Span(lineinfo_start));
    case U'_':  advance(); return Token(TokenKind::UNDERSCORE            , U"_",  Span(lineinfo_start));
    case U'|':  advance(); return Token(TokenKind::BAR                   , U"|",  Span(lineinfo_start));

    default: ;
  }

  /* match with DFAs (Deterministic Finite Automaton) */
  if (auto result = MatchList().match(*this); result.has_value()) {
    return result.value();
  }

  /* fallback to unknown */
  const std::u32string lexeme(1, advance());
  return Token(TokenKind::UNKNOWN, lexeme, Span(getLineInfo()));
}

void Scanner::consume(const int len) {
  for (int i = 0; i < len; i++) {
    advance();
  }
}

std::vector<Token> tokenize(const std::string& source) {
  Scanner scanner(source);
  std::vector<Token> tokens;

  while (true) {
    Token token = scanner.nextToken();
    tokens.push_back(token);
    if (token.type == TokenKind::END_OF_FILE) { break; }
  }

  // TEMP - Debug only
  for (int i = 0; i < tokens.size(); i++) {
    tokens[i].dump();
  }

  return tokens;
}

}
