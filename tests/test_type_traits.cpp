#include <mana/util/type_traits.hpp>
#include <cassert>
#include <vector>
#include <string>
#include <tuple>
#include <map>

void test_type_traits_container() {
    // Should be containers
    static_assert(mana::util::Container<std::vector<int>>);
    static_assert(mana::util::Container<std::string>);
    static_assert(mana::util::Container<std::vector<std::string>>);

    // Should not be containers
    static_assert(!mana::util::Container<int>);
    static_assert(!mana::util::Container<double>);
    static_assert(!mana::util::Container<std::pair<int, int>>);
}

void test_type_traits_specialization() {
    // Should match
    static_assert(mana::util::is_specialization_of_v<std::vector<int>, std::vector>);
    static_assert(mana::util::is_specialization_of_v<std::string, std::basic_string>);
    static_assert(mana::util::is_specialization_of_v<std::pair<int, int>, std::pair>);

    // Should not match
    static_assert(!mana::util::is_specialization_of_v<int, std::vector>);
    static_assert(!mana::util::is_specialization_of_v<std::vector<int>, std::pair>);
}

void test_type_traits_typelist() {
    using List = mana::util::TypeList<int, double, std::string>;

    static_assert(List::size == 3);
    static_assert(List::contains<int>);
    static_assert(List::contains<double>);
    static_assert(List::contains<std::string>);
    static_assert(!List::contains<float>);

    // Transform
    using ConstList = List::transform<std::add_const_t>;
    static_assert(ConstList::contains<const int>);
    static_assert(ConstList::contains<const double>);
}
