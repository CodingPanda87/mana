# Util 模块重构实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将现有的 `mana::util` 工具模块重构为分类组织的单头文件库，采用纯 header-only 实现，优先使用 `constexpr` 和 `inline`。

**Architecture:** 纯 header-only 实现，所有函数标记为 `inline` 或 `constexpr`。按功能域组织到 `include/mana/util/` 目录下的独立头文件。移除原有的 `.cpp` 实现文件。

**Tech Stack:** C++23, MSVC 2019 16.10+, Windows, Standard Library only

## Global Constraints

- **Compiler:** MSVC 2019 16.10+ (C++20 std::format and concepts)
- **Standard:** C++23
- **Platform:** Windows
- **Dependencies:** C++23 Standard Library only
- **Naming:** Full English words, no abbreviations (AI-friendly)
- **Namespaces:** `mana::util::module_name` structure
- **Implementation:** Pure header-only, no .cpp files
- **Keywords:** `inline` for all functions, `constexpr` where possible

---

## File Structure

```
include/mana/
├── util/
│   ├── string.hpp    # 字符串操作
│   ├── io.hpp        # 文件工具（无状态）
│   ├── util.hpp      # 通用工具
│   ├── math.hpp      # 数学函数
│   └── geo.hpp       # 几何/地理
├── mana.h            # 更新 include
tests/
├── test_string.cpp   # 字符串测试
├── test_io.cpp       # 文件工具测试
├── test_util.cpp     # 通用工具测试
├── test_math.cpp     # 数学测试
└── test_geo.cpp      # 几何/地理测试
```

---

### Task 1: 字符串模块 (string.hpp)

**Files:**
- Create: `include/mana/util/string.hpp`
- Create: `tests/test_string.cpp`
- Delete: `include/mana/util.h` (旧头文件)
- Delete: `src/util.cpp` (旧实现)

**Interfaces:**
- Produces: `mana::util::string::split()`, `trim()`, `to_lower()`, `to_upper()`, `starts_with()`, `ends_with()`, `contains()`, `replace()`, `join()`

- [ ] **Step 1: Create string.hpp**

```cpp
#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <sstream>

namespace mana::util::string {

inline std::vector<std::string> split(std::string_view str, std::string_view delimiter) {
    std::vector<std::string> result;
    if (delimiter.empty()) {
        result.emplace_back(str);
        return result;
    }
    if (str.empty()) {
        result.emplace_back("");
        return result;
    }

    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    result.emplace_back(str.substr(start));
    return result;
}

inline std::string trim(std::string_view str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }
    if (start == str.end()) {
        return "";
    }
    auto end = str.end() - 1;
    while (end != start && std::isspace(static_cast<unsigned char>(*end))) {
        --end;
    }
    return std::string(start, end + 1);
}

inline std::string to_lower(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

inline std::string to_upper(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

inline bool starts_with(std::string_view str, std::string_view prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

inline bool ends_with(std::string_view str, std::string_view suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

inline bool contains(std::string_view str, std::string_view sub) {
    return str.find(sub) != std::string_view::npos;
}

inline std::string replace(std::string_view str, std::string_view from, std::string_view to) {
    if (from.empty()) {
        return std::string(str);
    }
    std::string result(str);
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

inline std::string join(const std::vector<std::string>& parts, std::string_view delimiter) {
    if (parts.empty()) {
        return "";
    }
    std::ostringstream oss;
    oss << parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        oss << delimiter << parts[i];
    }
    return oss.str();
}

} // namespace mana::util::string
```

- [ ] **Step 2: Create test_string.cpp**

```cpp
#include <mana/util/string.hpp>
#include <cassert>
#include <iostream>

void test_split() {
    auto result = mana::util::string::split("hello,world,foo", ",");
    assert(result.size() == 3);
    assert(result[0] == "hello");
    assert(result[1] == "world");
    assert(result[2] == "foo");
}

void test_split_empty() {
    auto result = mana::util::string::split("", ",");
    assert(result.size() == 1);
    assert(result[0] == "");
}

void test_split_empty_delimiter() {
    auto result = mana::util::string::split("hello", "");
    assert(result.size() == 1);
    assert(result[0] == "hello");
}

void test_trim() {
    assert(mana::util::string::trim("  hello  ") == "hello");
    assert(mana::util::string::trim("hello") == "hello");
    assert(mana::util::string::trim("  ") == "");
    assert(mana::util::string::trim("") == "");
}

void test_to_lower() {
    assert(mana::util::string::to_lower("HELLO") == "hello");
    assert(mana::util::string::to_lower("Hello") == "hello");
}

void test_to_upper() {
    assert(mana::util::string::to_upper("hello") == "HELLO");
    assert(mana::util::string::to_upper("Hello") == "HELLO");
}

void test_starts_with() {
    assert(mana::util::string::starts_with("hello world", "hello") == true);
    assert(mana::util::string::starts_with("hello world", "world") == false);
}

void test_ends_with() {
    assert(mana::util::string::ends_with("hello world", "world") == true);
    assert(mana::util::string::ends_with("hello world", "hello") == false);
}

void test_contains() {
    assert(mana::util::string::contains("hello world", "lo wo") == true);
    assert(mana::util::string::contains("hello world", "xyz") == false);
}

void test_replace() {
    assert(mana::util::string::replace("hello world", "world", "cpp") == "hello cpp");
    assert(mana::util::string::replace("aaa", "a", "b") == "bbb");
}

void test_join() {
    std::vector<std::string> parts = {"hello", "world", "foo"};
    assert(mana::util::string::join(parts, ",") == "hello,world,foo");
    assert(mana::util::string::join({}, ",") == "");
}

int main() {
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
    return 0;
}
```

