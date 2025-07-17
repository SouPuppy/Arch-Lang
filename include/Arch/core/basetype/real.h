#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "arch/core/basetype/baseType.h"

#include "arch/util/string_utils.h"

namespace Arch::HoTT {


struct RealType final : BaseType {
  double value;
  explicit RealType(double value): value(value) {}
  std::string to_string() override {
    return std::to_string(value) + " ℝ";
  }
};

struct RealType_add final : Term {
  std::shared_ptr<RealType> a, b;
  explicit RealType_add(std::shared_ptr<RealType> a, std::shared_ptr<RealType> b): a(std::move(a)), b(std::move(b)) {}

  std::string to_string() override {
    return "+ " + a->to_string() + " " + b->to_string();
  }

  std::optional<std::shared_ptr<Term>> evaluate() override {
    /* type check */
    if (!a || !b) return std::nullopt;
    auto realA = std::dynamic_pointer_cast<RealType>(a);
    auto realB = std::dynamic_pointer_cast<RealType>(b);

    if (!realA || !realB) {
      std::cerr << "Usage: + ℝ ℝ" << std::endl;
      return std::nullopt;
    }

    /* evaluation */
    double result = a->value + b->value;
    return std::make_shared<RealType>(RealType{result});
  }
};


}
