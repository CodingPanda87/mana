#pragma once

#include <optional>
#include <utility>
#include <functional>

namespace mana::util {

// and_then: T -> optional<U>
template<typename T, typename F>
constexpr auto and_then(std::optional<T>&& opt, F&& func)
    -> decltype(std::invoke(std::forward<F>(func), std::declval<T>())) {
    if (opt) {
        return std::invoke(std::forward<F>(func), std::move(*opt));
    }
    return std::nullopt;
}

template<typename T, typename F>
constexpr auto and_then(const std::optional<T>& opt, F&& func)
    -> decltype(std::invoke(std::forward<F>(func), std::declval<const T&>())) {
    if (opt) {
        return std::invoke(std::forward<F>(func), *opt);
    }
    return std::nullopt;
}

// transform: T -> U
template<typename T, typename F>
constexpr auto transform(std::optional<T>&& opt, F&& func)
    -> std::optional<decltype(std::invoke(std::forward<F>(func), std::declval<T>()))> {
    using U = decltype(std::invoke(std::forward<F>(func), std::declval<T>()));
    if (opt) {
        return std::optional<U>(std::invoke(std::forward<F>(func), std::move(*opt)));
    }
    return std::nullopt;
}

template<typename T, typename F>
constexpr auto transform(const std::optional<T>& opt, F&& func)
    -> std::optional<decltype(std::invoke(std::forward<F>(func), std::declval<const T&>()))> {
    using U = decltype(std::invoke(std::forward<F>(func), std::declval<const T&>()));
    if (opt) {
        return std::optional<U>(std::invoke(std::forward<F>(func), *opt));
    }
    return std::nullopt;
}

// or_else: () -> optional<T>
template<typename T, typename F>
constexpr std::optional<T> or_else(std::optional<T>&& opt, F&& func) {
    if (opt) {
        return std::move(opt);
    }
    return std::invoke(std::forward<F>(func));
}

template<typename T, typename F>
constexpr std::optional<T> or_else(const std::optional<T>& opt, F&& func) {
    if (opt) {
        return opt;
    }
    return std::invoke(std::forward<F>(func));
}

// value_or_else: () -> T (lazy default)
template<typename T, typename F>
constexpr T value_or_else(std::optional<T>&& opt, F&& func) {
    if (opt) {
        return std::move(*opt);
    }
    return std::invoke(std::forward<F>(func));
}

template<typename T, typename F>
constexpr T value_or_else(const std::optional<T>& opt, F&& func) {
    if (opt) {
        return *opt;
    }
    return std::invoke(std::forward<F>(func));
}

} // namespace mana::util
