# Util 模块扩充实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 为 Mana 框架补充 10 个高级 C++ 工具模块（header-only，constexpr/inline），覆盖错误处理、作用域守卫、枚举工具、编译期哈希、类型萃取、位操作、安全算术、固定容器、Optional/Variant 扩展。

**Architecture:** 全部采用 header-only (.hpp) 模式，放在 `include/mana/util/` 下。每个模块独立可测试，遵循 `mana::util::<module>` 命名规范。测试文件放在 `tests/` 下，使用 assert() 验证。

**Tech Stack:** C++23, MSVC 2019 16.10+, CMake 3.20+

## Global Constraints

- 全部 header-only，零链接依赖
- 使用 C++20/23 特性：concepts、requires、if consteval、std::endian
- 遵循现有命名规范：`mana::util::<module>`
- 向后兼容：不修改现有模块，仅新增文件
- 每个文件独立 include，独立测试

---

## 文件结构

### 新增头文件（include/mana/util/）

| 文件 | 职责 |
|------|------|
| `result.hpp` | Result<T,E> 无异常错误处理 + monadic 链式操作 |
| `scope_guard.hpp` | ScopeGuard / defer / ScopeSuccess / ScopeFailure |
| `enum.hpp` | 枚举 ↔ 字符串转换（宏注册） |
| `hash.hpp` | 编译期 FNV-1a、CRC32、hash_combine |
| `type_traits.hpp` | Container concept、is_specialization_of、TypeList |
| `bit.hpp` | popcount、countl_zero、bit_ceil/floor、byteswap、endian |
| `numeric.hpp` | sat_cast、SafeArithmetic、gcd/lcm/ipow/factorial |
| `fixed.hpp` | fixed_string<N>、fixed_vector<T,N>、small_vector<T,N> |
| `optional.hpp` | and_then、transform、or_else、value_or_else |
| `variant.hpp` | overloaded、match() |

### 新增测试文件（tests/）

| 文件 | 测试内容 |
|------|----------|
| `test_result.cpp` | ok/err 构造、monadic 链、operator& 组合 |
| `test_scope_guard.cpp` | defer 执行、dismiss 取消、ScopeSuccess/Failure |
| `test_enum.cpp` | to_string、from_string、enum_values/names |
| `test_hash.cpp` | fnv1a、crc32 编译期验证、hash_combine |
| `test_type_traits.cpp` | Container concept、is_specialization_of、TypeList |
| `test_bit.cpp` | popcount、bit_ceil/floor、byteswap、endian |
| `test_numeric.cpp` | sat_cast、SafeArithmetic、gcd/lcm/ipow |
| `test_fixed.cpp` | fixed_string NTTP、fixed_vector constexpr、small_vector |
| `test_optional.cpp` | and_then、transform、or_else |
| `test_variant.cpp` | overloaded、match |

### 修改文件

| 文件 | 修改内容 |
|------|----------|
| `include/mana/mana.h` | 添加 10 个 #include |
| `tests/test_util.cpp` | 添加 10 个测试函数声明和调用 |
| `CMakeLists.txt` | 添加 10 个测试文件 |

---

## Task 1: 集成准备 — 更新 mana.h 和 CMakeLists.txt

**Files:**
- Modify: `include/mana/mana.h`
- Modify: `CMakeLists.txt`
- Modify: `tests/test_util.cpp`

**Interfaces:**
- Produces: 所有后续任务依赖的 include 和构建配置

- [ ] **Step 1: 更新 mana.h 添加新 include**

在 `mana.h` 的 util include 区域添加：

```cpp
// New util modules
#include "util/string.hpp"
#include "util/io.hpp"
#include "util/util.hpp"
#include "util/math.hpp"
#include "util/geo.hpp"
// Enrichment util modules
#include "util/result.hpp"
#include "util/scope_guard.hpp"
#include "util/enum.hpp"
#include "util/hash.hpp"
#include "util/type_traits.hpp"
#include "util/bit.hpp"
#include "util/numeric.hpp"
#include "util/fixed.hpp"
#include "util/optional.hpp"
#include "util/variant.hpp"
```

- [ ] **Step 2: 更新 CMakeLists.txt 添加测试文件**

在 `mana_tests` 的源文件列表末尾添加：

```cmake
    tests/test_result.cpp
    tests/test_scope_guard.cpp
    tests/test_enum.cpp
    tests/test_hash.cpp
    tests/test_type_traits.cpp
    tests/test_bit.cpp
    tests/test_numeric.cpp
    tests/test_fixed.cpp
    tests/test_optional.cpp
    tests/test_variant.cpp
```

- [ ] **Step 3: 更新 test_util.cpp 添加测试声明和调用**

在 test_util.cpp 的声明区域添加：

```cpp
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
```

在 main() 函数末尾、`return 0;` 之前添加：

```cpp
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
```

- [ ] **Step 4: 验证构建配置**

Run: `cmake -B build -S . && cmake --build build`
Expected: 编译失败（因为头文件还不存在），但 CMake 配置成功

- [ ] **Step 5: Commit**

```bash
git add include/mana/mana.h CMakeLists.txt tests/test_util.cpp
git commit -m "chore: prepare integration points for util enrichment modules"
```

---

## Task 2: 实现 result.hpp — Result<T,E> 错误处理

**Files:**
- Create: `include/mana/util/result.hpp`
- Create: `tests/test_result.cpp`

**Interfaces:**
- Produces: `mana::util::Result<T,E>` 类，供后续模块使用

- [ ] **Step 1: 编写测试文件 test_result.cpp**

