#pragma once

#include <memory>
#include <variant>
#include "arch/libcore/term.h"

namespace Arch::HoTT {

struct Type: std::variant<Arch::HoTT::Term> {
  explicit Type(Arch::HoTT::Term term): std::variant<Arch::HoTT::Term>(term) {}
};

} // namespace Arch
