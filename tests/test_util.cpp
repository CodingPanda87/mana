#include <mana/util/util.hpp>
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
void test_util_modules();

// Math test declarations (defined in test_math.cpp)
void test_clamp();
void test_lerp();
void test_abs();
void test_min_max();
void test_radians_degrees();
void test_sin();
void test_cos();
void test_tan();
void test_sqrt();

// Geo test declarations (defined in test_geo.cpp)
void test_point();
void test_rect();
void test_circle();
void test_segment();
void test_haversine();
void test_bearing();
void test_destination();
void test_coord_convert();
void test_quadtree();

// Result test declarations (defined in test_result.cpp)
void test_result_ok();
void test_result_err();
void test_result_and_then();
void test_result_transform();
void test_result_or_else();
void test_result_operator_and();

// Scope guard test declarations (defined in test_scope_guard.cpp)
void test_scope_guard_defer();
void test_scope_guard_dismiss();
void test_scope_guard_success();
void test_scope_guard_failure();

// Enum test declarations (defined in test_enum.cpp)
void test_enum_to_string();
void test_enum_from_string();
void test_enum_values();
void test_enum_names();

// Hash test declarations (defined in test_hash.cpp)
void test_hash_fnv1a();
void test_hash_crc32();
void test_hash_combine();

// Type traits test declarations (defined in test_type_traits.cpp)
void test_type_traits_container();
void test_type_traits_specialization();
void test_type_traits_typelist();

// Bit test declarations (defined in test_bit.cpp)
void test_bit_popcount();
void test_bit_countl_zero();
void test_bit_bit_ceil();
void test_bit_bit_floor();
void test_bit_has_single_bit();
void test_bit_byteswap();
void test_bit_endian();

// Numeric test declarations (defined in test_numeric.cpp)
void test_numeric_sat_cast();
void test_numeric_safe_arithmetic();
void test_numeric_overflow_detection();
void test_numeric_gcd_lcm();
void test_numeric_ipow();
void test_numeric_factorial();

// Fixed container test declarations (defined in test_fixed.cpp)
void test_fixed_string();
void test_fixed_string_nttp();
void test_fixed_vector();
void test_fixed_vector_constexpr();
void test_small_vector();

// Optional test declarations (defined in test_optional.cpp)
void test_optional_and_then();
void test_optional_transform();
void test_optional_or_else();
void test_optional_value_or_else();

// Variant test declarations (defined in test_variant.cpp)
void test_variant_overloaded();
void test_variant_match();

void test_timestamp() {
    auto ms = mana::util::timestamp_ms();
    auto s = mana::util::timestamp_s();
    assert(ms > 0);
    assert(s > 0);
    assert(ms >= s * 1000);
}

void test_util_to_string() {
    assert(mana::util::to_string(42) == "42");
    assert(mana::util::to_string(3.14) == "3.140000");
    assert(mana::util::to_string(true) == "true");
    assert(mana::util::to_string(std::string("hello")) == "hello");
}

void test_util_from_string() {
    auto val1 = mana::util::from_string<int>("42");
    assert(val1.has_value());
    assert(val1.value() == 42);

    auto val2 = mana::util::from_string<double>("3.14");
    assert(val2.has_value());
    assert(val2.value() == 3.14);

    auto val3 = mana::util::from_string<int>("not_a_number");
    assert(!val3.has_value());
}

void test_util_contains() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    assert(mana::util::contains(vec, 3) == true);
    assert(mana::util::contains(vec, 6) == false);
}

void test_util_remove() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    mana::util::remove(vec, 3);
    assert(vec.size() == 4);
    assert(!mana::util::contains(vec, 3));
}

void test_util_filter() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};
    auto even = mana::util::filter(vec, [](int x) { return x % 2 == 0; });
    assert(even.size() == 3);
    assert(even[0] == 2);
    assert(even[1] == 4);
    assert(even[2] == 6);
}

void test_util_map() {
    std::vector<int> vec = {1, 2, 3};
    auto doubled = mana::util::map(vec, [](int x) { return x * 2; });
    assert(doubled.size() == 3);
    assert(doubled[0] == 2);
    assert(doubled[1] == 4);
    assert(doubled[2] == 6);
}

