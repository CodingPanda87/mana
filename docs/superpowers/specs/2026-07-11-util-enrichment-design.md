# Util 模块扩充设计

> 日期：2026-07-11
> 状态：已确认
> 方案：全量覆盖（方案 B）

## 1. 目标

为 Mana 框架的 `util` 模块补充高级 C++ 开发中常用的工具类和方法，全部采用 `constexpr` / `inline` header-only 模式，充分利用 C++20/23 特性（concepts、requires、if consteval 等）。

## 2. 新增文件清单

| 文件 | 命名空间 | 核心内容 |
|------|----------|----------|
| `result.hpp` | `mana::util` | `Result<T,E>` 无异常错误处理 + monadic 链式操作 |
| `scope_guard.hpp` | `mana::util` | `ScopeGuard` / `defer` / `ScopeSuccess` / `ScopeFailure` |
| `enum.hpp` | `mana::util` | 枚举 ↔ 字符串转换（宏注册） |
| `hash.hpp` | `mana::util` | 编译期 FNV-1a、CRC32、hash_combine |
| `type_traits.hpp` | `mana::util` | Container concept、is_specialization_of、TypeList |
| `bit.hpp` | `mana::util` | popcount、countl_zero、bit_ceil/floor、byteswap、endian |
| `numeric.hpp` | `mana::util` | sat_cast、SafeArithmetic、gcd/lcm/ipow/factorial |
| `fixed.hpp` | `mana::util` | `fixed_string<N>`、`fixed_vector<T,N>`、`small_vector<T,N>` |
| `optional.hpp` | `mana::util` | and_then、transform、or_else、value_or_else |
| `variant.hpp` | `mana::util` | `overloaded`、`match()` |

## 3. 各模块详细设计

### 3.1 result.hpp — Result<T, E>

无异常错误处理类型，支持 monadic 链式操作。

```cpp
namespace mana::util {

template<typename T>
concept IsResult = requires { typename T::value_type; typename T::error_type; };

template<typename T, typename E>
class Result {
public:
    // 构造
    constexpr static Result ok(T value);
    constexpr static Result err(E error);

    // 查询
    constexpr bool is_ok() const noexcept;
    constexpr bool is_err() const noexcept;
    constexpr explicit operator bool() const noexcept;

    // 访问
    constexpr const T& value() const&;
    constexpr T& value() &;
    constexpr const E& error() const&;
    constexpr T value_or(T default_value) const&;

    // Monadic 链式操作
    template<typename F> constexpr auto and_then(F&& func) const;   // T -> Result<U,E>
    template<typename F> constexpr auto transform(F&& func) const;  // T -> U
    template<typename F> constexpr auto or_else(F&& func) const;    // E -> Result<T,E2>

    // 组合：两个都 ok 返回 pair
    template<typename T2>
    constexpr auto operator&(const Result<T2, E>& other) const;
};

} // namespace mana::util
```

**设计要点**：
- 内部使用 `std::variant<T, E>` 存储
- 如果编译器支持 `<expected>`（C++23），通过 `#if __has_include(<expected>)` 切换为 `std::expected`
- `and_then` 用于链式操作：`read_file().and_then(parse).and_then(validate)`
- `transform` 用于值变换，`or_else` 用于错误恢复
- `operator&` 用于组合多个 Result

### 3.2 scope_guard.hpp — 作用域守卫

```cpp
namespace mana::util {

template<typename F>
class ScopeGuard {
public:
    constexpr explicit ScopeGuard(F&& func) noexcept;
    constexpr ~ScopeGuard() noexcept;  // 执行 func，吞掉异常
    constexpr void dismiss() noexcept;

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

template<typename F>
constexpr ScopeGuard<F> defer(F&& func) noexcept;

template<typename F>
class ScopeSuccess { /* 析构时无异常才触发 */ };

template<typename F>
class ScopeFailure { /* 析构时正在 unwind 才触发 */ };

} // namespace mana::util
```

**设计要点**：
- `defer` 类似 Go 的 defer，最常用
- `ScopeSuccess` / `ScopeFailure` 通过 `std::uncaught_exceptions()` 判断
- 析构函数 `noexcept` 并吞掉异常
- `dismiss()` 提前取消执行

### 3.3 enum.hpp — 枚举工具

```cpp
namespace mana::util {

// 注册宏
#define MANA_ENUM_REGISTER(EnumType, ...)                                      \
    inline constexpr auto enum_registry_##EnumType() {                         \
        struct Entry { EnumType value; std::string_view name; };               \
        return std::array<Entry, ...>{ __VA_ARGS__ };                          \
    }                                                                          \
    inline constexpr std::string_view to_string(EnumType e) { /* 查表 */ }     \
    inline constexpr std::optional<EnumType> EnumType##_from_string(           \
        std::string_view s) { /* 查表 */ }                                     \
    inline constexpr auto enum_values(EnumType) { /* 返回所有值 */ }           \
    inline constexpr auto enum_names(EnumType) { /* 返回所有名称 */ }

} // namespace mana::util
```

