#pragma once

#include <cstdint>
#include <string_view>
#include <array>
#include <functional>

namespace mana::util {

// FNV-1a hash (64-bit)
inline constexpr uint64_t fnv1a(std::string_view str) noexcept {
    constexpr uint64_t basis = 14695981039346656037ULL;
    constexpr uint64_t prime = 1099511628211ULL;

    uint64_t hash = basis;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= prime;
    }
    return hash;
}

// CRC32 hash
inline constexpr uint32_t crc32(std::string_view str) noexcept {
    // Generate lookup table at compile time
    constexpr auto table = [] {
        std::array<uint32_t, 256> t{};
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
            t[i] = crc;
        }
        return t;
    }();

    uint32_t crc = 0xFFFFFFFF;
    for (char c : str) {
        crc = table[(crc ^ static_cast<uint8_t>(c)) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// Hash combine (similar to boost::hash_combine)
namespace detail {
    template<typename T>
    constexpr size_t hash_one(const T& v) noexcept {
        return std::hash<T>{}(v);
    }

    template<typename T>
    constexpr void hash_combine_impl(size_t& seed, const T& v) noexcept {
        seed ^= hash_one(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
} // namespace detail

template<typename... Ts>
constexpr size_t hash_combine(const Ts&... values) noexcept {
    size_t seed = 0;
    (detail::hash_combine_impl(seed, values), ...);
    return seed;
}

} // namespace mana::util
