#include <mana/util/hash.hpp>
#include <cassert>
#include <iostream>

void test_hash_fnv1a() {
    // Compile-time verification
    constexpr auto h1 = mana::util::fnv1a("hello");
    constexpr auto h2 = mana::util::fnv1a("hello");
    constexpr auto h3 = mana::util::fnv1a("world");
    static_assert(h1 == h2);  // Same input -> same output
    static_assert(h1 != h3);  // Different input -> different output

    // Runtime verification
    assert(h1 == h2);
    assert(h1 != h3);
    std::cout << "fnv1a(\"hello\") = " << h1 << std::endl;
}

void test_hash_crc32() {
    // Compile-time verification
    constexpr auto h1 = mana::util::crc32("hello");
    constexpr auto h2 = mana::util::crc32("hello");
    constexpr auto h3 = mana::util::crc32("world");
    static_assert(h1 == h2);
    static_assert(h1 != h3);

    // Known CRC32 values
    constexpr auto h4 = mana::util::crc32("123456789");
    static_assert(h4 == 0xCBF43926);  // Known CRC32 of "123456789"

    assert(h1 == h2);
    assert(h1 != h3);
    std::cout << "crc32(\"hello\") = " << h1 << std::endl;
}

void test_hash_combine() {
    auto h1 = mana::util::hash_combine(42);
    auto h2 = mana::util::hash_combine(42, 100);
    auto h3 = mana::util::hash_combine(42, 200);
    assert(h1 != 0);
    assert(h2 != h3);  // Different seeds produce different results
}
