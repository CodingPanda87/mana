#pragma once

#include <utility>
#include <exception>

namespace mana::util {

template<typename F>
class ScopeGuard {
public:
    constexpr explicit ScopeGuard(F&& func) noexcept
        : func_(std::move(func)), active_(true) {}

    constexpr explicit ScopeGuard(const F& func) noexcept
        : func_(func), active_(true) {}

    ScopeGuard(ScopeGuard&& other) noexcept
        : func_(std::move(other.func_)), active_(other.active_) {
        other.active_ = false;
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    constexpr ~ScopeGuard() noexcept {
        if (active_) {
            try { func_(); } catch (...) {}
        }
    }

    constexpr void dismiss() noexcept {
        active_ = false;
    }

private:
    F func_;
    bool active_;
};

template<typename F>
constexpr ScopeGuard<F> defer(F&& func) noexcept {
    return ScopeGuard<F>(std::forward<F>(func));
}

template<typename F>
class ScopeSuccess {
public:
    constexpr explicit ScopeSuccess(F&& func) noexcept
        : func_(std::move(func)), uncaught_count_(std::uncaught_exceptions()) {}

    constexpr explicit ScopeSuccess(const F& func) noexcept
        : func_(func), uncaught_count_(std::uncaught_exceptions()) {}

    ScopeSuccess(ScopeSuccess&& other) noexcept
        : func_(std::move(other.func_)), uncaught_count_(other.uncaught_count_) {}

    ScopeSuccess(const ScopeSuccess&) = delete;
    ScopeSuccess& operator=(const ScopeSuccess&) = delete;
    ScopeSuccess& operator=(ScopeSuccess&&) = delete;

    constexpr ~ScopeSuccess() noexcept {
        if (std::uncaught_exceptions() == uncaught_count_) {
            try { func_(); } catch (...) {}
        }
    }

private:
    F func_;
    int uncaught_count_;
};

template<typename F>
class ScopeFailure {
public:
    constexpr explicit ScopeFailure(F&& func) noexcept
        : func_(std::move(func)), uncaught_count_(std::uncaught_exceptions()) {}

    constexpr explicit ScopeFailure(const F& func) noexcept
        : func_(func), uncaught_count_(std::uncaught_exceptions()) {}

    ScopeFailure(ScopeFailure&& other) noexcept
        : func_(std::move(other.func_)), uncaught_count_(other.uncaught_count_) {}

    ScopeFailure(const ScopeFailure&) = delete;
    ScopeFailure& operator=(const ScopeFailure&) = delete;
    ScopeFailure& operator=(ScopeFailure&&) = delete;

    constexpr ~ScopeFailure() noexcept {
        if (std::uncaught_exceptions() > uncaught_count_) {
            try { func_(); } catch (...) {}
        }
    }

private:
    F func_;
    int uncaught_count_;
};

} // namespace mana::util
