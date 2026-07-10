#pragma once

#include <type_traits>
#include <utility>
#include <new>

namespace mana::util {

template<typename T>
concept IsResult = requires { typename T::value_type; typename T::error_type; };

template<typename T, typename E>
class Result {
public:
    using value_type = T;
    using error_type = E;

    // Factory methods
    constexpr static Result ok(T value) {
        Result r;
        r.tag_ = Tag::Ok;
        ::new (&r.storage_) T(std::move(value));
        return r;
    }

    constexpr static Result err(E error) {
        Result r;
        r.tag_ = Tag::Err;
        ::new (&r.storage_) E(std::move(error));
        return r;
    }

    // Copy / move
    constexpr Result(const Result& other) : tag_(other.tag_) {
        if (tag_ == Tag::Ok) {
            ::new (&storage_) T(other.as_ok());
        } else {
            ::new (&storage_) E(other.as_err());
        }
    }

    constexpr Result(Result&& other) noexcept : tag_(other.tag_) {
        if (tag_ == Tag::Ok) {
            ::new (&storage_) T(std::move(other.as_ok()));
        } else {
            ::new (&storage_) E(std::move(other.as_err()));
        }
    }

    constexpr Result& operator=(const Result& other) {
        if (this != &other) {
            destroy();
            tag_ = other.tag_;
            if (tag_ == Tag::Ok) {
                ::new (&storage_) T(other.as_ok());
            } else {
                ::new (&storage_) E(other.as_err());
            }
        }
        return *this;
    }

    constexpr Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            destroy();
            tag_ = other.tag_;
            if (tag_ == Tag::Ok) {
                ::new (&storage_) T(std::move(other.as_ok()));
            } else {
                ::new (&storage_) E(std::move(other.as_err()));
            }
        }
        return *this;
    }

    constexpr ~Result() { destroy(); }

    // State queries
    constexpr bool is_ok() const noexcept { return tag_ == Tag::Ok; }
    constexpr bool is_err() const noexcept { return tag_ == Tag::Err; }
    constexpr explicit operator bool() const noexcept { return is_ok(); }

    // Value access
    constexpr const T& value() const& { return as_ok(); }
    constexpr T& value() & { return as_ok(); }
    constexpr T value() && { return std::move(as_ok()); }

    constexpr const E& error() const& { return as_err(); }
    constexpr E& error() & { return as_err(); }

    // Safe access
    constexpr T value_or(T default_value) const& {
        if (is_ok()) return as_ok();
        return default_value;
    }

    // Monadic operations
    template<typename F>
    constexpr auto and_then(F&& func) const& -> decltype(func(std::declval<const T&>())) {
        using U = decltype(func(std::declval<const T&>()));
        if (is_ok()) return func(as_ok());
        return U::err(as_err());
    }

    template<typename F>
    constexpr auto and_then(F&& func) && -> decltype(func(std::declval<T>())) {
        using U = decltype(func(std::declval<T>()));
        if (is_ok()) return func(std::move(as_ok()));
        return U::err(std::move(as_err()));
    }

    template<typename F>
    constexpr auto transform(F&& func) const& {
        using U = decltype(func(std::declval<const T&>()));
        if (is_ok()) return Result<U, E>::ok(func(as_ok()));
        return Result<U, E>::err(as_err());
    }

    template<typename F>
    constexpr auto transform(F&& func) && {
        using U = decltype(func(std::declval<T>()));
        if (is_ok()) return Result<U, E>::ok(func(std::move(as_ok())));
        return Result<U, E>::err(std::move(as_err()));
    }

    template<typename F>
    constexpr auto or_else(F&& func) const& -> decltype(func(std::declval<const E&>())) {
        using R = decltype(func(std::declval<const E&>()));
        if (is_err()) return func(as_err());
        return R::ok(as_ok());
    }

    template<typename F>
    constexpr auto or_else(F&& func) && -> decltype(func(std::declval<E>())) {
        using R = decltype(func(std::declval<E>()));
        if (is_err()) return func(std::move(as_err()));
        return R::ok(std::move(as_ok()));
    }

    // Combine two Results
    template<typename T2>
    constexpr auto operator&(const Result<T2, E>& other) const {
        using Pair = std::pair<T, T2>;
        if (is_ok() && other.is_ok()) {
            return Result<Pair, E>::ok(std::make_pair(as_ok(), other.value()));
        }
        if (is_err()) return Result<Pair, E>::err(as_err());
        return Result<Pair, E>::err(other.error());
    }

private:
    enum class Tag : unsigned char { Ok, Err };

    constexpr Result() : tag_(Tag::Ok), storage_{} {}

    constexpr void destroy() {
        if (tag_ == Tag::Ok) {
            as_ok().~T();
        } else {
            as_err().~E();
        }
    }

    constexpr const T& as_ok() const { return *std::launder(reinterpret_cast<const T*>(&storage_)); }
    constexpr T& as_ok() { return *std::launder(reinterpret_cast<T*>(&storage_)); }
    constexpr const E& as_err() const { return *std::launder(reinterpret_cast<const E*>(&storage_)); }
    constexpr E& as_err() { return *std::launder(reinterpret_cast<E*>(&storage_)); }

    Tag tag_;
    alignas(T) alignas(E) unsigned char storage_[sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E)];
};

} // namespace mana::util
