#include <mana/util/fixed.hpp>
#include <cassert>
#include <string_view>

void test_fixed_string() {
    mana::util::fixed_string<16> fs("hello");
    assert(fs.size() == 5);
    assert(fs.view() == "hello");
    assert(std::string_view(fs.c_str()) == "hello");

    mana::util::fixed_string<16> empty;
    assert(empty.size() == 0);
    assert(empty.view() == "");
}

void test_fixed_string_nttp() {
    // Test as non-type template parameter
    constexpr auto fs = mana::util::fixed_string("test");
    static_assert(fs.size() == 4);
    static_assert(fs == mana::util::fixed_string("test"));
    static_assert(fs != mana::util::fixed_string("other"));
}

void test_fixed_vector() {
    mana::util::fixed_vector<int, 4> fv;
    assert(fv.empty());
    assert(!fv.full());
    assert(fv.size() == 0);
    assert(fv.capacity() == 4);

    fv.push_back(1);
    fv.push_back(2);
    fv.push_back(3);
    assert(fv.size() == 3);
    assert(fv[0] == 1);
    assert(fv[1] == 2);
    assert(fv[2] == 3);

    fv.pop_back();
    assert(fv.size() == 2);

    fv.clear();
    assert(fv.empty());
}

void test_fixed_vector_constexpr() {
    // Compile-time construction
    constexpr auto fv = [] {
        mana::util::fixed_vector<int, 4> v;
        v.push_back(10);
        v.push_back(20);
        v.push_back(30);
        return v;
    }();

    static_assert(fv.size() == 3);
    static_assert(fv[0] == 10);
    static_assert(fv[1] == 20);
    static_assert(fv[2] == 30);
}

void test_small_vector() {
    mana::util::small_vector<int, 4> sv;
    assert(sv.empty());

    // Should use stack storage
    sv.push_back(1);
    sv.push_back(2);
    assert(sv.size() == 2);
    assert(sv[0] == 1);
    assert(sv[1] == 2);

    // Exceed stack capacity - should switch to heap
    sv.push_back(3);
    sv.push_back(4);
    sv.push_back(5);  // This triggers heap allocation
    assert(sv.size() == 5);
    assert(sv[0] == 1);
    assert(sv[4] == 5);
}
