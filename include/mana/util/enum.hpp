#pragma once

#include <string_view>
#include <optional>
#include <array>
#include <cstddef>

// Helper to generate enum registry and reflection functions.
// Usage:
//   enum class Color { Red, Green, Blue };
//   MANA_ENUM_REGISTER(Color,
//       Entry{Color::Red,   "Red"},
//       Entry{Color::Green, "Green"},
//       Entry{Color::Blue,  "Blue"}
//   )
//
// Generates:
//   to_string(Color)           -> std::string_view (constexpr)
//   Color_from_string(sv)      -> std::optional<Color> (constexpr)
//   enum_values(Color{})       -> std::array<Color, N> (constexpr)
//   enum_names(Color{})        -> std::array<std::string_view, N> (constexpr)

#define MANA_ENUM_REGISTER(EnumType, ...)                                      \
    struct enum_registry_##EnumType {                                           \
        struct Entry { EnumType value; std::string_view name; };               \
        static inline constexpr Entry entries[] = { __VA_ARGS__ };             \
        static constexpr std::size_t count = sizeof(entries) / sizeof(Entry);  \
    };                                                                         \
                                                                               \
    inline constexpr std::string_view to_string(EnumType e) {                  \
        using R = enum_registry_##EnumType;                                    \
        for (std::size_t i = 0; i < R::count; ++i) {                          \
            if (R::entries[i].value == e) return R::entries[i].name;           \
        }                                                                      \
        return "Unknown";                                                      \
    }                                                                          \
                                                                               \
    inline constexpr std::optional<EnumType> EnumType##_from_string(           \
        std::string_view s) {                                                  \
        using R = enum_registry_##EnumType;                                    \
        for (std::size_t i = 0; i < R::count; ++i) {                          \
            if (R::entries[i].name == s) return R::entries[i].value;           \
        }                                                                      \
        return std::nullopt;                                                   \
    }                                                                          \
                                                                               \
    inline constexpr auto enum_values(EnumType) {                              \
        using R = enum_registry_##EnumType;                                    \
        std::array<EnumType, R::count> values{};                               \
        for (std::size_t i = 0; i < R::count; ++i) {                          \
            values[i] = R::entries[i].value;                                   \
        }                                                                      \
        return values;                                                         \
    }                                                                          \
                                                                               \
    inline constexpr auto enum_names(EnumType) {                               \
        using R = enum_registry_##EnumType;                                    \
        std::array<std::string_view, R::count> names{};                        \
        for (std::size_t i = 0; i < R::count; ++i) {                          \
            names[i] = R::entries[i].name;                                     \
        }                                                                      \
        return names;                                                          \
    }
