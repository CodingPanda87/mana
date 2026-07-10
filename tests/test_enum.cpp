#include <mana/util/enum.hpp>
#include <cassert>
#include <iostream>

// Define test enum and register it
enum class Color { Red, Green, Blue, Yellow };
MANA_ENUM_REGISTER(Color,
    Entry{Color::Red,    "Red"},
    Entry{Color::Green,  "Green"},
    Entry{Color::Blue,   "Blue"},
    Entry{Color::Yellow, "Yellow"}
)

enum class Direction { North, South, East, West };
MANA_ENUM_REGISTER(Direction,
    Entry{Direction::North, "North"},
    Entry{Direction::South, "South"},
    Entry{Direction::East,  "East"},
    Entry{Direction::West,  "West"}
)

void test_enum_to_string() {
    static_assert(to_string(Color::Red) == "Red");
    static_assert(to_string(Color::Green) == "Green");
    static_assert(to_string(Color::Blue) == "Blue");

    assert(to_string(Color::Red) == "Red");
    assert(to_string(Direction::North) == "North");
}

void test_enum_from_string() {
    static_assert(Color_from_string("Red") == Color::Red);
    static_assert(Color_from_string("Green") == Color::Green);
    static_assert(!Color_from_string("Invalid").has_value());

    assert(Color_from_string("Red") == Color::Red);
    assert(Color_from_string("Invalid") == std::nullopt);
    assert(Direction_from_string("West") == Direction::West);
}

void test_enum_values() {
    auto values = enum_values(Color{});
    assert(values.size() == 4);
    assert(values[0] == Color::Red);
    assert(values[1] == Color::Green);
    assert(values[2] == Color::Blue);
    assert(values[3] == Color::Yellow);
}

void test_enum_names() {
    auto names = enum_names(Color{});
    assert(names.size() == 4);
    assert(names[0] == "Red");
    assert(names[1] == "Green");
    assert(names[2] == "Blue");
    assert(names[3] == "Yellow");
}
