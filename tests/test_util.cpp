#include <mana/util/time.hpp>
#include <cassert>
#include <iostream>

// String test declarations (defined in test_string.cpp)
void test_split();
void test_split_empty();
void test_split_empty_delimiter();
void test_trim();
void test_to_lower();
void test_to_upper();
void test_starts_with();
void test_ends_with();
void test_contains();
void test_replace();
void test_join();

// Log test declarations (defined in test_log.cpp)
void test_log_level();
void test_log_filtering();
void test_log_format();

// IO test declarations (defined in test_io_util.cpp)
void test_write_and_read();
void test_exists();
void test_file_size();
void test_read_nonexistent();
void test_append();
void test_read_lines();

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

// Event bus test declarations (defined in test_event_bus.cpp)
void test_event_bus_subscribe_publish();
void test_event_bus_multiple_subscribers();
void test_event_bus_unsubscribe();
void test_event_bus_different_types();

// Framework test declarations (defined in test_framework.cpp)
void test_framework_initialize();
void test_framework_singleton();
void test_framework_modules();

void test_timestamp() {
    auto ms = mana::util::time::current_timestamp_ms();
    auto s = mana::util::time::current_timestamp_s();
    assert(ms > 0);
    assert(s > 0);
    assert(ms >= s * 1000);
}

int main() {
    test_timestamp();
    std::cout << "All utility tests passed!" << std::endl;

    test_split();
    test_split_empty();
    test_split_empty_delimiter();
    test_trim();
    test_to_lower();
    test_to_upper();
    test_starts_with();
    test_ends_with();
    test_contains();
    test_replace();
    test_join();
    std::cout << "All string tests passed!" << std::endl;

    test_log_level();
    test_log_filtering();
    test_log_format();
    std::cout << "All logging tests passed!" << std::endl;

    test_write_and_read();
    test_exists();
    test_file_size();
    test_read_nonexistent();
    test_append();
    test_read_lines();
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

    test_event_bus_subscribe_publish();
    test_event_bus_multiple_subscribers();
    test_event_bus_unsubscribe();
    test_event_bus_different_types();
    std::cout << "All event bus tests passed!" << std::endl;

    test_framework_initialize();
    test_framework_singleton();
    test_framework_modules();
    std::cout << "All framework tests passed!" << std::endl;

    return 0;
}
