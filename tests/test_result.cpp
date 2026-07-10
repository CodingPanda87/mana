#include <mana/util/result.hpp>
#include <cassert>
#include <string>

void test_result_ok() {
    auto r = mana::util::Result<int, std::string>::ok(42);
    assert(r.is_ok());
    assert(!r.is_err());
    assert(static_cast<bool>(r));
    assert(r.value() == 42);
    assert(r.value_or(0) == 42);
}

void test_result_err() {
    auto r = mana::util::Result<int, std::string>::err("fail");
    assert(!r.is_ok());
    assert(r.is_err());
    assert(!static_cast<bool>(r));
    assert(r.error() == "fail");
    assert(r.value_or(99) == 99);
}

void test_result_and_then() {
    auto r = mana::util::Result<int, std::string>::ok(10);
    auto r2 = r.and_then([](int v) {
        return mana::util::Result<int, std::string>::ok(v * 2);
    });
    assert(r2.is_ok());
    assert(r2.value() == 20);

    auto r3 = mana::util::Result<int, std::string>::err("fail");
    auto r4 = r3.and_then([](int v) {
        return mana::util::Result<int, std::string>::ok(v * 2);
    });
    assert(r4.is_err());
    assert(r4.error() == "fail");
}

void test_result_transform() {
    auto r = mana::util::Result<int, std::string>::ok(10);
    auto r2 = r.transform([](int v) { return std::to_string(v); });
    assert(r2.is_ok());
    assert(r2.value() == "10");
}

void test_result_or_else() {
    auto r = mana::util::Result<int, std::string>::err("fail");
    auto r2 = r.or_else([](const std::string& e) {
        return mana::util::Result<int, std::string>::ok(0);
    });
    assert(r2.is_ok());
    assert(r2.value() == 0);
}

void test_result_operator_and() {
    auto r1 = mana::util::Result<int, std::string>::ok(1);
    auto r2 = mana::util::Result<double, std::string>::ok(2.0);
    auto combined = r1 & r2;
    assert(combined.is_ok());
    assert(std::get<0>(combined.value()) == 1);
    assert(std::get<1>(combined.value()) == 2.0);

    auto r3 = mana::util::Result<int, std::string>::err("fail");
    auto combined2 = r3 & r2;
    assert(combined2.is_err());
}
