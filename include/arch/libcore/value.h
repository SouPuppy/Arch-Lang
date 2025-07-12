#pragma once
#include <string>

namespace Arch::HoTT {

struct Value {
  virtual ~Value() = default;
  virtual std::string to_string() = 0;
};

}