- [ ] **Step 3: Build and run tests**

Run: `cmake --build build && ./build/Debug/mana_tests.exe`
Expected: All string tests passed!

- [ ] **Step 4: Delete old util files**

```bash
rm include/mana/util.h
rm src/util.cpp
```

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/string.hpp tests/test_string.cpp
git rm include/mana/util.h src/util.cpp
git commit -m "refactor: add string.hpp and remove old util module"
```

---

### Task 2: 文件工具模块 (io.hpp)

**Files:**
- Refactor: `include/mana/util/io.hpp`
- Create: `tests/test_io_util.cpp`
- Delete: `include/mana/io.h` (旧 IO 模块)
- Delete: `src/io.cpp` (旧实现)

**Interfaces:**
- Produces: `mana::util::io::read_all()`, `write()`, `append()`, `exists()`, `file_size()`, `read_lines()`

- [ ] **Step 1: Refactor io.hpp**

```cpp
#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <fstream>
#include <filesystem>

namespace mana::util::io {

inline std::optional<std::string> read_all(std::string_view path) {
    std::ifstream file(std::string(path), std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size < 0) {
        return std::nullopt;
    }
    file.seekg(0, std::ios::beg);
    std::string content(static_cast<size_t>(size), '\0');
    file.read(content.data(), size);
    return content;
}

inline bool write(std::string_view path, std::string_view content) {
    std::ofstream file(std::string(path), std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(content.data(), static_cast<std::streamsize>(content.size()));
    return file.good();
}

inline bool append(std::string_view path, std::string_view content) {
    std::ofstream file(std::string(path), std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    file.write(content.data(), static_cast<std::streamsize>(content.size()));
    return file.good();
}

inline bool exists(std::string_view path) {
    return std::filesystem::exists(path);
}

inline std::optional<size_t> file_size(std::string_view path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(std::string(path), ec);
    if (ec) {
        return std::nullopt;
    }
    return size;
}

inline std::vector<std::string> read_lines(std::string_view path) {
    std::vector<std::string> lines;
    std::ifstream file(std::string(path));
    if (!file.is_open()) {
        return lines;
    }
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(std::move(line));
    }
    return lines;
}

} // namespace mana::util::io
```

- [ ] **Step 2: Create test_io_util.cpp**

```cpp
#include <mana/util/io.hpp>
#include <cassert>
#include <iostream>
#include <filesystem>

void test_write_and_read() {
    std::string test_path = "test_io_util.txt";
    std::string content = "Hello, World!";

    assert(mana::util::io::write(test_path, content) == true);

    auto result = mana::util::io::read_all(test_path);
    assert(result.has_value());
    assert(result.value() == content);

    std::filesystem::remove(test_path);
}

void test_exists() {
    std::string test_path = "test_io_exists.txt";

    assert(mana::util::io::exists(test_path) == false);

    std::ofstream file(test_path);
    file << "test";
    file.close();

    assert(mana::util::io::exists(test_path) == true);

    std::filesystem::remove(test_path);
}

void test_file_size() {
    std::string test_path = "test_io_size.txt";
    std::string content = "Hello";

    mana::util::io::write(test_path, content);

    auto size = mana::util::io::file_size(test_path);
    assert(size.has_value());
    assert(size.value() == content.length());

    std::filesystem::remove(test_path);
}

void test_read_nonexistent() {
    auto result = mana::util::io::read_all("nonexistent_io.txt");
    assert(result.has_value() == false);
}

void test_append() {
    std::string test_path = "test_io_append.txt";

    mana::util::io::write(test_path, "Hello");
    mana::util::io::append(test_path, " World");

    auto result = mana::util::io::read_all(test_path);
    assert(result.has_value());
    assert(result.value() == "Hello World");

    std::filesystem::remove(test_path);
}

void test_read_lines() {
    std::string test_path = "test_io_lines.txt";

    mana::util::io::write(test_path, "line1\nline2\nline3");

    auto lines = mana::util::io::read_lines(test_path);
    assert(lines.size() == 3);
    assert(lines[0] == "line1");
    assert(lines[1] == "line2");
    assert(lines[2] == "line3");

    std::filesystem::remove(test_path);
}

int main() {
    test_write_and_read();
    test_exists();
    test_file_size();
    test_read_nonexistent();
    test_append();
    test_read_lines();
    std::cout << "All IO util tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 3: Build and run tests**

Run: `cmake --build build && ./build/Debug/mana_tests.exe`
Expected: All IO util tests passed!

- [ ] **Step 4: Delete old IO module**

```bash
rm include/mana/io.h
rm src/io.cpp
rm tests/test_io.cpp
```

- [ ] **Step 5: Commit**

```bash
git add include/mana/util/io.hpp tests/test_io_util.cpp
git rm include/mana/io.h src/io.cpp tests/test_io.cpp
git commit -m "refactor: add io.hpp utility and remove old IO module"
```

---

### Task 3: 通用工具模块 (util.hpp)

**Files:**
- Create: `include/mana/util/util.hpp`
- Update: `tests/test_util.cpp`

**Interfaces:**
- Produces: `mana::util::to_string()`, `from_string()`, `contains()`, `remove()`, `filter()`, `map()`, `timestamp_ms()`, `timestamp_s()`, `timestamp_iso8601()`, `in_range()`, `to_hex()`, `from_hex()`

- [ ] **Step 1: Create util.hpp**

```cpp
#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <optional>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <format>

namespace mana::util {

// Type conversion
template<typename T>
inline std::string to_string(T value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else {
        return std::to_string(value);
    }
}

template<typename T>
inline std::optional<T> from_string(std::string_view str) {
    T value{};
    std::istringstream iss(std::string(str));
    iss >> value;
    if (iss.fail()) {
        return std::nullopt;
    }
    return value;
}

// Container operations
template<typename T>
inline bool contains(const std::vector<T>& vec, const T& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

template<typename T>
inline void remove(std::vector<T>& vec, const T& value) {
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

template<typename T>
inline std::vector<T> filter(const std::vector<T>& vec, auto predicate) {
    std::vector<T> result;
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), predicate);
    return result;
}

template<typename T, typename F>
inline auto map(const std::vector<T>& vec, F func) -> std::vector<decltype(func(std::declval<T>()))> {
    std::vector<decltype(func(std::declval<T>()))> result;
    result.reserve(vec.size());
    for (const auto& item : vec) {
        result.push_back(func(item));
    }
    return result;
}

// Timestamps
inline int64_t timestamp_ms() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

inline int64_t timestamp_s() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

inline std::string timestamp_iso8601() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    return oss.str();
}

// Range check
template<typename T>
inline constexpr bool in_range(T value, T min, T max) {
    return value >= min && value <= max;
}

// Byte operations
inline std::string to_hex(std::string_view data) {
    std::string result;
    result.reserve(data.size() * 2);
    for (unsigned char c : data) {
        result += "0123456789abcdef"[c >> 4];
        result += "0123456789abcdef"[c & 0x0f];
    }
    return result;
}

inline std::optional<std::string> from_hex(std::string_view hex) {
    if (hex.length() % 2 != 0) {
        return std::nullopt;
    }
    std::string result;
    result.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        auto byte_str = hex.substr(i, 2);
        char* end = nullptr;
        auto byte = static_cast<unsigned char>(std::strtol(std::string(byte_str).c_str(), &end, 16));
        if (end != byte_str.data() + 2) {
            return std::nullopt;
        }
        result += static_cast<char>(byte);
    }
    return result;
}

} // namespace mana::util
```

- [ ] **Step 2: Update test_util.cpp**

```cpp
#include <mana/util/util.hpp>
#include <cassert>
#include <iostream>

void test_to_string() {
    assert(mana::util::to_string(42) == "42");
    assert(mana::util::to_string(3.14) == "3.140000");
    assert(mana::util::to_string(true) == "true");
    assert(mana::util::to_string(std::string("hello")) == "hello");
}

void test_from_string() {
    auto val1 = mana::util::from_string<int>("42");
    assert(val1.has_value());
    assert(val1.value() == 42);

    auto val2 = mana::util::from_string<double>("3.14");
    assert(val2.has_value());
    assert(val2.value() == 3.14);

    auto val3 = mana::util::from_string<int>("not_a_number");
    assert(!val3.has_value());
}

void test_contains() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    assert(mana::util::contains(vec, 3) == true);
    assert(mana::util::contains(vec, 6) == false);
}

void test_remove() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    mana::util::remove(vec, 3);
    assert(vec.size() == 4);
    assert(!mana::util::contains(vec, 3));
}

void test_filter() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};
    auto even = mana::util::filter(vec, [](int x) { return x % 2 == 0; });
    assert(even.size() == 3);
    assert(even[0] == 2);
    assert(even[1] == 4);
    assert(even[2] == 6);
}

void test_map() {
    std::vector<int> vec = {1, 2, 3};
    auto doubled = mana::util::map(vec, [](int x) { return x * 2; });
    assert(doubled.size() == 3);
    assert(doubled[0] == 2);
    assert(doubled[1] == 4);
    assert(doubled[2] == 6);
}

void test_timestamp() {
    auto ms = mana::util::timestamp_ms();
    auto s = mana::util::timestamp_s();
    assert(ms > 0);
    assert(s > 0);
    assert(ms >= s * 1000);
}

void test_timestamp_iso8601() {
    auto ts = mana::util::timestamp_iso8601();
    assert(ts.length() == 24); // "2026-07-10T12:00:00.000Z"
    assert(ts.back() == 'Z');
}

void test_in_range() {
    assert(mana::util::in_range(5, 1, 10) == true);
    assert(mana::util::in_range(0, 1, 10) == false);
    assert(mana::util::in_range(1, 1, 10) == true);
    assert(mana::util::in_range(10, 1, 10) == true);
}

void test_hex() {
    std::string data = "Hello";
    auto hex = mana::util::to_hex(data);
    assert(hex == "48656c6c6f");

    auto decoded = mana::util::from_hex(hex);
    assert(decoded.has_value());
    assert(decoded.value() == data);
}

void test_from_hex_invalid() {
    auto result = mana::util::from_hex("invalid");
    assert(!result.has_value());
}

int main() {
    test_to_string();
    test_from_string();
    test_contains();
    test_remove();
    test_filter();
    test_map();
    test_timestamp();
    test_timestamp_iso8601();
    test_in_range();
    test_hex();
    test_from_hex_invalid();
    std::cout << "All util tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 3: Build and run tests**

Run: `cmake --build build && ./build/Debug/mana_tests.exe`
Expected: All util tests passed!

- [ ] **Step 4: Commit**

```bash
git add include/mana/util/util.hpp tests/test_util.cpp
git commit -m "refactor: add util.hpp with type conversion and container operations"
```

---

### Task 4: 数学模块 (math.hpp)

**Files:**
- Create: `include/mana/util/math.hpp`
- Create: `tests/test_math.cpp`

**Interfaces:**
- Produces: `mana::util::math::clamp()`, `lerp()`, `abs()`, `min()`, `max()`, `to_radians()`, `to_degrees()`, `sin()`, `cos()`, `tan()`, `sqrt()`, constants `pi`, `e`, `epsilon`

- [ ] **Step 1: Create math.hpp**

```cpp
#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace mana::util::math {

// Constants
inline constexpr double pi = 3.14159265358979323846;
inline constexpr double e  = 2.71828182845904523536;
inline constexpr double epsilon = std::numeric_limits<double>::epsilon();

// Basic constexpr functions
template<typename T>
inline constexpr T clamp(T value, T min_val, T max_val) {
    return (value < min_val) ? min_val : (value > max_val) ? max_val : value;
}

template<typename T>
inline constexpr T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

template<typename T>
inline constexpr T abs(T value) {
    return (value < 0) ? -value : value;
}

template<typename T>
inline constexpr T min(T a, T b) {
    return (a < b) ? a : b;
}

template<typename T>
inline constexpr T max(T a, T b) {
    return (a > b) ? a : b;
}

// Degree/radian conversion
inline constexpr double to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline constexpr double to_degrees(double radians) {
    return radians * 180.0 / pi;
}

// Constexpr trigonometric functions (Taylor series approximation)
namespace detail {
    inline constexpr double factorial(int n) {
        double result = 1.0;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    }

    inline constexpr double power(double base, int exp) {
        double result = 1.0;
        for (int i = 0; i < exp; ++i) {
            result *= base;
        }
        return result;
    }

    inline constexpr double normalize_angle(double x) {
        // Normalize to [-pi, pi]
        while (x > pi) x -= 2 * pi;
        while (x < -pi) x += 2 * pi;
        return x;
    }
} // namespace detail

inline constexpr double sin(double x) {
    x = detail::normalize_angle(x);
    double result = 0.0;
    double term = x;
    for (int n = 0; n < 15; ++n) {
        result += term;
        term *= -x * x / ((2 * n + 2) * (2 * n + 3));
    }
    return result;
}

inline constexpr double cos(double x) {
    return sin(x + pi / 2.0);
}

inline constexpr double tan(double x) {
    double c = cos(x);
    if (detail::abs(c) < epsilon) {
        return (sin(x) > 0) ? 1e308 : -1e308;
    }
    return sin(x) / c;
}

inline constexpr double sqrt(double x) {
    if (x < 0) return 0;
    if (x == 0) return 0;

    double guess = x / 2.0;
    for (int i = 0; i < 100; ++i) {
        guess = (guess + x / guess) / 2.0;
    }
    return guess;
}

} // namespace mana::util::math
```

- [ ] **Step 2: Create test_math.cpp**

```cpp
#include <mana/util/math.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

void test_clamp() {
    assert(mana::util::math::clamp(5, 1, 10) == 5);
    assert(mana::util::math::clamp(0, 1, 10) == 1);
    assert(mana::util::math::clamp(15, 1, 10) == 10);
}

void test_lerp() {
    assert(mana::util::math::lerp(0.0, 10.0, 0.5) == 5.0);
    assert(mana::util::math::lerp(0.0, 10.0, 0.0) == 0.0);
    assert(mana::util::math::lerp(0.0, 10.0, 1.0) == 10.0);
}

void test_abs() {
    assert(mana::util::math::abs(5) == 5);
    assert(mana::util::math::abs(-5) == 5);
    assert(mana::util::math::abs(0) == 0);
}

void test_min_max() {
    assert(mana::util::math::min(3, 5) == 3);
    assert(mana::util::math::min(5, 3) == 3);
    assert(mana::util::math::max(3, 5) == 5);
    assert(mana::util::math::max(5, 3) == 5);
}

void test_radians_degrees() {
    assert(std::abs(mana::util::math::to_radians(180.0) - mana::util::math::pi) < 1e-10);
    assert(std::abs(mana::util::math::to_degrees(mana::util::math::pi) - 180.0) < 1e-10);
}

void test_sin() {
    assert(std::abs(mana::util::math::sin(0.0) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::sin(mana::util::math::pi / 2) - 1.0) < 1e-10);
    assert(std::abs(mana::util::math::sin(mana::util::math::pi) - 0.0) < 1e-10);
}

void test_cos() {
    assert(std::abs(mana::util::math::cos(0.0) - 1.0) < 1e-10);
    assert(std::abs(mana::util::math::cos(mana::util::math::pi / 2) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::cos(mana::util::math::pi) - (-1.0)) < 1e-10);
}

void test_tan() {
    assert(std::abs(mana::util::math::tan(0.0) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::tan(mana::util::math::pi / 4) - 1.0) < 1e-10);
}

void test_sqrt() {
    assert(std::abs(mana::util::math::sqrt(0.0) - 0.0) < 1e-10);
    assert(std::abs(mana::util::math::sqrt(1.0) - 1.0) < 1e-10);
    assert(std::abs(mana::util::math::sqrt(4.0) - 2.0) < 1e-10);
    assert(std::abs(mana::util::math::sqrt(9.0) - 3.0) < 1e-10);
}

int main() {
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
    return 0;
}
```

- [ ] **Step 3: Build and run tests**

Run: `cmake --build build && ./build/Debug/mana_tests.exe`
Expected: All math tests passed!

- [ ] **Step 4: Commit**

```bash
git add include/mana/util/math.hpp tests/test_math.cpp
git commit -m "feat: add math.hpp with constexpr trigonometric functions"
```

---

### Task 5: 几何/地理模块 (geo.hpp)

**Files:**
- Create: `include/mana/util/geo.hpp`
- Create: `tests/test_geo.cpp`

**Interfaces:**
- Produces: `mana::util::geo::Point`, `Rect`, `Circle`, `Segment`, `GeoPoint`, `haversine_distance()`, `bearing()`, `destination()`, `convert()`, `QuadTree`

- [ ] **Step 1: Create geo.hpp**

```cpp
#pragma once

#include <cmath>
#include <vector>
#include <optional>
#include <memory>
#include <algorithm>
#include <limits>

namespace mana::util::geo {

// Constants
inline constexpr double earth_radius_m = 6371000.0;
inline constexpr double pi = 3.14159265358979323846;

// 2D Point
struct Point {
    double x = 0, y = 0;

