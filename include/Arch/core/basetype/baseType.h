#pragma once
#include <memory>
#include <optional>
#include <optional>
#include "arch/core/term.h"

namespace Arch::HoTT {

struct BaseType : Term {
  std::optional<std::shared_ptr<Arch::HoTT::Term>>
  evaluate() override { return std::nullopt; }
};

}