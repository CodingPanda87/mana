#include <mana/util/numeric.hpp>
#include <cassert>
#include <cstdint>
#include <limits>
#include <iostream>

void test_numeric_sat_cast() {
    // Signed to signed
    static_assert(mana::util::sat_cast<int8_t>(100) == 100);
    static_assert(mana::util::sat_cast<int8_t>(200) == 127);   // Overflow -> max
    static_assert(mana::util::sat_cast<int8_t>(-200) == -128); // Underflow -> min

    // Unsigned to signed
    static_assert(mana::util::sat_cast<int8_t>(static_cast<uint8_t>(200)) == 127);

    // Signed to unsigned
    static_assert(mana::util::sat_cast<uint8_t>(-5) == 0);     // Negative -> 0
    static_assert(mana::util::sat_cast<uint8_t>(200) == 200);
    static_assert(mana::util::sat_cast<uint8_t>(300) == 255);  // Overflow -> max

    assert(mana::util::sat_cast<int8_t>(200) == 127);
}

void test_numeric_safe_arithmetic() {
    using SA = mana::util::SafeArithmetic<int>;

    auto r1 = SA{100} + SA{200};
    assert(r1.value == 300);
    assert(!r1.overflow);

    auto r2 = SA{std::numeric_limits<int>::max()} + SA{1};
    assert(r2.overflow);

    auto r3 = SA{10} * SA{20};
    assert(r3.value == 200);
    assert(!r3.overflow);

    auto r4 = SA{0} / SA{5};
    assert(r4.value == 0);
    assert(!r4.overflow);

    auto r5 = SA{10} / SA{0};  // Division by zero
    assert(r5.overflow);
}

void test_numeric_overflow_detection() {
    assert(!mana::util::add_overflows(100, 200));
    assert(mana::util::add_overflows(std::numeric_limits<int>::max(), 1));

    assert(!mana::util::mul_overflows(100, 200));
    assert(mana::util::mul_overflows(std::numeric_limits<int>::max(), 2));
}

void test_numeric_gcd_lcm() {
    static_assert(mana::util::gcd(12, 8) == 4);
    static_assert(mana::util::gcd(8, 12) == 4);
    static_assert(mana::util::gcd(7, 13) == 1);
    static_assert(mana::util::gcd(0, 5) == 5);

    static_assert(mana::util::lcm(4, 6) == 12);
    static_assert(mana::util::lcm(6, 4) == 12);
    static_assert(mana::util::lcm(0, 5) == 0);

    assert(mana::util::gcd(12, 8) == 4);
    assert(mana::util::lcm(4, 6) == 12);
}

void test_numeric_ipow() {
    static_assert(mana::util::ipow(2, 0) == 1);
    static_assert(mana::util::ipow(2, 1) == 2);
    static_assert(mana::util::ipow(2, 10) == 1024);
    static_assert(mana::util::ipow(3, 3) == 27);
    static_assert(mana::util::ipow(10, 0) == 1);

    assert(mana::util::ipow(2, 10) == 1024);
}

void test_numeric_factorial() {
    static_assert(mana::util::factorial(0) == 1);
    static_assert(mana::util::factorial(1) == 1);
    static_assert(mana::util::factorial(5) == 120);
    static_assert(mana::util::factorial(10) == 3628800);

    assert(mana::util::factorial(5) == 120);
}
