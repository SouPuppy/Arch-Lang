#include "arch/libcore/context.h"

namespace Arch::HoTT {

void TypeContext::append(std::u32string variable, std::shared_ptr<Term> type) {
  parameter_space.back()[std::move(variable)] = std::move(type);
}

void TypeContext::append(const TypeContext &other) {
  for (const auto& scope : other.parameter_space) {
    for (const auto& [var, type] : scope) {
      parameter_space.back()[var] = type;
    }
  }
}

void TypeContext::extend(const TypeContext &other) {
  for (const auto& scope : other.parameter_space) {
    parameter_space.push_back(scope);
  }
}

TypeContext merge(const TypeContext *a, const TypeContext *b) {
  if (!a && !b) return TypeContext{};
  if (!a) return *b;
  if (!b) return *a;

  TypeContext result = *a;
  result.append(*b);
  return result;
}

TypeContext extend(const TypeContext *a, const TypeContext *b) {
  if (!a && !b) return TypeContext{};
  if (!a) return *b;
  if (!b) return *a;

  TypeContext result = *a;
  result.extend(*b);
  return result;
}

} // namespace Arch
