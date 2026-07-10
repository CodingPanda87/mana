#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <concepts>

namespace mana::util {

// Saturating cast - clamp to target range on overflow/underflow
template<std::integral To, std::integral From>
inline constexpr To sat_cast(From value) noexcept {
    constexpr auto lo = std::numeric_limits<To>::lowest();
    constexpr auto hi = std::numeric_limits<To>::max();

    // signed -> unsigned: negative values clamp to 0
    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        if (value < 0) return 0;
        // value is non-negative, safe to compare as unsigned
        if (static_cast<std::make_unsigned_t<From>>(value) > static_cast<std::make_unsigned_t<To>>(hi))
            return hi;
        return static_cast<To>(value);
    }
    // unsigned -> signed: only upper bound check needed
    else if constexpr (std::is_unsigned_v<From> && std::is_signed_v<To>) {
        if (value > static_cast<From>(hi)) return hi;
        return static_cast<To>(value);
    }
    // same signedness
    else {
        if constexpr (sizeof(From) > sizeof(To)) {
            // Narrowing: check both bounds (From is wider so casts are exact)
            if (value > static_cast<From>(hi)) return hi;
            if (value < static_cast<From>(lo)) return lo;
        }
        // Widening or same size: always fits
        return static_cast<To>(value);
    }
}

// Overflow detection for addition
template<std::integral T>
inline constexpr bool add_overflows(T a, T b) noexcept {
    if constexpr (sizeof(T) < sizeof(int64_t)) {
        // Promote to wider type for safe comparison
        if constexpr (std::is_signed_v<T>) {
            auto result = static_cast<int64_t>(a) + static_cast<int64_t>(b);
            return result > std::numeric_limits<T>::max() || result < std::numeric_limits<T>::lowest();
        } else {
            auto result = static_cast<uint64_t>(a) + static_cast<uint64_t>(b);
            return result > std::numeric_limits<T>::max();
        }
    } else {
        // 64-bit types: manual overflow detection
        if constexpr (std::is_signed_v<T>) {
            if (b > 0 && a > std::numeric_limits<T>::max() - b) return true;
            if (b < 0 && a < std::numeric_limits<T>::lowest() - b) return true;
            return false;
        } else {
            return b > std::numeric_limits<T>::max() - a;
        }
    }
}

// Overflow detection for multiplication
template<std::integral T>
inline constexpr bool mul_overflows(T a, T b) noexcept {
    if constexpr (sizeof(T) < sizeof(int64_t)) {
        // Promote to wider type for safe comparison
        if constexpr (std::is_signed_v<T>) {
            auto result = static_cast<int64_t>(a) * static_cast<int64_t>(b);
            return result > std::numeric_limits<T>::max() || result < std::numeric_limits<T>::lowest();
        } else {
            auto result = static_cast<uint64_t>(a) * static_cast<uint64_t>(b);
            return result > std::numeric_limits<T>::max();
        }
    } else {
        // 64-bit types: manual overflow detection
        if (a == 0 || b == 0) return false;
        if constexpr (std::is_signed_v<T>) {
            T max_val = std::numeric_limits<T>::max();
            T min_val = std::numeric_limits<T>::lowest();
            if (a > 0 && b > 0 && a > max_val / b) return true;
            if (a > 0 && b < 0 && b < min_val / a) return true;
            if (a < 0 && b > 0 && a < min_val / b) return true;
            if (a < 0 && b < 0 && a < max_val / b) return true;
            return false;
        } else {
            return a > std::numeric_limits<T>::max() / b;
        }
    }
}

// Safe arithmetic with overflow detection
template<std::integral T>
struct SafeArithmetic {
    T value{};
    bool overflow = false;

    constexpr SafeArithmetic operator+(SafeArithmetic other) const noexcept {
        if (overflow || other.overflow) return {T{}, true};
        if (add_overflows(value, other.value)) return {T{}, true};
        return {static_cast<T>(value + other.value), false};
    }

    constexpr SafeArithmetic operator-(SafeArithmetic other) const noexcept {
        if (overflow || other.overflow) return {T{}, true};
        bool ov = false;
        if constexpr (std::is_signed_v<T>) {
            if (other.value > 0 && value < std::numeric_limits<T>::lowest() + other.value) ov = true;
            if (other.value < 0 && value > std::numeric_limits<T>::max() + other.value) ov = true;
        } else {
            if (value < other.value) ov = true;
        }
        if (ov) return {T{}, true};
        return {static_cast<T>(value - other.value), false};
    }

    constexpr SafeArithmetic operator*(SafeArithmetic other) const noexcept {
        if (overflow || other.overflow) return {T{}, true};
        if (mul_overflows(value, other.value)) return {T{}, true};
        return {static_cast<T>(value * other.value), false};
    }

    constexpr SafeArithmetic operator/(SafeArithmetic other) const noexcept {
        if (overflow || other.overflow) return {T{}, true};
        if (other.value == 0) return {T{}, true};
        return {static_cast<T>(value / other.value), false};
    }
};

// GCD (Greatest Common Divisor)
template<std::integral T>
inline constexpr T gcd(T a, T b) noexcept {
    if (a == 0) return b < 0 ? -b : b;
    if (b == 0) return a < 0 ? -a : a;
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
        T temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// LCM (Least Common Multiple)
template<std::integral T>
inline constexpr T lcm(T a, T b) noexcept {
    if (a == 0 || b == 0) return 0;
    T g = gcd(a, b);
    return (a / g) * b;
}

// Integer power (binary exponentiation)
template<std::integral T>
inline constexpr T ipow(T base, T exp) noexcept {
    if (exp == 0) return 1;
    if (exp < 0) return 0;  // Negative exponent not supported for integers

    T result = 1;
    while (exp > 0) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
}

// Factorial
inline constexpr int64_t factorial(int n) noexcept {
    if (n <= 1) return 1;
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

} // namespace mana::util