```cpp
#include <mana/util/result.hpp>
#include <cassert>
#include <string>

void test_result_ok() {
    auto r = mana::util::Result<int, std::string>::ok(42);
    assert(r.is_ok());
    assert(!r.is_err());
    assert(static_cast<bool>(r));
    assert(r.value() == 42);
    assert(r.value_or(0) == 42);
}

void test_result_err() {
    auto r = mana::util::Result<int, std::string>::err("fail");
    assert(!r.is_ok());
    assert(r.is_err());
    assert(!static_cast<bool>(r));
    assert(r.error() == "fail");
    assert(r.value_or(99) == 99);
}

void test_result_and_then() {
    auto r = mana::util::Result<int, std::string>::ok(10);
    auto r2 = r.and_then([](int v) {
        return mana::util::Result<int, std::string>::ok(v * 2);
    });
    assert(r2.is_ok());
    assert(r2.value() == 20);

    auto r3 = mana::util::Result<int, std::string>::err("fail");
    auto r4 = r3.and_then([](int v) {
        return mana::util::Result<int, std::string>::ok(v * 2);
    });
    assert(r4.is_err());
    assert(r4.error() == "fail");
}

void test_result_transform() {
    auto r = mana::util::Result<int, std::string>::ok(10);
    auto r2 = r.transform([](int v) { return std::to_string(v); });
    assert(r2.is_ok());
    assert(r2.value() == "10");
}

void test_result_or_else() {
    auto r = mana::util::Result<int, std::string>::err("fail");
    auto r2 = r.or_else([](const std::string& e) {
        return mana::util::Result<int, std::string>::ok(0);
    });
    assert(r2.is_ok());
    assert(r2.value() == 0);
}

void test_result_operator_and() {
    auto r1 = mana::util::Result<int, std::string>::ok(1);
    auto r2 = mana::util::Result<double, std::string>::ok(2.0);
    auto combined = r1 & r2;
    assert(combined.is_ok());
    assert(std::get<0>(combined.value()) == 1);
    assert(std::get<1>(combined.value()) == 2.0);

    auto r3 = mana::util::Result<int, std::string>::err("fail");
    auto combined2 = r3 & r2;
    assert(combined2.is_err());
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: 编译失败（result.hpp 不存在）

- [ ] **Step 3: 实现 result.hpp**

```cpp
#pragma once

#include <variant>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <tuple>

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
        r.storage_ = std::move(value);
        return r;
    }

    constexpr static Result err(E error) {
        Result r;
        r.storage_ = std::move(error);
        return r;
    }

    // State queries
    constexpr bool is_ok() const noexcept {
        return std::holds_alternative<T>(storage_);
    }

    constexpr bool is_err() const noexcept {
        return std::holds_alternative<E>(storage_);
    }

    constexpr explicit operator bool() const noexcept {
        return is_ok();
    }

    // Value access
    constexpr const T& value() const& {
        return std::get<T>(storage_);
    }

    constexpr T& value() & {
        return std::get<T>(storage_);
    }

    constexpr T value() && {
        return std::get<T>(std::move(storage_));
    }

    constexpr const E& error() const& {
        return std::get<E>(storage_);
    }

    constexpr E& error() & {
        return std::get<E>(storage_);
    }

    // Safe access
    constexpr T value_or(T default_value) const& {
        if (is_ok()) return value();
        return default_value;
    }

    // Monadic operations
    template<typename F>
    constexpr auto and_then(F&& func) const& -> decltype(func(std::declval<const T&>())) {
        using U = decltype(func(std::declval<const T&>()));
        if (is_ok()) return func(value());
        return U::err(error());
    }

    template<typename F>
    constexpr auto and_then(F&& func) && -> decltype(func(std::declval<T>())) {
        using U = decltype(func(std::declval<T>()));
        if (is_ok()) return func(std::move(value()));
        return U::err(std::move(error()));
    }

    template<typename F>
    constexpr auto transform(F&& func) const& -> Result<decltype(func(std::declval<const T&>())), E> {
        using U = decltype(func(std::declval<const T&>()));
        if (is_ok()) return Result<U, E>::ok(func(value()));
        return Result<U, E>::err(error());
    }

    template<typename F>
    constexpr auto transform(F&& func) && -> Result<decltype(func(std::declval<T>())), E> {
        using U = decltype(func(std::declval<T>()));
        if (is_ok()) return Result<U, E>::ok(func(std::move(value())));
        return Result<U, E>::err(std::move(error()));
    }

    template<typename F>
    constexpr auto or_else(F&& func) const& -> decltype(func(std::declval<const E&>())) {
        using R = decltype(func(std::declval<const E&>()));
        if (is_err()) return func(error());
        return R::ok(value());
    }

    template<typename F>
    constexpr auto or_else(F&& func) && -> decltype(func(std::declval<E>())) {
        using R = decltype(func(std::declval<E>()));
        if (is_err()) return func(std::move(error()));
        return R::ok(std::move(value()));
    }

    // Combine two Results
    template<typename T2>
    constexpr auto operator&(const Result<T2, E>& other) const -> Result<std::pair<T, T2>, E> {
        if (is_ok() && other.is_ok()) {
            return Result<std::pair<T, T2>, E>::ok(std::make_pair(value(), other.value()));
        }
        if (is_err()) return Result<std::pair<T, T2>, E>::err(error());
        return Result<std::pair<T, T2>, E>::err(other.error());
    }

private:
    constexpr Result() = default;

    std::variant<T, E> storage_;
};

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All result tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/result.hpp tests/test_result.cpp
git commit -m "feat(util): add Result<T,E> error handling type with monadic operations"
```

---

## Task 3: 实现 scope_guard.hpp — 作用域守卫

**Files:**
- Create: `include/mana/util/scope_guard.hpp`
- Create: `tests/test_scope_guard.cpp`

**Interfaces:**
- Produces: `mana::util::ScopeGuard<F>`, `mana::util::defer()`, `mana::util::ScopeSuccess<F>`, `mana::util::ScopeFailure<F>`

- [ ] **Step 1: 编写测试文件 test_scope_guard.cpp**

```cpp
#include <mana/util/scope_guard.hpp>
#include <cassert>

