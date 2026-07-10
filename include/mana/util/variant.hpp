#pragma once

#include <variant>
#include <utility>

namespace mana::util {

// Overloaded - combines multiple lambdas into a single visitor
template<typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// CTAD deduction guide
template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// match - type-safe pattern matching syntax sugar
template<typename Variant, typename... Visitors>
constexpr auto match(Variant&& variant, Visitors&&... visitors) {
    return std::visit(
        overloaded{std::forward<Visitors>(visitors)...},
        std::forward<Variant>(variant)
    );
}

} // namespace mana::util