void test_util_timestamp() {
    auto ms = mana::util::timestamp_ms();
    auto s = mana::util::timestamp_s();
    assert(ms > 0);
    assert(s > 0);
    assert(ms >= s * 1000);
}

void test_util_timestamp_iso8601() {
    auto ts = mana::util::timestamp_iso8601();
    assert(ts.length() == 24); // "2026-07-10T12:00:00.000Z"
    assert(ts.back() == 'Z');
}

void test_util_in_range() {
    assert(mana::util::in_range(5, 1, 10) == true);
    assert(mana::util::in_range(0, 1, 10) == false);
    assert(mana::util::in_range(1, 1, 10) == true);
    assert(mana::util::in_range(10, 1, 10) == true);
}

void test_util_hex() {
    std::string data = "Hello";
    auto hex = mana::util::to_hex(data);
    assert(hex == "48656c6c6f");

    auto decoded = mana::util::from_hex(hex);
    assert(decoded.has_value());
    assert(decoded.value() == data);
}

void test_util_from_hex_invalid() {
    auto result = mana::util::from_hex("invalid");
    assert(!result.has_value());
}

int main() {
    test_timestamp();
    test_util_to_string();
    test_util_from_string();
    test_util_contains();
    test_util_remove();
    test_util_filter();
    test_util_map();
    test_util_timestamp();
    test_util_timestamp_iso8601();
    test_util_in_range();
    test_util_hex();
    test_util_from_hex_invalid();
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
    test_util_modules();
    std::cout << "All framework tests passed!" << std::endl;

    test_clamp();
    test_lerp();
    test_abs();
    test_min_max();
    test_radians_degrees();
    test_sin();
    test_cos();
    test_tan();
    test_sqrt();
    std::cout << "All math tests passed!" << std::endl;

    test_point();
    test_rect();
    test_circle();
    test_segment();
    test_haversine();
    test_bearing();
    test_destination();
    test_coord_convert();
    test_quadtree();
    std::cout << "All geo tests passed!" << std::endl;

    test_result_ok();
    test_result_err();
    test_result_and_then();
    test_result_transform();
    test_result_or_else();
    test_result_operator_and();
    std::cout << "All result tests passed!" << std::endl;

    test_scope_guard_defer();
    test_scope_guard_dismiss();
    test_scope_guard_success();
    test_scope_guard_failure();
    std::cout << "All scope guard tests passed!" << std::endl;

    test_enum_to_string();
    test_enum_from_string();
    test_enum_values();
    test_enum_names();
    std::cout << "All enum tests passed!" << std::endl;

    test_hash_fnv1a();
    test_hash_crc32();
    test_hash_combine();
    std::cout << "All hash tests passed!" << std::endl;

    test_type_traits_container();
    test_type_traits_specialization();
    test_type_traits_typelist();
    std::cout << "All type traits tests passed!" << std::endl;

    test_bit_popcount();
    test_bit_countl_zero();
    test_bit_bit_ceil();
    test_bit_bit_floor();
    test_bit_has_single_bit();
    test_bit_byteswap();
    test_bit_endian();
    std::cout << "All bit tests passed!" << std::endl;

    test_numeric_sat_cast();
    test_numeric_safe_arithmetic();
    test_numeric_overflow_detection();
    test_numeric_gcd_lcm();
    test_numeric_ipow();
    test_numeric_factorial();
    std::cout << "All numeric tests passed!" << std::endl;

    test_fixed_string();
    test_fixed_string_nttp();
    test_fixed_vector();
    test_fixed_vector_constexpr();
    test_small_vector();
    std::cout << "All fixed container tests passed!" << std::endl;

    test_optional_and_then();
    test_optional_transform();
    test_optional_or_else();
    test_optional_value_or_else();
    std::cout << "All optional tests passed!" << std::endl;

    test_variant_overloaded();
    test_variant_match();
    std::cout << "All variant tests passed!" << std::endl;

    return 0;
}
