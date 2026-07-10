#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace mana::util::math {

// Constants
inline constexpr double pi = 3.14159265358979323846;
inline constexpr double e  = 2.71828182845904523536;
inline constexpr double epsilon = std::numeric_limits<double>::epsilon();

// Basic constexpr functions
template<typename T>
inline constexpr T clamp(T value, T min_val, T max_val) {
    return (value < min_val) ? min_val : (value > max_val) ? max_val : value;
}

template<typename T>
inline constexpr T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

template<typename T>
inline constexpr T abs(T value) {
    return (value < 0) ? -value : value;
}

template<typename T>
inline constexpr T min(T a, T b) {
    return (a < b) ? a : b;
}

template<typename T>
inline constexpr T max(T a, T b) {
    return (a > b) ? a : b;
}

// Degree/radian conversion
inline constexpr double to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline constexpr double to_degrees(double radians) {
    return radians * 180.0 / pi;
}

// Constexpr trigonometric functions (Taylor series approximation)
namespace detail {
    inline constexpr double factorial(int n) {
        double result = 1.0;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    }

    inline constexpr double power(double base, int exp) {
        double result = 1.0;
        for (int i = 0; i < exp; ++i) {
            result *= base;
        }
        return result;
    }

    inline constexpr double normalize_angle(double x) {
        // Normalize to [-pi, pi]
        while (x > pi) x -= 2 * pi;
        while (x < -pi) x += 2 * pi;
        return x;
    }
} // namespace detail

inline constexpr double sin(double x) {
    x = detail::normalize_angle(x);
    double result = 0.0;
    double term = x;
    for (int n = 0; n < 15; ++n) {
        result += term;
        term *= -x * x / ((2 * n + 2) * (2 * n + 3));
    }
    return result;
}

inline constexpr double cos(double x) {
    return sin(x + pi / 2.0);
}

inline constexpr double tan(double x) {
    double c = cos(x);
    if (abs(c) < epsilon) {
        return (sin(x) > 0) ? 1e308 : -1e308;
    }
    return sin(x) / c;
}

inline constexpr double sqrt(double x) {
    if (x < 0) return 0;
    if (x == 0) return 0;

    double guess = x / 2.0;
    for (int i = 0; i < 100; ++i) {
        guess = (guess + x / guess) / 2.0;
    }
    return guess;
}

} // namespace mana::util::math
