#pragma once

namespace Arch::HoTT {

struct Term {
  virtual ~Term() = default;
  virtual std::string to_string() = 0;
};

}
