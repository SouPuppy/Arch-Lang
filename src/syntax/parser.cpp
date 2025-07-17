#include "arch/syntax/parser.h"

#include <memory>

#include "arch/syntax/token.h"
#include "arch/syntax/lexer.h"
#include "arch/core/context.h"
#include "arch/core/term.h"
#include "arch/core/basetype/real.h"

namespace Arch::Syntax {

// TODO - Complete later
Result Parser::parse() {
  while (!isAtEnd()) {

    /* #type <term> */
    if (match(TokenKind::INSTR_TYPE)) {
      const auto term = parse_term();

      if (const auto real_ptr = std::dynamic_pointer_cast<Arch::HoTT::BaseType::Real>(term.value())) {
        std::cout << "\nREAL: " << real_ptr->value << std::endl;
      } else {
        std::cout << "Parsed term is not a Real type" << std::endl;
      }
      continue;
    }

    /* #info */
    if (match(TokenKind::INSTR_INFO)) {
      ctx->dump();
      continue;
    }

    /* def <identifier> : <term> */
    if (match(TokenKind::KEYWORD_DEF)) {
      if (auto identifier = expect(TokenKind::IDENTIFIER)) {
        if (expect(TokenKind::COLON)) {
          if (auto type = parse_type()) {
            ctx->append(identifier.value().lexeme, type.value());
          } else {
          }
        }
      }
      continue;
    }

    /* fallback */
    {
      consume();
    }
  }
  return Result(ExitCode::Success);
}

std::optional<std::shared_ptr<HoTT::Term>> parse_term() {
  /* Real Number Builtins */
  {
    /* ℝ () ℝ */

    /* (ℝ) */

  }

  return std::nullopt;
}

std::optional<std::shared_ptr<HoTT::Term>> Parser::parse_term() {
  if (std::optional<Token> token = match(TokenKind::REAL); token.has_value()) {
    return std::make_shared<HoTT::BaseType::Real>(token.value().lexeme);
  }
  return std::nullopt;
}

std::optional<std::shared_ptr<HoTT::Term>> Parser::parse_type() {
  if (auto type = match(TokenKind::KEYWORD_REAL)) {
    return std::make_shared<HoTT::BaseType::RealType>();
  }
  return std::nullopt;
}

/* parsing source code */
Result parse(const std::string &source, const HoTT::TypeContext *ctx) {
  const std::vector<Token> tokens = tokenize(source);

  /* create an extended context */
  const HoTT::TypeContext local_ctx;
  HoTT::TypeContext extended_ctx = Arch::HoTT::merge(ctx, &local_ctx);

  Parser parser(tokens, &extended_ctx);
  const auto result = Parser(tokens, &extended_ctx).parse();
  std::cout << "Parse State: " << (result.ok() ? "OK" : "ERROR") << std::endl;
  return result;
}

}
