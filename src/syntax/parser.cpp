#include "arch/syntax/parser.h"
#include "arch/syntax/token.h"
#include "arch/syntax/lexer.h"
#include "arch/core/context.h"
#include "arch/syntax/ast.h"

namespace Arch::Syntax {

// TODO - Complete later
Result Parser::parse_all() {
  while (!isAtEnd()) {
    /* #info */
    if (match(TokenKind::INSTR_INFO)) {
      ctx->dump();
      continue;
    }

    /* parse Instructions */
    
    /* parse Decl */
    if (const auto ptr = parse_Decl(); ptr) {
      std::cout << "!!!";
    }
    else {
      std::cout << "QWQ";
    }

    /* fallback */
    {
      consume();
    }
  }
  return Result(ExitCode::Success);
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
