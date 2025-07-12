#pragma once
#include <string>
#include "arch/libcore/term.h"
#include "arch/libcore/value.h"
#include "arch/util/string_utils.h"

namespace Arch::HoTT::BaseType {

struct RealType final : Term {

  std::string to_string() override { return "ℝ"; }
};

struct Real final : Term {
  double value;
  explicit Real(const double val): value(val) {};
  explicit Real(const std::u32string &val): value(Arch::Utils::u32_to_double(val)) {};
  std::string to_string() override { return std::to_string(value); }
};

struct RealConstant final : Value {
  
};

}
