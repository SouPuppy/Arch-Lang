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
    if (const auto ptr = parse_Term(); ptr) {
      // std::cout << "Parsed\n";
      continue;
    }

    /* fallba1ck */
    {
      consume();
    }
  }
  return Result(ExitCode::Success);
}

Ptr<Const> Parser::parse_Const() {
  if (const auto real = parse_ConstReal(); real) {
    const auto node = ConstNode{ *real };
    return std::make_unique<Const>(Const{ node });
  }
  if (const auto str = parse_ConstString(); str) {
    const auto node = ConstNode{ *str };
    return std::make_unique<Const>(Const{ node });
  }
  return nullptr;
}


Ptr<ConstString> Parser::parse_ConstString() {
  stash();
  if (const auto token = match(TokenKind::STRING)) {
    pop();
// std::cout << "Parsed Str: " << Utils::utf32_to_utf8(token->lexeme) << std::endl;
    return std::make_unique<ConstString>(ConstString{.value = Utils::utf32_to_utf8(token->lexeme)});
  }
  revert();
  return nullptr;
}
Ptr<ConstReal> Parser::parse_ConstReal() {
  if (const auto token = match(TokenKind::INTEGER)) {
// std::cout << "Parsed R: " << Utils::u32_to_double(token->lexeme) << std::endl;
    return std::make_unique<ConstReal>(ConstReal{.value = Utils::u32_to_double(token->lexeme)});
  }
  if (const auto token = match(TokenKind::REAL)) {
// std::cout << "Parsed R: " << Utils::u32_to_double(token->lexeme) << std::endl;
    return std::make_unique<ConstReal>(ConstReal{.value = Utils::u32_to_double(token->lexeme)});
  }
  return nullptr;
}

Ptr<Term> Parser::parse_Term() {
  stash();
  /* const Term */
  if (auto ptr = parse_Const(); ptr) {
    const auto const_term = std::make_unique<ConstTerm>();
    const_term->value = std::move(ptr);

    auto term = std::make_unique<Term>();
    term->node = std::move(*const_term);

    pop();
    return term;
  }
  if (auto ptr = parse_BaseTerm(); ptr) {
    const auto base_term = std::make_unique<BaseTerm>();

    auto term = std::make_unique<Term>();
    term->node = std::move(*base_term);
    pop();
    return term;
  }

  revert();
  return nullptr;
}

Ptr<ApplyTerm> Parser::parse_ApplyTerm() { return nullptr; }
Ptr<AnnotatedTerm> Parser::parse_AnnotatedTerm() { return nullptr; }
Ptr<BinaryPrefixTerm> Parser::parse_BinaryPrefixTerm() { return nullptr; }
Ptr<BinaryInfixTerm> Parser::parse_BinaryInfixTerm() { return nullptr; }
Ptr<TermString> Parser::parse_TermString() { return nullptr; }
Ptr<TermReal> Parser::parse_TermReal() { return nullptr; }
Ptr<ConstTerm> Parser::parse_ConstTerm() { return nullptr; }
Ptr<BaseTerm> Parser::parse_BaseTerm() {
  if (const auto token = match(TokenKind::KEYWORD_REAL)) {
    std::cout << "Parsed R\n";
    return std::make_unique<BaseTerm>(BaseTerm{.kind = BaseTerm::Kind::Real});
  }
  // if (const auto token = match(TokenKind::STRING)) {
  //   return std::make_unique<BaseTerm>(BaseTerm{.kind = BaseTerm::Kind::String});
  // }
  return nullptr;
}

Ptr<Type> Parser::parse_Type() { return nullptr; }

Ptr<Decl> Parser::parse_Decl() { return nullptr; }
Ptr<DefDecl> Parser::parse_DefDecl() { return nullptr; }
Ptr<AssignDecl> Parser::parse_AssignDecl() { return nullptr; }
Ptr<FullDecl> Parser::parse_FullDecl() { return nullptr; }

Ptr<Instruction> Parser::parse_Instruction() { return nullptr; }
Ptr<TopLevelItem> Parser::parse_TopLevelItem() { return nullptr; }

Ptr<Identifier> Parser::parse_Identifier() { return nullptr; }

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
