#include <mana/util/math.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

void test_clamp() {
    assert(mana::util::math::clamp(5, 1, 10) == 5);
    assert(mana::util::math::clamp(0, 1, 10) == 1);
    assert(mana::util::math::clamp(15, 1, 10) == 10);
}

void test_lerp() {
    assert(mana::util::math::lerp(0.0, 10.0, 0.5) == 5.0);
    assert(mana::util::math::lerp(0.0, 10.0, 0.0) == 0.0);
    assert(mana::util::math::lerp(0.0, 10.0, 1.0) == 10.0);
}

void test_abs() {
    assert(mana::util::math::abs(5) == 5);
    assert(mana::util::math::abs(-5) == 5);
    assert(mana::util::math::abs(0) == 0);
}

void test_min_max() {
    assert(mana::util::math::min(3, 5) == 3);
    assert(mana::util::math::min(5, 3) == 3);
    assert(mana::util::math::max(3, 5) == 5);
    assert(mana::util::math::max(5, 3) == 5);
}

void test_radians_degrees() {
    assert(std::abs(mana::util::math::to_radians(180.0) - mana::util::math::pi) < 1e-10);
    assert(std::abs(mana::util::math::to_degrees(mana::util::math::pi) - 180.0) < 1e-10);
}

void test_sin() {
    assert(std::abs(mana::util::math::sin(0.0) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::sin(mana::util::math::pi / 2) - 1.0) < 1e-10);
    assert(std::abs(mana::util::math::sin(mana::util::math::pi) - 0.0) < 1e-10);
}

void test_cos() {
    assert(std::abs(mana::util::math::cos(0.0) - 1.0) < 1e-10);
    assert(std::abs(mana::util::math::cos(mana::util::math::pi / 2) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::cos(mana::util::math::pi) - (-1.0)) < 1e-10);
}

void test_tan() {
    assert(std::abs(mana::util::math::tan(0.0) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::tan(mana::util::math::pi / 4) - 1.0) < 1e-10);
}

void test_sqrt() {
    assert(std::abs(mana::util::math::sqrt(0.0) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::sqrt(1.0) - 1.0) < 1e-10);
    assert(std::abs(mana::util::math::sqrt(4.0) - 2.0) < 1e-10);
    assert(std::abs(mana::util::math::sqrt(9.0) - 3.0) < 1e-10);
}
