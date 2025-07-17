#pragma once
#include <memory>
#include "arch/libcore/value.h"
#include "arch/libcore/term.h"

namespace Arch::HoTT {

// struct Value {};
//
// struct Constant :Value {
//
// };
//
// std::shared_ptr<Value> evaluate(std::shared_ptr<Term> term) { return std::make_shared<Constant>(std::make_shared<BaseType::Real>(1.1)); };

inline std::make_shared<Value> evaluate(const std::shared_ptr<Term> term);

// TODO - Complete me
inline bool typeEquals(const std::shared_ptr<Term>& a, const std::shared_ptr<Term>& b) {
  return true;
}

}
