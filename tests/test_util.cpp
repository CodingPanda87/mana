#include <mana/util.h>
#include <cassert>
#include <iostream>

void test_split() {
    auto result = mana::util::split("hello,world,foo", ",");
    assert(result.size() == 3);
    assert(result[0] == "hello");
    assert(result[1] == "world");
    assert(result[2] == "foo");
}

void test_split_empty() {
    auto result = mana::util::split("", ",");
    assert(result.size() == 1);
    assert(result[0] == "");
}

void test_trim() {
    assert(mana::util::trim("  hello  ") == "hello");
    assert(mana::util::trim("hello") == "hello");
    assert(mana::util::trim("  ") == "");
}

void test_to_lower() {
    assert(mana::util::to_lower("HELLO") == "hello");
    assert(mana::util::to_lower("Hello") == "hello");
}

void test_to_upper() {
    assert(mana::util::to_upper("hello") == "HELLO");
    assert(mana::util::to_upper("Hello") == "HELLO");
}

void test_starts_with() {
    assert(mana::util::starts_with("hello world", "hello") == true);
    assert(mana::util::starts_with("hello world", "world") == false);
}

void test_ends_with() {
    assert(mana::util::ends_with("hello world", "world") == true);
    assert(mana::util::ends_with("hello world", "hello") == false);
}

void test_timestamp() {
    auto ms = mana::util::current_timestamp_ms();
    auto s = mana::util::current_timestamp_s();
    assert(ms > 0);
    assert(s > 0);
    assert(ms >= s * 1000);
}

int main() {
    test_split();
    test_split_empty();
    test_trim();
    test_to_lower();
    test_to_upper();
    test_starts_with();
    test_ends_with();
    test_timestamp();
    std::cout << "All utility tests passed!" << std::endl;
    return 0;
}
