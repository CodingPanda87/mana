#pragma once

#include <type_traits>
#include <tuple>
#include <iterator>
#include <concepts>

namespace mana::util {

// Container concept - detects begin/end/size/value_type
template<typename T>
concept Container = requires(T t) {
    std::begin(t);
    std::end(t);
    std::size(t);
    typename T::value_type;
};

// Tuple-like concept
template<typename T>
concept TupleLike = requires {
    std::tuple_size<std::remove_cvref_t<T>>::value;
};

// Check if type is a specialization of a template
template<typename T, template<typename...> class Template>
struct is_specialization_of : std::false_type {};

template<template<typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

template<typename T, template<typename...> class Template>
inline constexpr bool is_specialization_of_v = is_specialization_of<T, Template>::value;

// TypeList - compile-time type list operations
template<typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);

    template<typename T>
    static constexpr bool contains = (std::is_same_v<T, Ts> || ...);

    template<template<typename> class F>
    using transform = TypeList<F<Ts>...>;

    template<typename T>
    using append = TypeList<Ts..., T>;

    template<typename T>
    using prepend = TypeList<T, Ts...>;
};

// Type index - find position of T in Ts...
namespace detail {
    template<typename T, typename... Ts>
    struct index_of_impl;

    template<typename T, typename First, typename... Rest>
    struct index_of_impl<T, First, Rest...> {
        static constexpr size_t value = std::is_same_v<T, First>
            ? 0
            : 1 + index_of_impl<T, Rest...>::value;
    };

    template<typename T>
    struct index_of_impl<T> {
        static constexpr size_t value = 0;  // Not found, return 0 as fallback
    };
} // namespace detail

template<typename T, typename... Ts>
inline constexpr size_t index_of = detail::index_of_impl<T, Ts...>::value;

// Nth type
template<size_t N, typename... Ts>
using nth_type = std::tuple_element_t<N, std::tuple<Ts...>>;

// Check if type is in parameter pack
template<typename T, typename... Ts>
inline constexpr bool is_one_of = (std::is_same_v<T, Ts> || ...);

} // namespace mana::util
