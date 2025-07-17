#include "arch/syntax/parser.h"

#include <memory>
#include <optional>

#include "arch/syntax/token.h"
#include "arch/syntax/lexer.h"
#include "arch/core/context.h"
#include "arch/core/term.h"
#include "arch/util/string_utils.h"

#include "arch/core/basetype/real.h"

namespace Arch::Syntax {

// TODO - Complete later
Result Parser::parse_all() {
  while (!isAtEnd()) {
    /* #info */
    if (match(TokenKind::INSTR_INFO)) {
      ctx->dump();
      continue;
    }

    /* #eval */
    if (match(TokenKind::INSTR_EVAL)) {
      if(auto term = parse_term()) {
        if (auto result = term.value()->evaluate()) {
          std::cout << "Evaluation Result: " << result.value()->to_string() << std::endl;
        } else {
          std::cerr << "Evaluation failed." << std::endl;
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

std::optional<std::shared_ptr<Arch::HoTT::Term>> Parser::parse_term() {
  stash();

  /* + a:ℝ b:ℝ */
  if (match(TokenKind::PLUS)) {
    auto a = parse_term();
    auto b = parse_term();

    if (!a || !b) {
      revert();
      return std::nullopt;
    }

    auto realA = std::dynamic_pointer_cast<Arch::HoTT::RealType>(a.value());
    auto realB = std::dynamic_pointer_cast<Arch::HoTT::RealType>(b.value());

    if (!realA || !realB) {
      revert();
      return std::nullopt;
    }

    return std::make_shared<Arch::HoTT::RealType_add>(realA, realB);
  }
  
  /* ℝ */
  if (match(TokenKind::INTEGER)) {
    double value = Utils::u32_to_double(previous().lexeme);
    return std::make_shared<Arch::HoTT::RealType>(value);
  }

  revert();
  return std::nullopt;
}


/* parsing source code */
Result parse(const std::string &source, const HoTT::TypeContext *ctx) {
  const std::vector<Token> tokens = tokenize(source);

  /* create an extended context */
  const HoTT::TypeContext local_ctx;
  HoTT::TypeContext extended_ctx = Arch::HoTT::merge(ctx, &local_ctx);

  Parser parser(tokens, &extended_ctx);
  const auto result = Parser(tokens, &extended_ctx).parse_all();
  std::cout << "Parse State: " << (result.ok() ? "OK" : "ERROR") << std::endl;
  return result;
}

}
