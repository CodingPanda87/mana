#include <mana/util/string.hpp>
#include <cassert>
#include <iostream>

void test_split() {
    auto result = mana::util::string::split("hello,world,foo", ",");
    assert(result.size() == 3);
    assert(result[0] == "hello");
    assert(result[1] == "world");
    assert(result[2] == "foo");
}

void test_split_empty() {
    auto result = mana::util::string::split("", ",");
    assert(result.size() == 1);
    assert(result[0] == "");
}

void test_split_empty_delimiter() {
    auto result = mana::util::string::split("hello", "");
    assert(result.size() == 1);
    assert(result[0] == "hello");
}

void test_trim() {
    assert(mana::util::string::trim("  hello  ") == "hello");
    assert(mana::util::string::trim("hello") == "hello");
    assert(mana::util::string::trim("  ") == "");
    assert(mana::util::string::trim("") == "");
}

void test_to_lower() {
    assert(mana::util::string::to_lower("HELLO") == "hello");
    assert(mana::util::string::to_lower("Hello") == "hello");
}

void test_to_upper() {
    assert(mana::util::string::to_upper("hello") == "HELLO");
    assert(mana::util::string::to_upper("Hello") == "HELLO");
}

void test_starts_with() {
    assert(mana::util::string::starts_with("hello world", "hello") == true);
    assert(mana::util::string::starts_with("hello world", "world") == false);
}

void test_ends_with() {
    assert(mana::util::string::ends_with("hello world", "world") == true);
    assert(mana::util::string::ends_with("hello world", "hello") == false);
}

void test_contains() {
    assert(mana::util::string::contains("hello world", "lo wo") == true);
    assert(mana::util::string::contains("hello world", "xyz") == false);
}

void test_replace() {
    assert(mana::util::string::replace("hello world", "world", "cpp") == "hello cpp");
    assert(mana::util::string::replace("aaa", "a", "b") == "bbb");
}

void test_join() {
    std::vector<std::string> parts = {"hello", "world", "foo"};
    assert(mana::util::string::join(parts, ",") == "hello,world,foo");
    assert(mana::util::string::join({}, ",") == "");
}