**设计要点**：
- 注册数据为 `constexpr std::array`，编译期可查
- 使用 `std::string_view` 避免堆分配
- 函数名自动生成：`color_from_string` 避免歧义
- 不侵入枚举定义本身

### 3.4 hash.hpp — 编译期哈希

```cpp
namespace mana::util {

// FNV-1a — 通用字符串哈希
inline constexpr uint64_t fnv1a(std::string_view str) noexcept;

// CRC32 — 校验场景
inline constexpr uint32_t crc32(std::string_view str) noexcept;

// 组合多个哈希值
template<typename... Ts>
constexpr size_t hash_combine(const Ts&... values) noexcept;

} // namespace mana::util
```

**设计要点**：
- 全部 `constexpr`，可在编译期执行
- CRC32 使用 constexpr lambda 生成查找表
- `fnv1a` 返回值可用于 switch-case 编译期字符串匹配

### 3.5 type_traits.hpp — 类型萃取辅助

```cpp
namespace mana::util {

// 容器概念
template<typename T>
concept Container = requires(T t) {
    std::begin(t); std::end(t); std::size(t);
    typename T::value_type;
};

// Tuple-like 概念
template<typename T>
concept TupleLike = requires {
    std::tuple_size<std::remove_cvref_t<T>>::value;
};

// 模板特化判断
template<typename T, template<typename...> class Template>
struct is_specialization_of : std::false_type {};

template<template<typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

template<typename T, template<typename...> class Template>
inline constexpr bool is_specialization_of_v = is_specialization_of<T, Template>::value;

// 类型列表
template<typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);
    template<typename T> static constexpr bool contains = (std::is_same_v<T, Ts> || ...);
    template<template<typename> class F> using transform = TypeList<F<Ts>...>;
};

// 类型索引
template<typename T, typename... Ts>
inline constexpr size_t index_of = /* 编译期查找 */;

template<size_t N, typename... Ts>
using nth_type = std::tuple_element_t<N, std::tuple<Ts...>>;

} // namespace mana::util
```

### 3.6 bit.hpp — 位操作

```cpp
namespace mana::util {

template<std::unsigned_integral T>
inline constexpr int popcount(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr int countl_zero(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr int countr_zero(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr T bit_ceil(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr T bit_floor(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr bool has_single_bit(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr T byteswap(T x) noexcept;

inline constexpr bool is_little_endian = (std::endian::native == std::endian::little);
inline constexpr bool is_big_endian = (std::endian::native == std::endian::big);

template<std::unsigned_integral T>
inline constexpr T to_le(T x) noexcept;

template<std::unsigned_integral T>
inline constexpr T to_be(T x) noexcept;

} // namespace mana::util
```

**设计要点**：
- 优先使用 C++20 `<bit>` 标准库版本，不可用时提供 fallback
- 全部 `constexpr`
- `to_le` / `to_be` 在对应字节序系统上为 no-op

### 3.7 numeric.hpp — 安全算术

```cpp
namespace mana::util {

// 饱和转换
template<std::integral To, std::integral From>
inline constexpr To sat_cast(From value) noexcept;

// 溢出安全算术
template<std::integral T>
struct SafeArithmetic {
    T value;
    bool overflow = false;
    constexpr SafeArithmetic operator+(SafeArithmetic other) const noexcept;
    constexpr SafeArithmetic operator-(SafeArithmetic other) const noexcept;
    constexpr SafeArithmetic operator*(SafeArithmetic other) const noexcept;
    constexpr SafeArithmetic operator/(SafeArithmetic other) const noexcept;
};

// 溢出检测
template<std::integral T>
inline constexpr bool add_overflows(T a, T b) noexcept;

template<std::integral T>
inline constexpr bool mul_overflows(T a, T b) noexcept;

// 数学函数
template<std::integral T> inline constexpr T gcd(T a, T b) noexcept;
template<std::integral T> inline constexpr T lcm(T a, T b) noexcept;
template<std::integral T> inline constexpr T ipow(T base, T exp) noexcept;
inline constexpr int64_t factorial(int n) noexcept;

} // namespace mana::util
```

### 3.8 fixed.hpp — 编译期友好容器

