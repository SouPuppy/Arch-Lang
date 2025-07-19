#pragma once

#include <memory>
#include <string>
#include <variant>

namespace Arch::Syntax {

template<typename T> using Ptr = std::unique_ptr<T>;

struct Identifier { std::string name; };
struct Type;
struct Term;

/*** Instruction ***/
struct Instruction {
  enum class Kind { Evaluate, TypeInfer };
  Kind kind;
  Ptr<Term> term;
};

/*** Declarations (Decl) ***/
struct DefDecl { Ptr<Identifier> id; Ptr<Type> type; };
struct AssignDecl { Ptr<Identifier> id; Ptr<Term> term; };
struct FullDecl { Ptr<Identifier> id; Ptr<Type> type; Ptr<Term> term; };

using DeclNode = std::variant<DefDecl, AssignDecl, FullDecl>;
struct Decl {
  DeclNode node;
};

/*** Const Literals ***/
struct ConstString { std::string value; };
struct ConstReal { double value; };

using ConstNode = std::variant<ConstString, ConstReal>;
struct Const {
  ConstNode node;
};

/*** Terms ***/
struct ApplyTerm { Ptr<Term> lhs; Ptr<Term> rhs; };
struct AnnotatedTerm { Ptr<Term> term; Ptr<Type> type; };
struct BinaryPrefixTerm {
  enum class Op { CircPlus, CircTimes } op;
  Ptr<Term> lhs, rhs;
};
struct BinaryInfixTerm {
  enum class Op { Plus, Times } op;
  Ptr<Term> lhs, rhs;
};
struct TermString {};
struct TermReal {};
struct ConstTerm { Ptr<Const> value; };

using TermNode = std::variant<
  ApplyTerm,
  AnnotatedTerm,
  BinaryPrefixTerm,
  BinaryInfixTerm,
  TermString,
  TermReal,
  ConstTerm
>;
struct Term {
  TermNode node;
};

/*** Type ***/
using TypeNode = std::variant<Term>;
struct Type {
  TypeNode node;
};

/*** Top Level Items ***/
using TopLevelItem = std::variant<
  Ptr<Decl>,
  Ptr<Instruction>
>;

}