void test_scope_guard_defer() {
    int counter = 0;
    {
        auto guard = mana::util::defer([&] { counter++; });
        assert(counter == 0);
    }
    assert(counter == 1);
}

void test_scope_guard_dismiss() {
    int counter = 0;
    {
        auto guard = mana::util::defer([&] { counter++; });
        guard.dismiss();
        assert(counter == 0);
    }
    assert(counter == 0);
}

void test_scope_guard_success() {
    int counter = 0;
    {
        auto guard = mana::util::ScopeSuccess([&] { counter++; });
        assert(counter == 0);
    }
    assert(counter == 1);  // No exception, so it fires
}

void test_scope_guard_failure() {
    int counter = 0;
    {
        auto guard = mana::util::ScopeFailure([&] { counter++; });
        assert(counter == 0);
    }
    assert(counter == 0);  // No exception, so it doesn't fire
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（scope_guard.hpp 不存在）

- [ ] **Step 3: 实现 scope_guard.hpp**

```cpp
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
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All scope guard tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/scope_guard.hpp tests/test_scope_guard.cpp
git commit -m "feat(util): add ScopeGuard, defer, ScopeSuccess, ScopeFailure"
```

---

## Task 4: 实现 hash.hpp — 编译期哈希

**Files:**
- Create: `include/mana/util/hash.hpp`
- Create: `tests/test_hash.cpp`

**Interfaces:**
- Produces: `mana::util::fnv1a()`, `mana::util::crc32()`, `mana::util::hash_combine()`

- [ ] **Step 1: 编写测试文件 test_hash.cpp**

```cpp
#include <mana/util/hash.hpp>
#include <cassert>
#include <iostream>

void test_hash_fnv1a() {
    // Compile-time verification
    constexpr auto h1 = mana::util::fnv1a("hello");
    constexpr auto h2 = mana::util::fnv1a("hello");
    constexpr auto h3 = mana::util::fnv1a("world");
    static_assert(h1 == h2);  // Same input -> same output
    static_assert(h1 != h3);  // Different input -> different output

    // Runtime verification
    assert(h1 == h2);
    assert(h1 != h3);
    std::cout << "fnv1a(\"hello\") = " << h1 << std::endl;
}

void test_hash_crc32() {
    // Compile-time verification
    constexpr auto h1 = mana::util::crc32("hello");
    constexpr auto h2 = mana::util::crc32("hello");
    constexpr auto h3 = mana::util::crc32("world");
    static_assert(h1 == h2);
    static_assert(h1 != h3);

    // Known CRC32 values
    constexpr auto h4 = mana::util::crc32("123456789");
    static_assert(h4 == 0xCBF43926);  // Known CRC32 of "123456789"

    assert(h1 == h2);
    assert(h1 != h3);
    std::cout << "crc32(\"hello\") = " << h1 << std::endl;
}

void test_hash_combine() {
    auto h1 = mana::util::hash_combine(42);
    auto h2 = mana::util::hash_combine(42, 100);
    auto h3 = mana::util::hash_combine(42, 200);
    assert(h1 != 0);
    assert(h2 != h3);  // Different seeds produce different results
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（hash.hpp 不存在）

- [ ] **Step 3: 实现 hash.hpp**

```cpp
#pragma once

#include <cstdint>
#include <string_view>
#include <array>
#include <functional>

namespace mana::util {

// FNV-1a hash (64-bit)
inline constexpr uint64_t fnv1a(std::string_view str) noexcept {
    constexpr uint64_t basis = 14695981039346656037ULL;
    constexpr uint64_t prime = 1099511628211ULL;

    uint64_t hash = basis;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= prime;
    }
    return hash;
}

// CRC32 hash
inline constexpr uint32_t crc32(std::string_view str) noexcept {
    // Generate lookup table at compile time
    constexpr auto table = [] {
        std::array<uint32_t, 256> t{};
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
            }
            t[i] = crc;
        }
        return t;
    }();

    uint32_t crc = 0xFFFFFFFF;
    for (char c : str) {
        crc = table[(crc ^ static_cast<uint8_t>(c)) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// Hash combine (similar to boost::hash_combine)
namespace detail {
    template<typename T>
    constexpr size_t hash_one(const T& v) noexcept {
        return std::hash<T>{}(v);
    }

    template<typename T>
    constexpr void hash_combine_impl(size_t& seed, const T& v) noexcept {
        seed ^= hash_one(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
} // namespace detail

template<typename... Ts>
constexpr size_t hash_combine(const Ts&... values) noexcept {
    size_t seed = 0;
    (detail::hash_combine_impl(seed, values), ...);
    return seed;
}

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All hash tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/hash.hpp tests/test_hash.cpp
git commit -m "feat(util): add compile-time FNV-1a, CRC32, and hash_combine"
```

---

## Task 5: 实现 type_traits.hpp — 类型萃取辅助

**Files:**
- Create: `include/mana/util/type_traits.hpp`
- Create: `tests/test_type_traits.cpp`

**Interfaces:**
- Produces: `mana::util::Container`, `mana::util::TupleLike`, `mana::util::is_specialization_of`, `mana::util::TypeList`

- [ ] **Step 1: 编写测试文件 test_type_traits.cpp**

```cpp
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
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（type_traits.hpp 不存在）

- [ ] **Step 3: 实现 type_traits.hpp**

```cpp
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
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All type traits tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/type_traits.hpp tests/test_type_traits.cpp
git commit -m "feat(util): add type traits helpers: Container concept, TypeList, is_specialization_of"
```

---

## Task 6: 实现 bit.hpp — 位操作

**Files:**
- Create: `include/mana/util/bit.hpp`
- Create: `tests/test_bit.cpp`

**Interfaces:**
- Produces: `mana::util::popcount()`, `mana::util::countl_zero()`, `mana::util::bit_ceil()`, `mana::util::bit_floor()`, `mana::util::has_single_bit()`, `mana::util::byteswap()`, `mana::util::to_le()`, `mana::util::to_be()`

- [ ] **Step 1: 编写测试文件 test_bit.cpp**

```cpp
#include <mana/util/bit.hpp>
#include <cassert>
#include <iostream>

void test_bit_popcount() {
    static_assert(mana::util::popcount(0u) == 0);
    static_assert(mana::util::popcount(1u) == 1);
    static_assert(mana::util::popcount(0b10101100u) == 4);
    static_assert(mana::util::popcount(0xFFFFFFFFu) == 32);

    assert(mana::util::popcount(0u) == 0);
    assert(mana::util::popcount(1u) == 1);
    assert(mana::util::popcount(0b10101100u) == 4);
}

void test_bit_countl_zero() {
    static_assert(mana::util::countl_zero(0u) == 32);
    static_assert(mana::util::countl_zero(1u) == 31);
    static_assert(mana::util::countl_zero(0xFFu) == 24);
    static_assert(mana::util::countl_zero(0x80000000u) == 0);

    assert(mana::util::countl_zero(0u) == 32);
    assert(mana::util::countl_zero(1u) == 31);
}

void test_bit_bit_ceil() {
    static_assert(mana::util::bit_ceil(0u) == 1);
    static_assert(mana::util::bit_ceil(1u) == 1);
    static_assert(mana::util::bit_ceil(2u) == 2);
    static_assert(mana::util::bit_ceil(3u) == 4);
    static_assert(mana::util::bit_ceil(5u) == 8);
    static_assert(mana::util::bit_ceil(1000u) == 1024);

    assert(mana::util::bit_ceil(5u) == 8);
}

void test_bit_bit_floor() {
    static_assert(mana::util::bit_floor(0u) == 0);
    static_assert(mana::util::bit_floor(1u) == 1);
    static_assert(mana::util::bit_floor(2u) == 2);
    static_assert(mana::util::bit_floor(3u) == 2);
    static_assert(mana::util::bit_floor(5u) == 4);
    static_assert(mana::util::bit_floor(1000u) == 512);

    assert(mana::util::bit_floor(5u) == 4);
}

void test_bit_has_single_bit() {
    static_assert(mana::util::has_single_bit(0u) == false);
    static_assert(mana::util::has_single_bit(1u) == true);
    static_assert(mana::util::has_single_bit(2u) == true);
    static_assert(mana::util::has_single_bit(3u) == false);
    static_assert(mana::util::has_single_bit(16u) == true);

    assert(mana::util::has_single_bit(16u) == true);
    assert(mana::util::has_single_bit(15u) == false);
}

void test_bit_byteswap() {
    static_assert(mana::util::byteswap(static_cast<uint16_t>(0xAABB)) == 0xBBAA);
    static_assert(mana::util::byteswap(static_cast<uint32_t>(0xAABBCCDD)) == 0xDDCCBBAA);

    assert(mana::util::byteswap(static_cast<uint16_t>(0xAABB)) == 0xBBAA);
}

void test_bit_endian() {
    // Just verify they compile and are constexpr
    constexpr bool le = mana::util::is_little_endian;
    constexpr bool be = mana::util::is_big_endian;
    assert(le || be);  // Must be one or the other
    assert(!(le && be));  // Can't be both

    // to_le/to_be should be identity on corresponding system
    uint32_t val = 0x12345678;
    if constexpr (mana::util::is_little_endian) {
        assert(mana::util::to_le(val) == val);
    } else {
        assert(mana::util::to_be(val) == val);
    }

    std::cout << "System is " << (le ? "little-endian" : "big-endian") << std::endl;
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（bit.hpp 不存在）

- [ ] **Step 3: 实现 bit.hpp**

```cpp
#pragma once

#include <cstdint>
#include <type_traits>
#include <bit>
#include <concepts>

namespace mana::util {

// Popcount - count number of 1 bits
template<std::unsigned_integral T>
inline constexpr int popcount(T x) noexcept {
    return std::popcount(x);
}

// Count leading zeros (from MSB)
template<std::unsigned_integral T>
inline constexpr int countl_zero(T x) noexcept {
    return std::countl_zero(x);
}

// Count trailing zeros (from LSB)
template<std::unsigned_integral T>
inline constexpr int countr_zero(T x) noexcept {
    return std::countr_zero(x);
}

// Round up to next power of 2
template<std::unsigned_integral T>
inline constexpr T bit_ceil(T x) noexcept {
    return std::bit_ceil(x);
}

// Round down to previous power of 2
template<std::unsigned_integral T>
inline constexpr T bit_floor(T x) noexcept {
    return std::bit_floor(x);
}

// Check if x is a power of 2
template<std::unsigned_integral T>
inline constexpr bool has_single_bit(T x) noexcept {
    return std::has_single_bit(x);
}

// Byte swap
template<std::unsigned_integral T>
inline constexpr T byteswap(T x) noexcept {
    return std::byteswap(x);
}

// Endian detection
inline constexpr bool is_little_endian = (std::endian::native == std::endian::little);
inline constexpr bool is_big_endian = (std::endian::native == std::endian::big);

// Convert to little-endian
template<std::unsigned_integral T>
inline constexpr T to_le(T x) noexcept {
    if constexpr (is_little_endian) {
        return x;
    } else {
        return byteswap(x);
    }
}

// Convert to big-endian
template<std::unsigned_integral T>
inline constexpr T to_be(T x) noexcept {
    if constexpr (is_big_endian) {
        return x;
    } else {
        return byteswap(x);
    }
}

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All bit tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/bit.hpp tests/test_bit.cpp
git commit -m "feat(util): add bit manipulation utilities using C++20 <bit>"
```

---

## Task 7: 实现 numeric.hpp — 安全算术

**Files:**
- Create: `include/mana/util/numeric.hpp`
- Create: `tests/test_numeric.cpp`

**Interfaces:**
- Produces: `mana::util::sat_cast()`, `mana::util::SafeArithmetic<T>`, `mana::util::add_overflows()`, `mana::util::mul_overflows()`, `mana::util::gcd()`, `mana::util::lcm()`, `mana::util::ipow()`, `mana::util::factorial()`

- [ ] **Step 1: 编写测试文件 test_numeric.cpp**

```cpp
#include <mana/util/numeric.hpp>
#include <cassert>
#include <cstdint>
#include <limits>
#include <iostream>

void test_numeric_sat_cast() {
    // Signed to signed
    static_assert(mana::util::sat_cast<int8_t>(100) == 100);
    static_assert(mana::util::sat_cast<int8_t>(200) == 127);   // Overflow -> max
    static_assert(mana::util::sat_cast<int8_t>(-200) == -128); // Underflow -> min

    // Unsigned to signed
    static_assert(mana::util::sat_cast<int8_t>(static_cast<uint8_t>(200)) == 127);

    // Signed to unsigned
    static_assert(mana::util::sat_cast<uint8_t>(-5) == 0);     // Negative -> 0
    static_assert(mana::util::sat_cast<uint8_t>(200) == 200);
    static_assert(mana::util::sat_cast<uint8_t>(300) == 255);  // Overflow -> max

    assert(mana::util::sat_cast<int8_t>(200) == 127);
}

void test_numeric_safe_arithmetic() {
    using SA = mana::util::SafeArithmetic<int>;

    auto r1 = SA{100} + SA{200};
    assert(r1.value == 300);
    assert(!r1.overflow);

    auto r2 = SA{std::numeric_limits<int>::max()} + SA{1};
    assert(r2.overflow);

    auto r3 = SA{10} * SA{20};
    assert(r3.value == 200);
    assert(!r3.overflow);

    auto r4 = SA{0} / SA{5};
    assert(r4.value == 0);
    assert(!r4.overflow);

    auto r5 = SA{10} / SA{0};  // Division by zero
    assert(r5.overflow);
}

void test_numeric_overflow_detection() {
    assert(!mana::util::add_overflows(100, 200));
    assert(mana::util::add_overflows(std::numeric_limits<int>::max(), 1));

    assert(!mana::util::mul_overflows(100, 200));
    assert(mana::util::mul_overflows(std::numeric_limits<int>::max(), 2));
}

void test_numeric_gcd_lcm() {
    static_assert(mana::util::gcd(12, 8) == 4);
    static_assert(mana::util::gcd(8, 12) == 4);
    static_assert(mana::util::gcd(7, 13) == 1);
    static_assert(mana::util::gcd(0, 5) == 5);

    static_assert(mana::util::lcm(4, 6) == 12);
    static_assert(mana::util::lcm(6, 4) == 12);
    static_assert(mana::util::lcm(0, 5) == 0);

    assert(mana::util::gcd(12, 8) == 4);
    assert(mana::util::lcm(4, 6) == 12);
}

void test_numeric_ipow() {
    static_assert(mana::util::ipow(2, 0) == 1);
    static_assert(mana::util::ipow(2, 1) == 2);
    static_assert(mana::util::ipow(2, 10) == 1024);
    static_assert(mana::util::ipow(3, 3) == 27);
    static_assert(mana::util::ipow(10, 0) == 1);

    assert(mana::util::ipow(2, 10) == 1024);
}

void test_numeric_factorial() {
    static_assert(mana::util::factorial(0) == 1);
    static_assert(mana::util::factorial(1) == 1);
    static_assert(mana::util::factorial(5) == 120);
    static_assert(mana::util::factorial(10) == 3628800);

    assert(mana::util::factorial(5) == 120);
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（numeric.hpp 不存在）

- [ ] **Step 3: 实现 numeric.hpp**

```cpp
#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <concepts>

namespace mana::util {

// Saturating cast - clamp to target range on overflow
template<std::integral To, std::integral From>
inline constexpr To sat_cast(From value) noexcept {
    constexpr auto lo = std::numeric_limits<To>::lowest();
    constexpr auto hi = std::numeric_limits<To>::max();

    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        if (value < 0) return 0;
    }

    if (static_cast<std::make_unsigned_t<From>>(value) > static_cast<std::make_unsigned_t<From>>(hi)) {
        return hi;
    }
    if constexpr (std::is_signed_v<From> && std::is_signed_v<To>) {
        if (value < static_cast<From>(lo)) return lo;
    }

    return static_cast<To>(value);
}

// Safe arithmetic with overflow detection
template<std::integral T>
struct SafeArithmetic {
    T value{};
    bool overflow = false;

    constexpr SafeArithmetic() = default;
    constexpr explicit SafeArithmetic(T v) : value(v) {}

    constexpr SafeArithmetic operator+(SafeArithmetic other) const noexcept {
        SafeArithmetic result{};
        if (__builtin_add_overflow(value, other.value, &result.value)) {
            result.overflow = true;
        }
        return result;
    }

    constexpr SafeArithmetic operator-(SafeArithmetic other) const noexcept {
        SafeArithmetic result{};
        if (__builtin_sub_overflow(value, other.value, &result.value)) {
            result.overflow = true;
        }
        return result;
    }

    constexpr SafeArithmetic operator*(SafeArithmetic other) const noexcept {
        SafeArithmetic result{};
        if (__builtin_mul_overflow(value, other.value, &result.value)) {
            result.overflow = true;
        }
        return result;
    }

    constexpr SafeArithmetic operator/(SafeArithmetic other) const noexcept {
        if (other.value == 0) return {0, true};
        return SafeArithmetic(value / other.value);
    }
};

// Overflow detection functions
template<std::integral T>
inline constexpr bool add_overflows(T a, T b) noexcept {
    T result{};
    return __builtin_add_overflow(a, b, &result);
}

template<std::integral T>
inline constexpr bool mul_overflows(T a, T b) noexcept {
    T result{};
    return __builtin_mul_overflow(a, b, &result);
}

// GCD (Greatest Common Divisor)
template<std::integral T>
inline constexpr T gcd(T a, T b) noexcept {
    if (a == 0) return b < 0 ? -b : b;
    if (b == 0) return a < 0 ? -a : a;

    // Make positive
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    while (b != 0) {
        T temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// LCM (Least Common Multiple)
template<std::integral T>
inline constexpr T lcm(T a, T b) noexcept {
    if (a == 0 || b == 0) return 0;
    T g = gcd(a, b);
    return (a / g) * b;  // Avoid overflow by dividing first
}

// Integer power (fast exponentiation)
template<std::integral T>
inline constexpr T ipow(T base, T exp) noexcept {
    if (exp == 0) return 1;
    if (exp < 0) return 0;  // Negative exp not supported for integers

    T result = 1;
    while (exp > 0) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
}

// Factorial
inline constexpr int64_t factorial(int n) noexcept {
    if (n <= 1) return 1;
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All numeric tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/numeric.hpp tests/test_numeric.cpp
git commit -m "feat(util): add safe arithmetic: sat_cast, overflow detection, gcd/lcm/ipow"
```

---

## Task 8: 实现 enum.hpp — 枚举工具

**Files:**
- Create: `include/mana/util/enum.hpp`
- Create: `tests/test_enum.cpp`

**Interfaces:**
- Produces: `MANA_ENUM_REGISTER` 宏，生成 `to_string()`、`from_string()`、`enum_values()`、`enum_names()`

- [ ] **Step 1: 编写测试文件 test_enum.cpp**

```cpp
#include <mana/util/enum.hpp>
#include <cassert>
#include <iostream>

// Define test enum and register it
enum class Color { Red, Green, Blue, Yellow };
MANA_ENUM_REGISTER(Color,
    Entry{Color::Red,    "Red"},
    Entry{Color::Green,  "Green"},
    Entry{Color::Blue,   "Blue"},
    Entry{Color::Yellow, "Yellow"}
)

enum class Direction { North, South, East, West };
MANA_ENUM_REGISTER(Direction,
    Entry{Direction::North, "North"},
    Entry{Direction::South, "South"},
    Entry{Direction::East,  "East"},
    Entry{Direction::West,  "West"}
)

void test_enum_to_string() {
    static_assert(to_string(Color::Red) == "Red");
    static_assert(to_string(Color::Green) == "Green");
    static_assert(to_string(Color::Blue) == "Blue");

    assert(to_string(Color::Red) == "Red");
    assert(to_string(Direction::North) == "North");
}

void test_enum_from_string() {
    static_assert(Color_from_string("Red") == Color::Red);
    static_assert(Color_from_string("Green") == Color::Green);
    static_assert(!Color_from_string("Invalid").has_value());

    assert(Color_from_string("Red") == Color::Red);
    assert(Color_from_string("Invalid") == std::nullopt);
    assert(Direction_from_string("West") == Direction::West);
}

void test_enum_values() {
    auto values = enum_values(Color{});
    assert(values.size() == 4);
    assert(values[0] == Color::Red);
    assert(values[1] == Color::Green);
    assert(values[2] == Color::Blue);
    assert(values[3] == Color::Yellow);
}

void test_enum_names() {
    auto names = enum_names(Color{});
    assert(names.size() == 4);
    assert(names[0] == "Red");
    assert(names[1] == "Green");
    assert(names[2] == "Blue");
    assert(names[3] == "Yellow");
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（enum.hpp 不存在）

- [ ] **Step 3: 实现 enum.hpp**

```cpp
#pragma once

#include <string_view>
#include <optional>
#include <array>
#include <algorithm>

namespace mana::util {

// Helper to generate enum registry and functions
#define MANA_ENUM_REGISTER(EnumType, ...)                                      \
    inline constexpr auto enum_registry_##EnumType() {                         \
        struct Entry { EnumType value; std::string_view name; };               \
        return std::array<Entry, sizeof...( __VA_ARGS__ ) / sizeof(Entry)>     \
            __VA_ARGS__;                                                       \
    }                                                                          \
                                                                               \
    inline constexpr std::string_view to_string(EnumType e) {                  \
        constexpr auto registry = enum_registry_##EnumType();                  \
        for (const auto& entry : registry) {                                   \
            if (entry.value == e) return entry.name;                           \
        }                                                                      \
        return "Unknown";                                                      \
    }                                                                          \
                                                                               \
    inline constexpr std::optional<EnumType> EnumType##_from_string(           \
        std::string_view s) {                                                  \
        constexpr auto registry = enum_registry_##EnumType();                  \
        for (const auto& entry : registry) {                                   \
            if (entry.name == s) return entry.value;                           \
        }                                                                      \
        return std::nullopt;                                                   \
    }                                                                          \
                                                                               \
    inline constexpr auto enum_values(EnumType) {                              \
        constexpr auto registry = enum_registry_##EnumType();                  \
        std::array<EnumType, registry.size()> values{};                        \
        for (size_t i = 0; i < registry.size(); ++i) {                         \
            values[i] = registry[i].value;                                     \
        }                                                                      \
        return values;                                                         \
    }                                                                          \
                                                                               \
    inline constexpr auto enum_names(EnumType) {                               \
        constexpr auto registry = enum_registry_##EnumType();                  \
        std::array<std::string_view, registry.size()> names{};                 \
        for (size_t i = 0; i < registry.size(); ++i) {                         \
            names[i] = registry[i].name;                                       \
        }                                                                      \
        return names;                                                          \
    }

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All enum tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/enum.hpp tests/test_enum.cpp
git commit -m "feat(util): add enum reflection with MANA_ENUM_REGISTER macro"
```

---

## Task 9: 实现 fixed.hpp — 编译期友好容器

**Files:**
- Create: `include/mana/util/fixed.hpp`
- Create: `tests/test_fixed.cpp`

**Interfaces:**
- Produces: `mana::util::fixed_string<N>`, `mana::util::fixed_vector<T,N>`, `mana::util::small_vector<T,N>`

- [ ] **Step 1: 编写测试文件 test_fixed.cpp**

```cpp
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
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（fixed.hpp 不存在）

- [ ] **Step 3: 实现 fixed.hpp**

```cpp
#pragma once

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <string_view>
#include <memory>
#include <initializer_list>
#include <stdexcept>

namespace mana::util {

// Fixed-size string (supports NTTP in C++20)
template<size_t N>
class fixed_string {
public:
    char data_[N]{};
    size_t size_ = 0;

    constexpr fixed_string() = default;

    constexpr fixed_string(const char (&str)[N]) {
        for (size_t i = 0; i < N && str[i] != '\0'; ++i) {
            data_[i] = str[i];
            ++size_;
        }
    }

    constexpr size_t size() const noexcept { return size_; }
    constexpr const char* c_str() const noexcept { return data_; }
    constexpr std::string_view view() const noexcept { return {data_, size_}; }

    constexpr bool operator==(const fixed_string& other) const {
        if (size_ != other.size_) return false;
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i] != other.data_[i]) return false;
        }
        return true;
    }

    constexpr auto operator<=>(const fixed_string& other) const {
        for (size_t i = 0; i < std::min(size_, other.size_); ++i) {
            if (data_[i] != other.data_[i]) {
                return data_[i] <=> other.data_[i];
            }
        }
        return size_ <=> other.size_;
    }

    template<size_t M>
    constexpr fixed_string<N + M - 1> operator+(const fixed_string<M>& other) const {
        fixed_string<N + M - 1> result;
        for (size_t i = 0; i < size_; ++i) {
            result.data_[result.size_++] = data_[i];
        }
        for (size_t i = 0; i < other.size_; ++i) {
            result.data_[result.size_++] = other.data_[i];
        }
        return result;
    }
};

// CTAD deduction guide
template<size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

// Fixed-capacity dynamic array (constexpr-friendly)
template<typename T, size_t N>
class fixed_vector {
public:
    T data_[N]{};
    size_t size_ = 0;

    constexpr fixed_vector() = default;

    constexpr fixed_vector(std::initializer_list<T> init) {
        for (const auto& item : init) {
            push_back(item);
        }
    }

    constexpr void push_back(const T& value) {
        if (size_ >= N) throw std::overflow_error("fixed_vector: capacity exceeded");
        data_[size_++] = value;
    }

    constexpr void push_back(T&& value) {
        if (size_ >= N) throw std::overflow_error("fixed_vector: capacity exceeded");
        data_[size_++] = std::move(value);
    }

    constexpr void pop_back() {
        if (size_ > 0) --size_;
    }

    constexpr void clear() noexcept { size_ = 0; }

    constexpr T& operator[](size_t i) { return data_[i]; }
    constexpr const T& operator[](size_t i) const { return data_[i]; }

    constexpr size_t size() const noexcept { return size_; }
    constexpr size_t capacity() const noexcept { return N; }
    constexpr bool empty() const noexcept { return size_ == 0; }
    constexpr bool full() const noexcept { return size_ == N; }

    constexpr T* begin() noexcept { return data_; }
    constexpr T* end() noexcept { return data_ + size_; }
    constexpr const T* begin() const noexcept { return data_; }
    constexpr const T* end() const noexcept { return data_ + size_; }
};

// Small buffer optimization vector
template<typename T, size_t N>
class small_vector {
    alignas(T) char stack_buf_[N * sizeof(T)]{};
    T* data_ = reinterpret_cast<T*>(stack_buf_);
    size_t size_ = 0;
    size_t capacity_ = N;

    void grow() {
        size_t new_cap = capacity_ * 2;
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        if (data_ != reinterpret_cast<T*>(stack_buf_)) {
            ::operator delete(data_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }

public:
    constexpr small_vector() = default;

    small_vector(const small_vector& other) {
        reserve(other.size_);
        for (size_t i = 0; i < other.size_; ++i) {
            push_back(other.data_[i]);
        }
    }

    small_vector(small_vector&& other) noexcept {
        if (other.data_ == reinterpret_cast<T*>(other.stack_buf_)) {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(std::move(other.data_[i]));
            }
        } else {
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = reinterpret_cast<T*>(other.stack_buf_);
            other.size_ = 0;
            other.capacity_ = N;
        }
    }

    ~small_vector() {
        clear();
        if (data_ != reinterpret_cast<T*>(stack_buf_)) {
            ::operator delete(data_);
        }
    }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity_) return;
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        if (data_ != reinterpret_cast<T*>(stack_buf_)) {
            ::operator delete(data_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) grow();
        new (data_ + size_) T(value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ >= capacity_) grow();
        new (data_ + size_) T(std::move(value));
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }

    size_t size() const noexcept { return size_; }
    size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    bool full() const noexcept { return size_ == capacity_; }

    T* begin() noexcept { return data_; }
    T* end() noexcept { return data_ + size_; }
    const T* begin() const noexcept { return data_; }
    const T* end() const noexcept { return data_ + size_; }
};

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All fixed container tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/fixed.hpp tests/test_fixed.cpp
git commit -m "feat(util): add constexpr containers: fixed_string, fixed_vector, small_vector"
```

---

## Task 10: 实现 optional.hpp — Optional Monadic 扩展

**Files:**
- Create: `include/mana/util/optional.hpp`
- Create: `tests/test_optional.cpp`

**Interfaces:**
- Produces: `mana::util::and_then()`, `mana::util::transform()`, `mana::util::or_else()`, `mana::util::value_or_else()`

- [ ] **Step 1: 编写测试文件 test_optional.cpp**

```cpp
#include <mana/util/optional.hpp>
#include <cassert>
#include <string>
#include <optional>

void test_optional_and_then() {
    auto opt = std::optional<int>(42);
    auto result = mana::util::and_then(std::move(opt), [](int v) -> std::optional<std::string> {
        return std::to_string(v);
    });
    assert(result.has_value());
    assert(result.value() == "42");

    auto empty = std::optional<int>(std::nullopt);
    auto result2 = mana::util::and_then(std::move(empty), [](int v) -> std::optional<std::string> {
        return std::to_string(v);
    });
    assert(!result2.has_value());
}

void test_optional_transform() {
    auto opt = std::optional<int>(10);
    auto result = mana::util::transform(std::move(opt), [](int v) { return v * 2.0; });
    assert(result.has_value());
    assert(result.value() == 20.0);

    auto empty = std::optional<int>(std::nullopt);
    auto result2 = mana::util::transform(std::move(empty), [](int v) { return v * 2.0; });
    assert(!result2.has_value());
}

void test_optional_or_else() {
    auto empty = std::optional<int>(std::nullopt);
    auto result = mana::util::or_else(std::move(empty), []() -> std::optional<int> {
        return 99;
    });
    assert(result.has_value());
    assert(result.value() == 99);

    auto opt = std::optional<int>(42);
    auto result2 = mana::util::or_else(std::move(opt), []() -> std::optional<int> {
        return 99;
    });
    assert(result2.has_value());
    assert(result2.value() == 42);
}

void test_optional_value_or_else() {
    auto empty = std::optional<int>(std::nullopt);
    auto result = mana::util::value_or_else(std::move(empty), []() { return 99; });
    assert(result == 99);

    auto opt = std::optional<int>(42);
    auto result2 = mana::util::value_or_else(std::move(opt), []() { return 99; });
    assert(result2 == 42);
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（optional.hpp 不存在）

- [ ] **Step 3: 实现 optional.hpp**

```cpp
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
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All optional tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/optional.hpp tests/test_optional.cpp
git commit -m "feat(util): add optional monadic extensions: and_then, transform, or_else"
```

---

## Task 11: 实现 variant.hpp — Variant 工具

**Files:**
- Create: `include/mana/util/variant.hpp`
- Create: `tests/test_variant.cpp`

**Interfaces:**
- Produces: `mana::util::overloaded`, `mana::util::match()`

- [ ] **Step 1: 编写测试文件 test_variant.cpp**

```cpp
#include <mana/util/variant.hpp>
#include <cassert>
#include <string>
#include <variant>

void test_variant_overloaded() {
    std::variant<int, double, std::string> v1 = 42;
    std::variant<int, double, std::string> v2 = 3.14;
    std::variant<int, double, std::string> v3 = "hello";

    auto visitor = mana::util::overloaded{
        [](int i) { return i * 2; },
        [](double d) { return d + 1.0; },
        [](const std::string& s) { return static_cast<int>(s.size()); }
    };

    assert(std::visit(visitor, v1) == 84);
    assert(std::visit(visitor, v2) == 4.14);
    assert(std::visit(visitor, v3) == 5);
}

void test_variant_match() {
    std::variant<int, double, std::string> v = "hello world";

    int result = mana::util::match(v,
        [](int i) { return i; },
        [](double d) { return static_cast<int>(d); },
        [](const std::string& s) { return static_cast<int>(s.size()); }
    );

    assert(result == 11);
}
```

- [ ] **Step 2: 运行测试验证失败**

Run: `cmake --build build`
Expected: 编译失败（variant.hpp 不存在）

- [ ] **Step 3: 实现 variant.hpp**

```cpp
#pragma once

#include <variant>
#include <utility>

namespace mana::util {

// Overloaded - combines multiple lambdas into a single visitor
template<typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// CTAD deduction guide
template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// match - type-safe pattern matching syntax sugar
template<typename Variant, typename... Visitors>
constexpr auto match(Variant&& variant, Visitors&&... visitors) {
    return std::visit(
        overloaded{std::forward<Visitors>(visitors)...},
        std::forward<Variant>(variant)
    );
}

} // namespace mana::util
```

- [ ] **Step 4: 运行测试验证通过**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All variant tests passed!

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/variant.hpp tests/test_variant.cpp
git commit -m "feat(util): add variant utilities: overloaded visitor and match()"
```

---

## Task 12: 最终集成验证

**Files:**
- Verify: 所有新增文件已正确集成

**Interfaces:**
- Consumes: 所有前面任务实现的模块

- [ ] **Step 1: 完整构建**

Run: `cmake -B build -S . && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 2: 运行所有测试**

Run: `ctest --test-dir build -C Debug`
Expected: All tests passed!

- [ ] **Step 3: 验证 mana.h 包含所有新模块**

```cpp
#include <mana/mana.h>
// 应该能使用所有新模块
static_assert(mana::util::is_little_endian);
```

- [ ] **Step 4: 最终 Commit（如果需要）**

```bash
git add -A
git commit -m "feat: complete util enrichment with 10 new modules"
```

---

## Self-Review Checklist

- ✅ Spec coverage: 所有 10 个模块都有对应的 Task
- ✅ Placeholder scan: 无 TBD/TODO/占位符
- ✅ Type consistency: 所有函数签名、类型名称在各 Task 间一致
- ✅ Test coverage: 每个模块都有完整的测试用例
- ✅ Build integration: mana.h 和 CMakeLists.txt 已更新
