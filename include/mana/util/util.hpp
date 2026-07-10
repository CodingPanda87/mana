#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <optional>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace mana::util {

// Type conversion
template<typename T>
inline std::string to_string(T value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else {
        return std::to_string(value);
    }
}

template<typename T>
inline std::optional<T> from_string(std::string_view str) {
    T value{};
    std::string s(str);
    std::istringstream iss{s};
    iss >> value;
    if (iss.fail()) {
        return std::nullopt;
    }
    return value;
}

// Container operations
template<typename T>
inline bool contains(const std::vector<T>& vec, const T& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

template<typename T>
inline void remove(std::vector<T>& vec, const T& value) {
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

template<typename T>
inline std::vector<T> filter(const std::vector<T>& vec, auto predicate) {
    std::vector<T> result;
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), predicate);
    return result;
}

template<typename T, typename F>
inline auto map(const std::vector<T>& vec, F func) -> std::vector<decltype(func(std::declval<T>()))> {
    std::vector<decltype(func(std::declval<T>()))> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        result.push_back(func(item));
    }
    return result;
}

// Timestamps
inline int64_t timestamp_ms() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

inline int64_t timestamp_s() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

inline std::string timestamp_iso8601() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm utc_time{};
    gmtime_s(&utc_time, &time);  // Windows thread-safe version

    std::ostringstream oss;
    oss << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    return oss.str();
}

// Range check
template<typename T>
inline constexpr bool in_range(T value, T min, T max) {
    return value >= min && value <= max;
}

// Byte operations
inline std::string to_hex(std::string_view data) {
    std::string result;
    result.reserve(data.size() * 2);
    for (unsigned char c : data) {
        result += "0123456789abcdef"[c >> 4];
        result += "0123456789abcdef"[c & 0x0f];
    }
    return result;
}

inline std::optional<std::string> from_hex(std::string_view hex) {
    if (hex.length() % 2 != 0) {
        return std::nullopt;
    }
    std::string result;
    result.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte_str(hex.substr(i, 2));
        char* end = nullptr;
        auto byte = static_cast<unsigned char>(std::strtol(byte_str.c_str(), &end, 16));
        if (end != byte_str.c_str() + 2) {
            return std::nullopt;
        }
        result += static_cast<char>(byte);
    }
    return result;
}

} // namespace mana::util
