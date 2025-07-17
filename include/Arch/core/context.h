#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "arch/core/term.h"
#include "arch/util/string_utils.h"

namespace Arch::HoTT {

/* HoTT-online Pg. 20
 *
 * Judgments may depend on assumptions of the form x: A, where x is a variable and A is a
 * type.
 *
 * The collection of all such assumptions is called the context; from a
 * topological point of view, it may be thought of as a "parameter space". In fact, technically the
 * context must be an ordered list of assumptions, since later assumptions may depend on previous
 * ones: the assumption x: A can only be made after the assumptions of any variables appearing
 * in the type A.
 */

struct TypeContext {
  std::vector<std::unordered_map<std::u32string, std::shared_ptr<Term>>> parameter_space{{}};

  TypeContext() = default;

  /* append an assumption to the current context */
  void append(std::u32string variable, std::shared_ptr<Term> type);

  /* merge another context to the current context */
  void append(const TypeContext &other);

  /* extend current context with another context */
  void extend(const TypeContext &other);

  /* dump */
  void dump() const {
    std::cout << " --- context ---\n";
    for (const auto& layer: parameter_space) {
      for (const auto& [identifier, type]: layer) {
        std::cout << "  " << Utils::utf32_to_utf8(identifier) << ": " << type->to_string() << std::endl;
      }
    }
    std::cout << " ---------------\n";
  }
};

/* return a merged context of two*/
TypeContext merge(const TypeContext *a, const TypeContext *b);

/* return an extended context of two */
TypeContext extend(const TypeContext *a, const TypeContext *b);

} // namespace Arch
