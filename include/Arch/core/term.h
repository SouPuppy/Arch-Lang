#pragma once

#include <memory>
#include <string>
#include <optional>

namespace Arch::HoTT {

struct Term {
  virtual ~Term() = default;
  virtual std::string to_string() = 0;
  virtual std::optional<std::shared_ptr<Term>> evaluate() = 0;
};

struct Application {
  std::shared_ptr<Term> function, argument;
};

}