    constexpr double distance_to(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    constexpr Point operator+(const Point& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr Point operator-(const Point& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// 2D Rectangle
struct Rect {
    Point min, max;

    constexpr double width() const { return max.x - min.x; }
    constexpr double height() const { return max.y - min.y; }
    constexpr double area() const { return width() * height(); }

    constexpr bool contains(const Point& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y;
    }

    constexpr bool intersects(const Rect& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y;
    }

    constexpr Rect intersection(const Rect& other) const {
        return {
            {std::max(min.x, other.min.x), std::max(min.y, other.min.y)},
            {std::min(max.x, other.max.x), std::min(max.y, other.max.y)}
        };
    }
};

// 2D Circle
struct Circle {
    Point center;
    double radius = 0;

    constexpr double area() const { return pi * radius * radius; }
    constexpr double circumference() const { return 2 * pi * radius; }

    constexpr bool contains(const Point& p) const {
        return center.distance_to(p) <= radius;
    }

    constexpr bool intersects(const Circle& other) const {
        double dist = center.distance_to(other.center);
        return dist <= radius + other.radius;
    }
};

// 2D Line Segment
struct Segment {
    Point a, b;

    constexpr double length() const { return a.distance_to(b); }

    constexpr double distance_to(const Point& p) const {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double len_sq = dx * dx + dy * dy;
        if (len_sq == 0) return a.distance_to(p);

        double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len_sq;
        t = std::max(0.0, std::min(1.0, t));

        Point projection = {a.x + t * dx, a.y + t * dy};
        return p.distance_to(projection);
    }

    constexpr bool intersects(const Segment& other) const {
        auto cross = [](Point p1, Point p2) {
            return p1.x * p2.y - p1.y * p2.x;
        };

        Point d1 = b - a;
        Point d2 = other.b - other.a;
        double denom = cross(d1, d2);

        if (std::abs(denom) < 1e-10) return false;

        Point d3 = other.a - a;
        double t = cross(d3, d2) / denom;
        double u = cross(d3, d1) / denom;

        return t >= 0 && t <= 1 && u >= 0 && u <= 1;
    }

    constexpr std::optional<Point> intersection(const Segment& other) const {
        auto cross = [](Point p1, Point p2) {
            return p1.x * p2.y - p1.y * p2.x;
        };

        Point d1 = b - a;
        Point d2 = other.b - other.a;
        double denom = cross(d1, d2);

        if (std::abs(denom) < 1e-10) return std::nullopt;

        Point d3 = other.a - a;
        double t = cross(d3, d2) / denom;
        double u = cross(d3, d1) / denom;

        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
            return Point{a.x + t * d1.x, a.y + t * d1.y};
        }
        return std::nullopt;
    }
};

// Geographic coordinate
struct GeoPoint {
    double latitude = 0;   // [-90, 90]
    double longitude = 0;  // [-180, 180]
};

// Coordinate system enum
enum class CoordSystem {
    WGS84,
    GCJ02,
    BD09
};

// Haversine distance (meters)
inline double haversine_distance(const GeoPoint& a, const GeoPoint& b) {
    auto to_rad = [](double deg) { return deg * pi / 180.0; };

    double lat1 = to_rad(a.latitude);
    double lat2 = to_rad(b.latitude);
    double dlat = to_rad(b.latitude - a.latitude);
    double dlon = to_rad(b.longitude - a.longitude);

    double h = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon / 2) * std::sin(dlon / 2);

    return 2 * earth_radius_m * std::asin(std::sqrt(h));
}

// Bearing (radians)
inline double bearing(const GeoPoint& from, const GeoPoint& to) {
    auto to_rad = [](double deg) { return deg * pi / 180.0; };

    double lat1 = to_rad(from.latitude);
    double lat2 = to_rad(to.latitude);
    double dlon = to_rad(to.longitude - from.longitude);

    double y = std::sin(dlon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) -
               std::sin(lat1) * std::cos(lat2) * std::cos(dlon);

    return std::atan2(y, x);
}

// Destination point
inline GeoPoint destination(const GeoPoint& from, double distance, double bearing_rad) {
    auto to_rad = [](double deg) { return deg * pi / 180.0; };
    auto to_deg = [](double rad) { return rad * 180.0 / pi; };

    double lat1 = to_rad(from.latitude);
    double lon1 = to_rad(from.longitude);
    double d = distance / earth_radius_m;

    double lat2 = std::asin(std::sin(lat1) * std::cos(d) +
                            std::cos(lat1) * std::sin(d) * std::cos(bearing_rad));
    double lon2 = lon1 + std::atan2(std::sin(bearing_rad) * std::sin(d) * std::cos(lat1),
                                     std::cos(d) - std::sin(lat1) * std::sin(lat2));

    return {to_deg(lat2), to_deg(lon2)};
}

// GCJ02 offset (simplified)
namespace detail {
    inline bool in_china(double lat, double lon) {
        return lat >= 0.8293 && lat <= 55.8271 && lon >= 72.004 && lon <= 137.8347;
    }

    inline double transform_lat(double x, double y) {
        double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * std::sqrt(std::abs(x));
        ret += (20.0 * std::sin(6.0 * x * pi) + 20.0 * std::sin(2.0 * x * pi)) * 2.0 / 3.0;
        ret += (20.0 * std::sin(y * pi) + 40.0 * std::sin(y / 3.0 * pi)) * 2.0 / 3.0;
        ret += (160.0 * std::sin(y / 12.0 * pi) + 320 * std::sin(y * pi / 30.0)) * 2.0 / 3.0;
        return ret;
    }

    inline double transform_lon(double x, double y) {
        double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * std::sqrt(std::abs(x));
        ret += (20.0 * std::sin(6.0 * x * pi) + 20.0 * std::sin(2.0 * x * pi)) * 2.0 / 3.0;
        ret += (20.0 * std::sin(x * pi) + 40.0 * std::sin(x / 3.0 * pi)) * 2.0 / 3.0;
        ret += (150.0 * std::sin(x / 12.0 * pi) + 300.0 * std::sin(x / 30.0 * pi)) * 2.0 / 3.0;
        return ret;
    }
} // namespace detail

// WGS84 to GCJ02
inline GeoPoint wgs84_to_gcj02(const GeoPoint& point) {
    if (!detail::in_china(point.latitude, point.longitude)) {
        return point;
    }

    double dlat = detail::transform_lat(point.longitude - 105.0, point.latitude - 35.0);
    double dlon = detail::transform_lon(point.longitude - 105.0, point.latitude - 35.0);

    double radlat = point.latitude / 180.0 * pi;
    double magic = std::sin(radlat);
    magic = 1 - 0.00669342162296594323 * magic * magic;

    double sqrtmagic = std::sqrt(magic);
    dlat = (dlat * 180.0) / ((6335552.717000426 * (1 - 0.00669342162296594323)) / (magic * sqrtmagic) * pi);
    dlon = (dlon * 180.0) / (6378245.0 / sqrtmagic * std::cos(radlat) * pi);

    return {point.latitude + dlat, point.longitude + dlon};
}

// GCJ02 to WGS84 (reverse)
inline GeoPoint gcj02_to_wgs84(const GeoPoint& point) {
    GeoPoint gcj = wgs84_to_gcj02(point);
    return {2 * point.latitude - gcj.latitude, 2 * point.longitude - gcj.longitude};
}

// GCJ02 to BD09
inline GeoPoint gcj02_to_bd09(const GeoPoint& point) {
    double x = point.longitude;
    double y = point.latitude;
    double z = std::sqrt(x * x + y * y) + 0.00002 * std::sin(y * pi);
    double theta = std::atan2(y, x) + 0.000003 * std::cos(x * pi);
    return {z * std::sin(theta) + 0.006, z * std::cos(theta) + 0.0065};
}

// BD09 to GCJ02
inline GeoPoint bd09_to_gcj02(const GeoPoint& point) {
    double x = point.longitude - 0.0065;
    double y = point.latitude - 0.006;
    double z = std::sqrt(x * x + y * y) - 0.00002 * std::sin(y * pi);
    double theta = std::atan2(y, x) - 0.000003 * std::cos(x * pi);
    return {z * std::sin(theta), z * std::cos(theta)};
}

// Generic convert
inline GeoPoint convert(const GeoPoint& point, CoordSystem from, CoordSystem to) {
    if (from == to) return point;

    // Convert to GCJ02 first
    GeoPoint gcj;
    switch (from) {
        case CoordSystem::WGS84: gcj = wgs84_to_gcj02(point); break;
        case CoordSystem::GCJ02: gcj = point; break;
        case CoordSystem::BD09: gcj = bd09_to_gcj02(point); break;
    }

    // Convert from GCJ02 to target
    switch (to) {
        case CoordSystem::WGS84: return gcj02_to_wgs84(gcj);
        case CoordSystem::GCJ02: return gcj;
        case CoordSystem::BD09: return gcj02_to_bd09(gcj);
    }

    return point; // Should not reach here
}

// QuadTree
class QuadTree {
public:
    QuadTree(const Rect& bounds, size_t capacity = 8)
        : bounds_(bounds), capacity_(capacity), root_(std::make_unique<Node>()) {}

    bool insert(const Point& point) {
        if (!bounds_.contains(point)) return false;
        return insert_impl(root_.get(), point, bounds_, 0);
    }

    std::vector<Point> query(const Rect& range) const {
        std::vector<Point> result;
        query_impl(root_.get(), bounds_, range, result);
        return result;
    }

    void clear() {
        root_ = std::make_unique<Node>();
        size_ = 0;
    }

    size_t size() const { return size_; }

private:
    static constexpr int max_depth = 10;

    struct Node {
        std::vector<Point> points;
        std::unique_ptr<Node> children[4]; // NW, NE, SW, SE
        bool is_leaf = true;
    };

    Node* get_quadrant(Node* node, const Rect& bounds, const Point& point) const {
        double mid_x = (bounds.min.x + bounds.max.x) / 2;
        double mid_y = (bounds.min.y + bounds.max.y) / 2;

        if (point.x < mid_x) {
            return point.y < mid_y ? node->children[2].get() : node->children[0].get();
        } else {
            return point.y < mid_y ? node->children[3].get() : node->children[1].get();
        }
    }

    Rect get_quadrant_bounds(const Rect& bounds, int index) const {
        double mid_x = (bounds.min.x + bounds.max.x) / 2;
        double mid_y = (bounds.min.y + bounds.max.y) / 2;

        switch (index) {
            case 0: return {{bounds.min.x, mid_y}, {mid_x, bounds.max.y}};  // NW
            case 1: return {{mid_x, mid_y}, {bounds.max.x, bounds.max.y}};  // NE
            case 2: return {{bounds.min.x, bounds.min.y}, {mid_x, mid_y}};  // SW
            case 3: return {{mid_x, bounds.min.y}, {bounds.max.x, mid_y}};  // SE
            default: return bounds;
        }
    }

    bool insert_impl(Node* node, const Point& point, const Rect& bounds, int depth) {
        if (node->is_leaf) {
            if (node->points.size() < capacity_ || depth >= max_depth) {
                node->points.push_back(point);
                ++size_;
                return true;
            }

            // Split node
            node->is_leaf = false;
            for (int i = 0; i < 4; ++i) {
                node->children[i] = std::make_unique<Node>();
            }

            // Redistribute existing points
            for (const auto& p : node->points) {
                insert_impl(get_quadrant(node, bounds, p), p, get_quadrant_bounds(bounds, 0), depth + 1);
            }
            node->points.clear();
        }

        int quadrant = 0;
        double mid_x = (bounds.min.x + bounds.max.x) / 2;
        double mid_y = (bounds.min.y + bounds.max.y) / 2;

        if (point.x >= mid_x) quadrant += 1;
        if (point.y >= mid_y) quadrant += 2;

        return insert_impl(node->children[quadrant].get(), point,
                          get_quadrant_bounds(bounds, quadrant), depth + 1);
    }

    void query_impl(Node* node, const Rect& bounds, const Rect& range, std::vector<Point>& result) const {
        if (!bounds.intersects(range)) return;

        for (const auto& p : node->points) {
            if (range.contains(p)) {
                result.push_back(p);
            }
        }

        if (!node->is_leaf) {
            for (int i = 0; i < 4; ++i) {
                query_impl(node->children[i].get(), get_quadrant_bounds(bounds, i), range, result);
            }
        }
    }

    Rect bounds_;
    size_t capacity_;
    size_t size_ = 0;
    std::unique_ptr<Node> root_;
};

} // namespace mana::util::geo
```

- [ ] **Step 2: Create test_geo.cpp**

```cpp
#include <mana/util/geo.hpp>
#include <cassert>
#include <iostream>
#include <cmath>

void test_point() {
    mana::util::geo::Point p1{0, 0};
    mana::util::geo::Point p2{3, 4};

    assert(std::abs(p1.distance_to(p2) - 5.0) < 1e-10);

    auto p3 = p1 + p2;
    assert(p3.x == 3 && p3.y == 4);

    auto p4 = p2 - p1;
    assert(p4.x == 3 && p4.y == 4);
}

void test_rect() {
    mana::util::geo::Rect rect{{0, 0}, {10, 10}};

    assert(rect.width() == 10);
    assert(rect.height() == 10);
    assert(rect.area() == 100);

    assert(rect.contains({5, 5}) == true);
    assert(rect.contains({15, 5}) == false);

    mana::util::geo::Rect other{{5, 5}, {15, 15}};
    assert(rect.intersects(other) == true);

    mana::util::geo::Rect far{{20, 20}, {30, 30}};
    assert(rect.intersects(far) == false);
}

void test_circle() {
    mana::util::geo::Circle circle{{0, 0}, 5};

    assert(std::abs(circle.area() - 78.53981633974483) < 1e-10);
    assert(std::abs(circle.circumference() - 31.41592653589793) < 1e-10);

    assert(circle.contains({3, 4}) == true);
    assert(circle.contains({6, 0}) == false);

    mana::util::geo::Circle other{{10, 0}, 5};
    assert(circle.intersects(other) == true);

    mana::util::geo::Circle far{{20, 0}, 5};
    assert(circle.intersects(far) == false);
}

void test_segment() {
    mana::util::geo::Segment seg{{0, 0}, {10, 0}};

    assert(seg.length() == 10);

    assert(std::abs(seg.distance_to({5, 3}) - 3.0) < 1e-10);
    assert(std::abs(seg.distance_to({-1, 0}) - 1.0) < 1e-10);

    mana::util::geo::Segment other{{5, -5}, {5, 5}};
    assert(seg.intersects(other) == true);

    mana::util::geo::Segment far{{5, 5}, {5, 10}};
    assert(seg.intersects(far) == false);
}

void test_haversine() {
    // Beijing to Shanghai (~1068 km)
    mana::util::geo::GeoPoint beijing{39.9042, 116.4074};
    mana::util::geo::GeoPoint shanghai{31.2304, 121.4737};

    double dist = mana::util::geo::haversine_distance(beijing, shanghai);
    assert(dist > 1060000 && dist < 1080000); // ~1068 km
}

void test_bearing() {
    mana::util::geo::GeoPoint north{0, 0};
    mana::util::geo::GeoPoint east{0, 90};

    double b = mana::util::geo::bearing(north, east);
    assert(std::abs(b - mana::util::geo::pi / 2) < 0.1); // ~90 degrees
}

void test_destination() {
    mana::util::geo::GeoPoint start{0, 0};
    auto dest = mana::util::geo::destination(start, 111320, 0); // ~1 degree north

    assert(std::abs(dest.latitude - 1.0) < 0.1);
    assert(std::abs(dest.longitude) < 0.1);
}

void test_coord_convert() {
    mana::util::geo::GeoPoint wgs{39.9042, 116.4074};

    // WGS84 -> GCJ02 -> WGS84 should be close to original
    auto gcj = mana::util::geo::wgs84_to_gcj02(wgs);
    auto back = mana::util::geo::gcj02_to_wgs84(gcj);

    assert(std::abs(back.latitude - wgs.latitude) < 0.001);
    assert(std::abs(back.longitude - wgs.longitude) < 0.001);

    // GCJ02 -> BD09 -> GCJ02 should be close to original
    auto bd = mana::util::geo::gcj02_to_bd09(gcj);
    auto gcj_back = mana::util::geo::bd09_to_gcj02(bd);

    assert(std::abs(gcj_back.latitude - gcj.latitude) < 0.001);
    assert(std::abs(gcj_back.longitude - gcj.longitude) < 0.001);
}

void test_quadtree() {
    mana::util::geo::Rect bounds{{0, 0}, {100, 100}};
    mana::util::geo::QuadTree qt(bounds, 4);

    // Insert points
    assert(qt.insert({10, 10}) == true);
    assert(qt.insert({20, 20}) == true);
    assert(qt.insert({30, 30}) == true);
    assert(qt.insert({40, 40}) == true);
    assert(qt.insert({50, 50}) == true);

    assert(qt.size() == 5);

    // Insert outside bounds
    assert(qt.insert({200, 200}) == false);
    assert(qt.size() == 5);

    // Range query
    mana::util::geo::Rect range{{0, 0}, {25, 25}};
    auto results = qt.query(range);
    assert(results.size() == 2);

    // Clear
    qt.clear();
    assert(qt.size() == 0);
}

int main() {
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
    return 0;
}
```

- [ ] **Step 3: Build and run tests**

Run: `cmake --build build && ./build/Debug/mana_tests.exe`
Expected: All geo tests passed!

- [ ] **Step 4: Commit**

```bash
git add include/mana/util/geo.hpp tests/test_geo.cpp
git commit -m "feat: add geo.hpp with geometry and coordinate conversion"
```

---

### Task 6: 集成与构建配置

**Files:**
- Modify: `include/mana/mana.h`
- Modify: `CMakeLists.txt`
- Modify: `tests/test_framework.cpp`

**Interfaces:**
- Consumes: All util modules from Tasks 1-5

- [ ] **Step 1: Update mana.h**

```cpp
#pragma once

#include "export.h"
#include "thread_pool.h"
#include "event.h"
#include "context.h"
#include "log.h"

// New util modules
#include "util/string.hpp"
#include "util/io.hpp"
#include "util/util.hpp"
#include "util/math.hpp"
#include "util/geo.hpp"

namespace mana {

// Framework state (internal)
struct MANA_API FrameworkState {
    event::EventBus event;
    context::Context context;
    log::Logger logger;
    thread_pool::ThreadPool thread_pool;

