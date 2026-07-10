#include <mana/util/optional.hpp>
#include <cassert>
#include <string>
#include <optional>

void test_optional_and_then() {
    auto opt = std::optional<int>(42);
    auto result = mana::util::and_then(std::move(opt), [](int v) -> std::optional<std::string> {
        return std::to_string(v);
    });
    assert(result.has_value());
    assert(result.value() == "42");

    auto empty = std::optional<int>(std::nullopt);
    auto result2 = mana::util::and_then(std::move(empty), [](int v) -> std::optional<std::string> {
        return std::to_string(v);
    });
    assert(!result2.has_value());
}

void test_optional_transform() {
    auto opt = std::optional<int>(10);
    auto result = mana::util::transform(std::move(opt), [](int v) { return v * 2.0; });
    assert(result.has_value());
    assert(result.value() == 20.0);

    auto empty = std::optional<int>(std::nullopt);
    auto result2 = mana::util::transform(std::move(empty), [](int v) { return v * 2.0; });
    assert(!result2.has_value());
}

void test_optional_or_else() {
    auto empty = std::optional<int>(std::nullopt);
    auto result = mana::util::or_else(std::move(empty), []() -> std::optional<int> {
        return 99;
    });
    assert(result.has_value());
    assert(result.value() == 99);

    auto opt = std::optional<int>(42);
    auto result2 = mana::util::or_else(std::move(opt), []() -> std::optional<int> {
        return 99;
    });
    assert(result2.has_value());
    assert(result2.value() == 42);
}

void test_optional_value_or_else() {
    auto empty = std::optional<int>(std::nullopt);
    auto result = mana::util::value_or_else(std::move(empty), []() { return 99; });
    assert(result == 99);

    auto opt = std::optional<int>(42);
    auto result2 = mana::util::value_or_else(std::move(opt), []() { return 99; });
    assert(result2 == 42);
}
