// #pragma once

// #include "arch/core/term.h"
// #include "arch/core/basetype/real.h"
// #include <memory>


// namespace Arch::HoTT {

// inline std::optional<std::shared_ptr<Term>> resolve_type(std::shared_ptr<Term> term) {
//   /* ℝ */
//   if (auto real = std::dynamic_pointer_cast<RealType>(term)) {
//     return std::make_shared<RealType>();
//   }

//   std::cerr << "Unknown term in type_of" << std::endl;
//   return std::nullopt;
// }

// /* NbE */
// inline bool type_equal_to(std::shared_ptr<Term> a, std::shared_ptr<Term> b) {
//   if (!a) {
//     return false;
//   }

//   if (const auto evaluated_a = a->evaluate()) {
//     return resolve_type(evaluated_a.value()) == resolve_type(b);
//   }
//   return false;
// }

// // 深度比较两个类型是否等价（支持 NbE）
// inline bool type_equals(std::shared_ptr<Term> a, std::shared_ptr<Term> b) {
//   if (!a || !b) return false;

//   auto norm_a = a->evaluate();
//   auto norm_b = b->evaluate();

//   if (!norm_a || !norm_b) return false;

//   // 先 resolve_type，然后再次 evaluate（即 NbE）
//   auto type_a = resolve_type(norm_a.value());
//   auto type_b = resolve_type(norm_b.value());

//   if (!type_a || !type_b) return false;

//   auto norm_type_a = type_a->evaluate();
//   auto norm_type_b = type_b->evaluate();

//   if (!norm_type_a || !norm_type_b) return false;

//   // 最后使用结构比对
//   return norm_type_a.value()->to_string() == norm_type_b.value()->to_string();
// }



// }