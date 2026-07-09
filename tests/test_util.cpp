#include <mana/util.h>
#include <cassert>
#include <iostream>

// Log test declarations (defined in test_log.cpp)
void test_log_level();
void test_log_filtering();
void test_log_format();

// IO test declarations (defined in test_io.cpp)
void test_write_and_read();
void test_exists();
void test_file_size();
void test_read_nonexistent();

// Context test declarations (defined in test_context.cpp)
void test_context_put_get();
void test_context_has();
void test_context_remove();
void test_context_clear();
void test_context_keys();
void test_context_stack();

// Thread pool test declarations (defined in test_thread_pool.cpp)
void test_thread_pool_submit();
void test_thread_pool_multiple_tasks();
void test_thread_pool_wait_all();
void test_thread_pool_pending_count();

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

void test_split_empty_delimiter() {
    auto result = mana::util::split("hello", "");
    assert(result.size() == 1);
    assert(result[0] == "hello");
}

void test_trim() {
    assert(mana::util::trim("  hello  ") == "hello");
    assert(mana::util::trim("hello") == "hello");
    assert(mana::util::trim("  ") == "");
    assert(mana::util::trim("") == "");
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
    test_split_empty_delimiter();
    test_trim();
    test_to_lower();
    test_to_upper();
    test_starts_with();
    test_ends_with();
    test_timestamp();
    std::cout << "All utility tests passed!" << std::endl;

    test_log_level();
    test_log_filtering();
    test_log_format();
    std::cout << "All logging tests passed!" << std::endl;

    test_write_and_read();
    test_exists();
    test_file_size();
    test_read_nonexistent();
    std::cout << "All IO tests passed!" << std::endl;

    test_context_put_get();
    test_context_has();
    test_context_remove();
    test_context_clear();
    test_context_keys();
    test_context_stack();
    std::cout << "All context tests passed!" << std::endl;

    test_thread_pool_submit();
    test_thread_pool_multiple_tasks();
    test_thread_pool_wait_all();
    test_thread_pool_pending_count();
    std::cout << "All thread pool tests passed!" << std::endl;

    return 0;
}
