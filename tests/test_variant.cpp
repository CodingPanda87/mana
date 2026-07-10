#include <mana/util/variant.hpp>
#include <cassert>
#include <string>
#include <variant>

void test_variant_overloaded() {
    std::variant<int, double, std::string> v1 = 42;
    std::variant<int, double, std::string> v2 = 3.14;
    std::variant<int, double, std::string> v3 = "hello";

    // All lambdas must return the same type for std::visit
    auto visitor = mana::util::overloaded{
        [](int i) -> int { return i * 2; },
        [](double d) -> int { return static_cast<int>((d + 1.0) * 100); },
        [](const std::string& s) -> int { return static_cast<int>(s.size()); }
    };

    assert(std::visit(visitor, v1) == 84);
    assert(std::visit(visitor, v2) == 414);
    assert(std::visit(visitor, v3) == 5);
}

void test_variant_match() {
    std::variant<int, double, std::string> v = "hello world";

    // All lambdas must return the same type for match (which uses std::visit)
    int result = mana::util::match(v,
        [](int i) -> int { return i; },
        [](double d) -> int { return static_cast<int>(d); },
        [](const std::string& s) -> int { return static_cast<int>(s.size()); }
    );

    assert(result == 11);
}
