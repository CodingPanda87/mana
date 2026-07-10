#include <mana/util/bit.hpp>
#include <cassert>
#include <iostream>

void test_bit_popcount() {
    static_assert(mana::util::popcount(0u) == 0);
    static_assert(mana::util::popcount(1u) == 1);
    static_assert(mana::util::popcount(0b10101100u) == 4);
    static_assert(mana::util::popcount(0xFFFFFFFFu) == 32);

    assert(mana::util::popcount(0u) == 0);
    assert(mana::util::popcount(1u) == 1);
    assert(mana::util::popcount(0b10101100u) == 4);
}

void test_bit_countl_zero() {
    static_assert(mana::util::countl_zero(0u) == 32);
    static_assert(mana::util::countl_zero(1u) == 31);
    static_assert(mana::util::countl_zero(0xFFu) == 24);
    static_assert(mana::util::countl_zero(0x80000000u) == 0);

    assert(mana::util::countl_zero(0u) == 32);
    assert(mana::util::countl_zero(1u) == 31);
}

void test_bit_bit_ceil() {
    static_assert(mana::util::bit_ceil(0u) == 1);
    static_assert(mana::util::bit_ceil(1u) == 1);
    static_assert(mana::util::bit_ceil(2u) == 2);
    static_assert(mana::util::bit_ceil(3u) == 4);
    static_assert(mana::util::bit_ceil(5u) == 8);
    static_assert(mana::util::bit_ceil(1000u) == 1024);

    assert(mana::util::bit_ceil(5u) == 8);
}

void test_bit_bit_floor() {
    static_assert(mana::util::bit_floor(0u) == 0);
    static_assert(mana::util::bit_floor(1u) == 1);
    static_assert(mana::util::bit_floor(2u) == 2);
    static_assert(mana::util::bit_floor(3u) == 2);
    static_assert(mana::util::bit_floor(5u) == 4);
    static_assert(mana::util::bit_floor(1000u) == 512);

    assert(mana::util::bit_floor(5u) == 4);
}

void test_bit_has_single_bit() {
    static_assert(mana::util::has_single_bit(0u) == false);
    static_assert(mana::util::has_single_bit(1u) == true);
    static_assert(mana::util::has_single_bit(2u) == true);
    static_assert(mana::util::has_single_bit(3u) == false);
    static_assert(mana::util::has_single_bit(16u) == true);

    assert(mana::util::has_single_bit(16u) == true);
    assert(mana::util::has_single_bit(15u) == false);
}

void test_bit_byteswap() {
    static_assert(mana::util::byteswap(static_cast<uint16_t>(0xAABB)) == 0xBBAA);
    static_assert(mana::util::byteswap(static_cast<uint32_t>(0xAABBCCDD)) == 0xDDCCBBAA);

    assert(mana::util::byteswap(static_cast<uint16_t>(0xAABB)) == 0xBBAA);
}

void test_bit_endian() {
    // Just verify they compile and are constexpr
    constexpr bool le = mana::util::is_little_endian;
    constexpr bool be = mana::util::is_big_endian;
    assert(le || be);  // Must be one or the other
    assert(!(le && be));  // Can't be both

    // to_le/to_be should be identity on corresponding system
    uint32_t val = 0x12345678;
    if constexpr (mana::util::is_little_endian) {
        assert(mana::util::to_le(val) == val);
    } else {
        assert(mana::util::to_be(val) == val);
    }

    std::cout << "System is " << (le ? "little-endian" : "big-endian") << std::endl;
}