```cpp
namespace mana::util {

// 编译期字符串（支持作为 NTTP）
template<size_t N>
class fixed_string {
    char data_[N]{};
    size_t size_ = 0;
public:
    constexpr fixed_string() = default;
    constexpr fixed_string(const char (&str)[N]);
    constexpr size_t size() const noexcept;
    constexpr const char* c_str() const noexcept;
    constexpr std::string_view view() const noexcept;
    constexpr bool operator==(const fixed_string&) const = default;
    constexpr auto operator<=>(const fixed_string&) const = default;
    template<size_t M>
    constexpr fixed_string<N+M-1> operator+(const fixed_string<M>& other) const;
};

template<size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

// 编译期固定容量动态数组
template<typename T, size_t N>
class fixed_vector {
    T data_[N]{};
    size_t size_ = 0;
public:
    constexpr fixed_vector() = default;
    constexpr fixed_vector(std::initializer_list<T> init);
    constexpr void push_back(const T& value);
    constexpr void push_back(T&& value);
    constexpr void pop_back();
    constexpr void clear() noexcept;
    constexpr T& operator[](size_t i);
    constexpr const T& operator[](size_t i) const;
    constexpr size_t size() const noexcept;
    constexpr size_t capacity() const noexcept;
    constexpr bool empty() const noexcept;
    constexpr bool full() const noexcept;
    constexpr T* begin() noexcept;
    constexpr T* end() noexcept;
    constexpr const T* begin() const noexcept;
    constexpr const T* end() const noexcept;
};

// 小缓冲区优化容器
template<typename T, size_t N>
class small_vector {
    T stack_[N]{};
    T* data_ = stack_;
    size_t size_ = 0;
    size_t capacity_ = N;
public:
    constexpr small_vector() = default;
    ~small_vector();
    constexpr void push_back(const T& value);  // 超出 N 自动转堆
    // ... 与 fixed_vector 相同的接口
};

} // namespace mana::util
```

### 3.9 optional.hpp — Optional Monadic 扩展

```cpp
namespace mana::util {

template<typename T>
constexpr auto and_then(std::optional<T>&& opt, auto&& func);
// func: T -> std::optional<U>

template<typename T>
constexpr auto transform(std::optional<T>&& opt, auto&& func);
// func: T -> U

template<typename T>
constexpr auto or_else(std::optional<T>&& opt, auto&& func);
// func: () -> std::optional<T>

template<typename T>
constexpr T value_or_else(std::optional<T>&& opt, auto&& func);
// func: () -> T（lazy 默认值）

} // namespace mana::util
```

**设计要点**：
- 自由函数而非成员函数，不修改 std::optional
- C++23 已有部分成员函数版本，这里提供 C++20 兼容的自由函数版

### 3.10 variant.hpp — Variant 工具

```cpp
namespace mana::util {

// 多 lambda 访问器合并
template<typename... Ts>
struct overloaded : Ts... { using Ts::operator()... };

template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// 模式匹配语法糖
template<typename Variant, typename... Visitors>
constexpr auto match(Variant&& variant, Visitors&&... visitors) {
    return std::visit(overloaded{std::forward<Visitors>(visitors)...},
                      std::forward<Variant>(variant));
}

} // namespace mana::util
```

## 4. 测试策略

每个模块对应 `tests/test_<module>.cpp`，共 10 个新测试文件：

| 测试文件 | 覆盖内容 |
|----------|----------|
| `test_result.cpp` | ok/err 构造、monadic 链、operator& 组合、边界条件 |
| `test_scope_guard.cpp` | defer 执行、dismiss 取消、ScopeSuccess/Failure |
| `test_enum.cpp` | to_string、from_string、enum_values/names |
| `test_hash.cpp` | fnv1a、crc32 编译期验证、hash_combine |
| `test_type_traits.cpp` | Container concept、is_specialization_of、TypeList |
| `test_bit.cpp` | popcount、bit_ceil/floor、byteswap、endian |
| `test_numeric.cpp` | sat_cast、SafeArithmetic、gcd/lcm/ipow |
| `test_fixed.cpp` | fixed_string NTTP、fixed_vector constexpr、small_vector |
| `test_optional.cpp` | and_then、transform、or_else |
| `test_variant.cpp` | overloaded、match |

**测试原则**：
- 优先使用 `static_assert` 验证编译期行为
- 运行时测试验证动态场景（如 small_vector 堆转栈、Result 异常路径）
- 边界条件：空值、溢出、零除、最大/最小值

## 5. 集成方式

### 5.1 mana.h

```cpp
// 新增 include
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

### 5.2 CMakeLists.txt

```cmake
add_executable(mana_tests
    # 现有测试...
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
)
```

## 6. 设计原则

1. **全部 header-only**：与现有 util 模块风格一致，零链接依赖
2. **constexpr 优先**：尽可能在编译期执行，减少运行时开销
3. **C++20/23 特性**：充分利用 concepts、requires、if consteval、std::endian 等
4. **向后兼容**：不修改现有模块，仅新增文件
5. **独立可测试**：每个文件独立 include，独立测试
6. **命名一致**：遵循 `mana::util::<module>` 命名规范
