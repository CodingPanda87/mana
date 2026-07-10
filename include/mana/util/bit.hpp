#pragma once

#include <cstdint>
#include <type_traits>
#include <bit>
#include <concepts>

namespace mana::util {

// Popcount - count number of 1 bits
template<std::unsigned_integral T>
inline constexpr int popcount(T x) noexcept {
    return std::popcount(x);
}

// Count leading zeros (from MSB)
template<std::unsigned_integral T>
inline constexpr int countl_zero(T x) noexcept {
    return std::countl_zero(x);
}

// Count trailing zeros (from LSB)
template<std::unsigned_integral T>
inline constexpr int countr_zero(T x) noexcept {
    return std::countr_zero(x);
}

// Round up to next power of 2
template<std::unsigned_integral T>
inline constexpr T bit_ceil(T x) noexcept {
    return std::bit_ceil(x);
}

// Round down to previous power of 2
template<std::unsigned_integral T>
inline constexpr T bit_floor(T x) noexcept {
    return std::bit_floor(x);
}

// Check if x is a power of 2
template<std::unsigned_integral T>
inline constexpr bool has_single_bit(T x) noexcept {
    return std::has_single_bit(x);
}

// Byte swap
template<std::unsigned_integral T>
inline constexpr T byteswap(T x) noexcept {
    return std::byteswap(x);
}

// Endian detection
inline constexpr bool is_little_endian = (std::endian::native == std::endian::little);
inline constexpr bool is_big_endian = (std::endian::native == std::endian::big);

// Convert to little-endian
template<std::unsigned_integral T>
inline constexpr T to_le(T x) noexcept {
    if constexpr (is_little_endian) {
        return x;
    } else {
        return byteswap(x);
    }
}

// Convert to big-endian
template<std::unsigned_integral T>
inline constexpr T to_be(T x) noexcept {
    if constexpr (is_big_endian) {
        return x;
    } else {
        return byteswap(x);
    }
}

} // namespace mana::util