    FrameworkState() = default;
    ~FrameworkState() = default;

    FrameworkState(const FrameworkState&) = delete;
    FrameworkState& operator=(const FrameworkState&) = delete;
    FrameworkState(FrameworkState&&) = delete;
    FrameworkState& operator=(FrameworkState&&) = delete;
};

// Get framework instance (singleton)
MANA_API FrameworkState& instance();

// Framework lifecycle
MANA_API void initialize();
MANA_API void shutdown();

// Direct module access
MANA_API log::Logger& logger();

} // namespace mana
```

- [ ] **Step 2: Update CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.20)
project(mana VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(MANA_BUILD_TESTS "Build tests" ON)
option(MANA_BUILD_EXAMPLES "Build examples" ON)

# Library target
add_library(mana SHARED
    src/log.cpp
    src/context.cpp
    src/thread_pool.cpp
    src/event.cpp
    src/framework.cpp
)

target_include_directories(mana PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(mana PRIVATE MANA_EXPORTS)

# Tests
if(MANA_BUILD_TESTS)
    enable_testing()
    add_executable(mana_tests
        tests/test_string.cpp
        tests/test_io_util.cpp
        tests/test_util.cpp
        tests/test_math.cpp
        tests/test_geo.cpp
        tests/test_log.cpp
        tests/test_context.cpp
        tests/test_thread_pool.cpp
        tests/test_event.cpp
        tests/test_framework.cpp
    )
    target_link_libraries(mana_tests PRIVATE mana)
    add_test(NAME mana_tests COMMAND mana_tests)
endif()

# Example
if(MANA_BUILD_EXAMPLES)
    add_executable(mana_example examples/main.cpp)
    target_link_libraries(mana_example PRIVATE mana)
endif()
```

- [ ] **Step 3: Update test_framework.cpp**

```cpp
#include <mana/mana.h>
#include <cassert>
#include <iostream>

void test_framework_initialize() {
    mana::initialize();
    auto& logger = mana::logger();
    logger.info("Framework initialized");
    mana::shutdown();
}

void test_framework_singleton() {
    mana::initialize();
    auto& instance1 = mana::instance();
    auto& instance2 = mana::instance();
    assert(&instance1 == &instance2);
    mana::shutdown();
}

void test_framework_modules() {
    mana::initialize();
    auto& bus = mana::instance().event;
    auto& ctx = mana::instance().context;
    auto& logger = mana::logger();
    auto& pool = mana::instance().thread_pool;
    assert(pool.thread_count() > 0);
    mana::shutdown();
}

void test_util_modules() {
    // Test string utilities
    auto result = mana::util::string::split("hello,world", ",");
    assert(result.size() == 2);

    // Test math utilities
    assert(mana::util::math::clamp(5, 1, 10) == 5);

    // Test geo utilities
    mana::util::geo::Point p1{0, 0};
    mana::util::geo::Point p2{3, 4};
    assert(std::abs(p1.distance_to(p2) - 5.0) < 1e-10);
}

int main() {
    test_framework_initialize();
    test_framework_singleton();
    test_framework_modules();
    test_util_modules();
    std::cout << "All framework tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 4: Build and run all tests**

Run: `cmake --build build && ctest --test-dir build -C Debug`
Expected: All tests pass

- [ ] **Step 5: Commit**

```bash
git add include/mana/mana.h CMakeLists.txt tests/test_framework.cpp
git commit -m "refactor: integrate new util modules into framework"
```

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-07-10-util-refactor.md`. Two execution options:**

**1. Subagent-Driven (recommended)** - I dispatch a fresh subagent per task, review between tasks, fast iteration

**2. Inline Execution** - Execute tasks in this session using executing-plans, batch execution with checkpoints

**Which approach?**
