# Mana Framework Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement the Mana C++23 framework as a dynamic library with thread pool, event bus, context management, logging, IO, and utility modules.

**Architecture:** Singleton-based framework with direct module access via `mana::module_name()` functions. Each module uses Pimpl idiom for implementation hiding and RAII for resource management.

**Tech Stack:** C++23, MSVC 2019 16.10+, Windows DLL, Standard Library only

## Global Constraints

- **Compiler:** MSVC 2019 16.10+ (C++20 std::format and concepts)
- **Standard:** C++23
- **Platform:** Windows
- **Dependencies:** C++23 Standard Library only
- **Naming:** Full English words, no abbreviations (AI-friendly)
- **Namespaces:** `mana::module_name` structure
- **Error Handling:** `std::optional` for not-found, exceptions for errors
- **Thread Safety:** Internal synchronization for all public APIs

---

## File Structure

```
mana/
├── CMakeLists.txt          # Build configuration
├── include/
│   └── mana/
│       ├── export.h        # DLL export macros
│       ├── util.h          # Utility functions
│       ├── log.h           # Logging module
│       ├── io.h            # File IO module
│       ├── context.h       # Context management
│       ├── thread_pool.h   # Thread pool
│       ├── event.h     # Message bus
│       └── mana.h          # Main header
├── src/
│   ├── util.cpp            # Utility implementation
│   ├── log.cpp             # Logging implementation
│   ├── io.cpp              # IO implementation
│   ├── context.cpp         # Context implementation
│   ├── thread_pool.cpp     # Thread pool implementation
│   ├── event.cpp       # Event bus implementation
│   └── framework.cpp       # Framework singleton
├── tests/
│   ├── test_util.cpp       # Utility tests
│   ├── test_log.cpp        # Logging tests
│   ├── test_io.cpp         # IO tests
│   ├── test_context.cpp    # Context tests
│   ├── test_thread_pool.cpp # Thread pool tests
│   ├── test_event.cpp  # Event bus tests
│   └── test_framework.cpp  # Integration tests
└── examples/
    └── main.cpp            # Usage example
```

---

### Task 1: Project Setup and Build Configuration

**Files:**
- Create: `CMakeLists.txt`
- Create: `include/mana/export.h`

**Interfaces:**
- Produces: Build system configuration, `MANA_API` macro for DLL export

- [ ] **Step 1: Create CMakeLists.txt**

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
    src/util.cpp
    src/log.cpp
    src/io.cpp
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
        tests/test_util.cpp
        tests/test_log.cpp
        tests/test_io.cpp
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

- [ ] **Step 2: Create export.h**

```cpp
#pragma once

#ifdef MANA_EXPORTS
    #define MANA_API __declspec(dllexport)
#else
    #define MANA_API __declspec(dllimport)
#endif
```

- [ ] **Step 3: Verify build configuration**

Run: `cmake -B build -S . && cmake --build build`
Expected: Build succeeds with empty source files

- [ ] **Step 4: Commit**

```bash
git add CMakeLists.txt include/mana/export.h
git commit -m "feat: add project setup and build configuration"
```

---

### Task 2: Utility Module

**Files:**
- Create: `include/mana/util.h`
- Create: `src/util.cpp`
- Create: `tests/test_util.cpp`

**Interfaces:**
- Produces: `mana::util::split()`, `mana::util::current_timestamp_ms()`, `mana::util::current_timestamp_s()`, `mana::util::trim()`, `mana::util::to_lower()`, `mana::util::to_upper()`, `mana::util::starts_with()`, `mana::util::ends_with()`

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_util.cpp
#include <mana/util.h>
#include <cassert>
#include <iostream>

void test_split() {
    auto result = mana::util::split("hello,world,foo", ",");
    assert(result.size() == 3);
    assert(result[0] == "hello");
    assert(result[1] == "world");
    assert(result[2] == "foo");
}

void test_split_empty() {
    auto result = mana::util::split("", ",");
    assert(result.size() == 1);
    assert(result[0] == "");
}

void test_trim() {
    assert(mana::util::trim("  hello  ") == "hello");
    assert(mana::util::trim("hello") == "hello");
    assert(mana::util::trim("  ") == "");
}

void test_to_lower() {
    assert(mana::util::to_lower("HELLO") == "hello");
    assert(mana::util::to_lower("Hello") == "hello");
}

void test_to_upper() {
    assert(mana::util::to_upper("hello") == "HELLO");
    assert(mana::util::to_upper("Hello") == "HELLO");
}

void test_starts_with() {
    assert(mana::util::starts_with("hello world", "hello") == true);
    assert(mana::util::starts_with("hello world", "world") == false);
}

void test_ends_with() {
    assert(mana::util::ends_with("hello world", "world") == true);
    assert(mana::util::ends_with("hello world", "hello") == false);
}

void test_timestamp() {
    auto ms = mana::util::current_timestamp_ms();
    auto s = mana::util::current_timestamp_s();
    assert(ms > 0);
    assert(s > 0);
    assert(ms >= s * 1000);
}

int main() {
    test_split();
    test_split_empty();
    test_trim();
    test_to_lower();
    test_to_upper();
    test_starts_with();
    test_ends_with();
    test_timestamp();
    std::cout << "All utility tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create util.h header**

```cpp
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace mana::util {

// Split string by delimiter
std::vector<std::string> split(const std::string& str, const std::string& delimiter);

// Get current timestamp in milliseconds
int64_t current_timestamp_ms();

// Get current timestamp in seconds
int64_t current_timestamp_s();

// Trim whitespace from both ends
std::string trim(const std::string& str);

// Convert string to lowercase
std::string to_lower(const std::string& str);

// Convert string to uppercase
std::string to_upper(const std::string& str);

// Check if string starts with prefix
bool starts_with(const std::string& str, const std::string& prefix);

// Check if string ends with suffix
bool ends_with(const std::string& str, const std::string& suffix);

} // namespace mana::util
```

- [ ] **Step 4: Create util.cpp implementation**

```cpp
#include <mana/util.h>
#include <algorithm>
#include <chrono>
#include <cctype>

namespace mana::util {

std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    if (str.empty()) {
        result.push_back("");
        return result;
    }
    
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

int64_t current_timestamp_ms() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int64_t current_timestamp_s() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }
    
    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    
    return std::string(start, end + 1);
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool starts_with(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

} // namespace mana::util
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All utility tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/util.h src/util.cpp tests/test_util.cpp
git commit -m "feat: add utility module with string and timestamp functions"
```

---

### Task 3: Logging Module

**Files:**
- Create: `include/mana/log.h`
- Create: `src/log.cpp`
- Create: `tests/test_log.cpp`

**Interfaces:**
- Consumes: `mana::util::current_timestamp_ms()` from Task 2
- Produces: `mana::log::Logger` class with `set_level()`, `log()`, `debug()`, `info()`, `warning()`, `error()`

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_log.cpp
#include <mana/log.h>
#include <cassert>
#include <iostream>
#include <sstream>

void test_log_level() {
    mana::log::Logger logger;
    logger.set_level(mana::log::Level::Info);
    assert(logger.level() == mana::log::Level::Info);
}

void test_log_filtering() {
    mana::log::Logger logger;
    logger.set_level(mana::log::Level::Warning);
    
    // These should not output (below threshold)
    logger.debug("Debug message");
    logger.info("Info message");
    
    // These should output
    logger.warning("Warning message");
    logger.error("Error message");
}

void test_log_format() {
    mana::log::Logger logger;
    logger.set_level(mana::log::Level::Debug);
    
    // Test formatted output
    logger.info("Hello {}, count: {}", "world", 42);
    logger.error("Error code: {}", 404);
}

int main() {
    test_log_level();
    test_log_filtering();
    test_log_format();
    std::cout << "All logging tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create log.h header**

```cpp
#pragma once

#include <string>
#include <format>
#include <memory>

#include "export.h"

namespace mana::log {

enum class Level {
    Debug,
    Info,
    Warning,
    Error
};

class MANA_API Logger {
public:
    Logger();
    ~Logger();
    
    // Set minimum log level
    void set_level(Level level);
    
    // Get current log level
    Level level() const;
    
    // Log message with level
    void log(Level level, const std::string& message);
    
    // Convenience functions with std::format
    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        if (level() <= Level::Debug) {
            log(Level::Debug, std::vformat(format, std::make_format_args(args...)));
        }
    }
    
    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        if (level() <= Level::Info) {
            log(Level::Info, std::vformat(format, std::make_format_args(args...)));
        }
    }
    
    template<typename... Args>
    void warning(const std::string& format, Args&&... args) {
        if (level() <= Level::Warning) {
            log(Level::Warning, std::vformat(format, std::make_format_args(args...)));
        }
    }
    
    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        if (level() <= Level::Error) {
            log(Level::Error, std::vformat(format, std::make_format_args(args...)));
        }
    }
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::log
```

- [ ] **Step 4: Create log.cpp implementation**

```cpp
#include <mana/log.h>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace mana::log {

class Logger::Impl {
public:
    Level level = Level::Info;
    std::mutex mutex;
    
    std::string level_string(Level level) const {
        switch (level) {
            case Level::Debug: return "DEBUG";
            case Level::Info: return "INFO";
            case Level::Warning: return "WARNING";
            case Level::Error: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
    std::string get_timestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
};

Logger::Logger() : impl_(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::set_level(Level level) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->level = level;
}

Level Logger::level() const {
    return impl_->level;
}

void Logger::log(Level level, const std::string& message) {
    if (level < impl_->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::cerr << "[" << impl_->get_timestamp() << "] "
              << "[" << impl_->level_string(level) << "] "
              << message << std::endl;
}

} // namespace mana::log
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All logging tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/log.h src/log.cpp tests/test_log.cpp
git commit -m "feat: add logging module with level filtering and std::format"
```

---

### Task 4: IO Module

**Files:**
- Create: `include/mana/io.h`
- Create: `src/io.cpp`
- Create: `tests/test_io.cpp`

**Interfaces:**
- Produces: `mana::io::FileManager` class with `read_all()`, `write()`, `exists()`, `file_size()`

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_io.cpp
#include <mana/io.h>
#include <cassert>
#include <iostream>
#include <fstream>

void test_write_and_read() {
    mana::io::FileManager fm;
    std::string test_path = "test_file.txt";
    std::string content = "Hello, World!";
    
    // Write file
    bool written = fm.write(test_path, content);
    assert(written == true);
    
    // Read file
    auto result = fm.read_all(test_path);
    assert(result.has_value());
    assert(result.value() == content);
    
    // Cleanup
    std::remove(test_path.c_str());
}

void test_exists() {
    mana::io::FileManager fm;
    std::string test_path = "test_exists.txt";
    
    // File doesn't exist yet
    assert(fm.exists(test_path) == false);
    
    // Create file
    std::ofstream file(test_path);
    file << "test";
    file.close();
    
    // Now exists
    assert(fm.exists(test_path) == true);
    
    // Cleanup
    std::remove(test_path.c_str());
}

void test_file_size() {
    mana::io::FileManager fm;
    std::string test_path = "test_size.txt";
    std::string content = "Hello";
    
    // Write file
    fm.write(test_path, content);
    
    // Check size
    auto size = fm.file_size(test_path);
    assert(size.has_value());
    assert(size.value() == content.length());
    
    // Cleanup
    std::remove(test_path.c_str());
}

void test_read_nonexistent() {
    mana::io::FileManager fm;
    auto result = fm.read_all("nonexistent.txt");
    assert(result.has_value() == false);
}

int main() {
    test_write_and_read();
    test_exists();
    test_file_size();
    test_read_nonexistent();
    std::cout << "All IO tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create io.h header**

```cpp
#pragma once

#include <string>
#include <optional>
#include <memory>

#include "export.h"

namespace mana::io {

class MANA_API FileManager {
public:
    FileManager();
    ~FileManager();
    
    // Read entire file content
    std::optional<std::string> read_all(const std::string& path);
    
    // Write content to file
    bool write(const std::string& path, const std::string& content);
    
    // Check if file exists
    bool exists(const std::string& path);
    
    // Get file size in bytes
    std::optional<size_t> file_size(const std::string& path);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::io
```

- [ ] **Step 4: Create io.cpp implementation**

```cpp
#include <mana/io.h>
#include <fstream>
#include <filesystem>

namespace mana::io {

class FileManager::Impl {
public:
    // Implementation details
};

FileManager::FileManager() : impl_(std::make_unique<Impl>()) {}

FileManager::~FileManager() = default;

std::optional<std::string> FileManager::read_all(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read content
    std::string content(size, '\0');
    file.read(content.data(), size);
    
    return content;
}

bool FileManager::write(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(content.data(), content.size());
    return file.good();
}

bool FileManager::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::optional<size_t> FileManager::file_size(const std::string& path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    if (ec) {
        return std::nullopt;
    }
    return size;
}

} // namespace mana::io
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All IO tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/io.h src/io.cpp tests/test_io.cpp
git commit -m "feat: add IO module with file read/write operations"
```

---

### Task 5: Context Module

**Files:**
- Create: `include/mana/context.h`
- Create: `src/context.cpp`
- Create: `tests/test_context.cpp`

**Interfaces:**
- Produces: `mana::context::Context` class with `put()`, `get()`, `has()`, `remove()`, `clear()`, `keys()`
- Produces: `mana::context::attach()`, `mana::context::detach()`, `mana::context::current()`

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_context.cpp
#include <mana/context.h>
#include <cassert>
#include <iostream>

void test_context_put_get() {
    mana::context::Context ctx;
    ctx.put("name", std::string("panda"));
    ctx.put("age", 42);
    
    auto name = ctx.get<std::string>("name");
    assert(name.has_value());
    assert(name.value() == "panda");
    
    auto age = ctx.get<int>("age");
    assert(age.has_value());
    assert(age.value() == 42);
}

void test_context_has() {
    mana::context::Context ctx;
    assert(ctx.has("key") == false);
    
    ctx.put("key", std::string("value"));
    assert(ctx.has("key") == true);
}

void test_context_remove() {
    mana::context::Context ctx;
    ctx.put("key", std::string("value"));
    assert(ctx.has("key") == true);
    
    ctx.remove("key");
    assert(ctx.has("key") == false);
}

void test_context_clear() {
    mana::context::Context ctx;
    ctx.put("key1", std::string("value1"));
    ctx.put("key2", std::string("value2"));
    
    ctx.clear();
    assert(ctx.has("key1") == false);
    assert(ctx.has("key2") == false);
}

void test_context_keys() {
    mana::context::Context ctx;
    ctx.put("key1", std::string("value1"));
    ctx.put("key2", std::string("value2"));
    
    auto keys = ctx.keys();
    assert(keys.size() == 2);
}

void test_context_stack() {
    mana::context::Context ctx1;
    ctx1.put("level", std::string("outer"));
    
    mana::context::attach(std::move(ctx1));
    assert(mana::context::current().get<std::string>("level").value() == "outer");
    
    {
        mana::context::Context ctx2;
        ctx2.put("level", std::string("inner"));
        mana::context::attach(std::move(ctx2));
        
        assert(mana::context::current().get<std::string>("level").value() == "inner");
        mana::context::detach();
    }
    
    assert(mana::context::current().get<std::string>("level").value() == "outer");
    mana::context::detach();
}

int main() {
    test_context_put_get();
    test_context_has();
    test_context_remove();
    test_context_clear();
    test_context_keys();
    test_context_stack();
    std::cout << "All context tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create context.h header**

```cpp
#pragma once

#include <string>
#include <any>
#include <optional>
#include <vector>
#include <memory>

#include "export.h"

namespace mana::context {

class MANA_API Context {
public:
    Context();
    ~Context();
    
    // Store value with key
    void put(const std::string& key, std::any value);
    
    // Retrieve value by key
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Remove key-value pair
    void remove(const std::string& key);
    
    // Clear all entries
    void clear();
    
    // Get all keys
    std::vector<std::string> keys() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Global context stack management
MANA_API void attach(Context context);
MANA_API void detach();
MANA_API Context& current();

} // namespace mana::context
```

- [ ] **Step 4: Create context.cpp implementation**

```cpp
#include <mana/context.h>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace mana::context {

class Context::Impl {
public:
    std::unordered_map<std::string, std::any> storage;
};

Context::Context() : impl_(std::make_unique<Impl>()) {}

Context::~Context() = default;

void Context::put(const std::string& key, std::any value) {
    impl_->storage[key] = std::move(value);
}

template<typename T>
std::optional<T> Context::get(const std::string& key) const {
    auto it = impl_->storage.find(key);
    if (it == impl_->storage.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

bool Context::has(const std::string& key) const {
    return impl_->storage.find(key) != impl_->storage.end();
}

void Context::remove(const std::string& key) {
    impl_->storage.erase(key);
}

void Context::clear() {
    impl_->storage.clear();
}

std::vector<std::string> Context::keys() const {
    std::vector<std::string> result;
    result.reserve(impl_->storage.size());
    for (const auto& pair : impl_->storage) {
        result.push_back(pair.first);
    }
    return result;
}

// Explicit template instantiation for common types
template MANA_API std::optional<std::string> Context::get<std::string>(const std::string&) const;
template MANA_API std::optional<int> Context::get<int>(const std::string&) const;
template MANA_API std::optional<double> Context::get<double>(const std::string&) const;
template MANA_API std::optional<bool> Context::get<bool>(const std::string&) const;

// Thread-local context stack
thread_local std::vector<Context> context_stack;

void attach(Context context) {
    context_stack.push_back(std::move(context));
}

void detach() {
    if (!context_stack.empty()) {
        context_stack.pop_back();
    }
}

Context& current() {
    if (context_stack.empty()) {
        // Create default context if stack is empty
        context_stack.push_back(Context());
    }
    return context_stack.back();
}

} // namespace mana::context
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All context tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/context.h src/context.cpp tests/test_context.cpp
git commit -m "feat: add context module with thread-local stack management"
```

---

### Task 6: Thread Pool Module

**Files:**
- Create: `include/mana/thread_pool.h`
- Create: `src/thread_pool.cpp`
- Create: `tests/test_thread_pool.cpp`

**Interfaces:**
- Produces: `mana::thread_pool::ThreadPool` class with `submit()`, `wait_all()`, `pending_count()`, `thread_count()`

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_thread_pool.cpp
#include <mana/thread_pool.h>
#include <cassert>
#include <iostream>
#include <future>

void test_thread_pool_submit() {
    mana::thread_pool::ThreadPool pool(2);
    
    auto future = pool.submit([]() {
        return 42;
    });
    
    assert(future.get() == 42);
}

void test_thread_pool_multiple_tasks() {
    mana::thread_pool::ThreadPool pool(2);
    
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; i++) {
        futures.push_back(pool.submit([i]() {
            return i * 2;
        }));
    }
    
    for (int i = 0; i < 10; i++) {
        assert(futures[i].get() == i * 2);
    }
}

void test_thread_pool_wait_all() {
    mana::thread_pool::ThreadPool pool(2);
    
    std::atomic<int> counter{0};
    for (int i = 0; i < 5; i++) {
        pool.submit([&counter]() {
            counter++;
        });
    }
    
    pool.wait_all();
    assert(counter == 5);
}

void test_thread_pool_pending_count() {
    mana::thread_pool::ThreadPool pool(1);
    
    // Submit a blocking task
    std::promise<void> blocker;
    pool.submit([&blocker]() {
        blocker.get_future().wait();
    });
    
    // Submit more tasks
    pool.submit([]() {});
    pool.submit([]() {});
    
    assert(pool.pending_count() >= 2);
    
    // Release blocker
    blocker.set_value();
    pool.wait_all();
}

int main() {
    test_thread_pool_submit();
    test_thread_pool_multiple_tasks();
    test_thread_pool_wait_all();
    test_thread_pool_pending_count();
    std::cout << "All thread pool tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create thread_pool.h header**

```cpp
#pragma once

#include <functional>
#include <future>
#include <memory>
#include <cstddef>

#include "export.h"

namespace mana::thread_pool {

class MANA_API ThreadPool {
public:
    explicit ThreadPool(size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // Submit task and return future
    template<typename F, typename... Args>
    auto submit(F&& func, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>>;
    
    // Wait for all pending tasks to complete
    void wait_all();
    
    // Get number of pending tasks
    size_t pending_count() const;
    
    // Get number of worker threads
    size_t thread_count() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Template implementation
template<typename F, typename... Args>
auto ThreadPool::submit(F&& func, Args&&... args) 
    -> std::future<std::invoke_result_t<F, Args...>> 
{
    using return_type = std::invoke_result_t<F, Args...>;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...)
    );
    
    std::future<return_type> result = task->get_future();
    
    // Submit task to queue
    {
        std::unique_lock<std::mutex> lock(impl_->mutex);
        impl_->tasks.push([task]() { (*task)(); });
    }
    
    impl_->condition.notify_one();
    return result;
}

} // namespace mana::thread_pool
```

- [ ] **Step 4: Create thread_pool.cpp implementation**

```cpp
#include <mana/thread_pool.h>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace mana::thread_pool {

class ThreadPool::Impl {
public:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    mutable std::mutex mutex;
    std::condition_variable condition;
    std::atomic<bool> stop{false};
    std::atomic<size_t> active_tasks{0};
};

ThreadPool::ThreadPool(size_t thread_count) : impl_(std::make_unique<Impl>()) {
    for (size_t i = 0; i < thread_count; i++) {
        impl_->workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(impl_->mutex);
                    impl_->condition.wait(lock, [this]() {
                        return impl_->stop.load() || !impl_->tasks.empty();
                    });
                    
                    if (impl_->stop.load() && impl_->tasks.empty()) {
                        return;
                    }
                    
                    task = std::move(impl_->tasks.front());
                    impl_->tasks.pop();
                }
                
                impl_->active_tasks++;
                task();
                impl_->active_tasks--;
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    impl_->stop.store(true);
    impl_->condition.notify_all();
    
    for (auto& worker : impl_->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::wait_all() {
    while (true) {
        std::unique_lock<std::mutex> lock(impl_->mutex);
        if (impl_->tasks.empty() && impl_->active_tasks.load() == 0) {
            break;
        }
        lock.unlock();
        std::this_thread::yield();
    }
}

size_t ThreadPool::pending_count() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->tasks.size();
}

size_t ThreadPool::thread_count() const {
    return impl_->workers.size();
}

} // namespace mana::thread_pool
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All thread pool tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/thread_pool.h src/thread_pool.cpp tests/test_thread_pool.cpp
git commit -m "feat: add thread pool module with future support"
```

---

### Task 7: Event Bus Module

**Files:**
- Create: `include/mana/event.h`
- Create: `src/event.cpp`
- Create: `tests/test_event.cpp`

**Interfaces:**
- Produces: `mana::event::EventBus` class with `subscribe()`, `unsubscribe()`, `publish()`, `publish_sync()`
- Produces: `mana::event::EventType` concept

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_event.cpp
#include <mana/event.h>
#include <cassert>
#include <iostream>
#include <atomic>

struct TestEvent {
    int value;
};

void test_event_subscribe_publish() {
    mana::event::EventBus bus;
    int received_value = 0;
    
    bus.subscribe<TestEvent>([&received_value](const TestEvent& e) {
        received_value = e.value;
    });
    
    bus.publish_sync(TestEvent{42});
    assert(received_value == 42);
}

void test_event_multiple_subscribers() {
    mana::event::EventBus bus;
    int count = 0;
    
    bus.subscribe<TestEvent>([&count](const TestEvent&) {
        count++;
    });
    
    bus.subscribe<TestEvent>([&count](const TestEvent&) {
        count++;
    });
    
    bus.publish_sync(TestEvent{0});
    assert(count == 2);
}

void test_event_unsubscribe() {
    mana::event::EventBus bus;
    int count = 0;
    
    auto id = bus.subscribe<TestEvent>([&count](const TestEvent&) {
        count++;
    });
    
    bus.publish_sync(TestEvent{0});
    assert(count == 1);
    
    bus.unsubscribe<TestEvent>(id);
    bus.publish_sync(TestEvent{0});
    assert(count == 1);  // Should not increase
}

void test_event_different_types() {
    mana::event::EventBus bus;
    int int_value = 0;
    std::string str_value;
    
    struct StringEvent {
        std::string value;
    };
    
    bus.subscribe<TestEvent>([&int_value](const TestEvent& e) {
        int_value = e.value;
    });
    
    bus.subscribe<StringEvent>([&str_value](const StringEvent& e) {
        str_value = e.value;
    });
    
    bus.publish_sync(TestEvent{42});
    bus.publish_sync(StringEvent{"hello"});
    
    assert(int_value == 42);
    assert(str_value == "hello");
}

int main() {
    test_event_subscribe_publish();
    test_event_multiple_subscribers();
    test_event_unsubscribe();
    test_event_different_types();
    std::cout << "All event bus tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create event.h header**

```cpp
#pragma once

#include <functional>
#include <memory>
#include <concepts>
#include <cstddef>

#include "export.h"

namespace mana::event {

// Event type concept - any copyable type can be used as an event
template<typename T>
concept EventType = std::copy_constructible<T>;

class MANA_API EventBus {
public:
    EventBus();
    ~EventBus();
    
    // Subscribe to event type
    template<EventType T>
    size_t subscribe(std::function<void(const T&)> callback);
    
    // Unsubscribe by ID
    template<EventType T>
    void unsubscribe(size_t subscription_id);
    
    // Publish event asynchronously
    template<EventType T>
    void publish(const T& event);
    
    // Publish event synchronously (wait for all subscribers)
    template<EventType T>
    void publish_sync(const T& event);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::event
```

- [ ] **Step 4: Create event.cpp implementation**

```cpp
#include <mana/event.h>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <mutex>
#include <shared_mutex>
#include <any>

namespace mana::event {

class EventBus::Impl {
public:
    struct Subscription {
        size_t id;
        std::function<void(const std::any&)> callback;
    };
    
    std::unordered_map<std::type_index, std::vector<Subscription>> subscribers;
    std::unordered_map<std::type_index, size_t> next_ids;
    mutable std::shared_mutex mutex;
};

EventBus::EventBus() : impl_(std::make_unique<Impl>()) {}

EventBus::~EventBus() = default;

template<EventType T>
size_t EventBus::subscribe(std::function<void(const T&)> callback) {
    std::unique_lock lock(impl_->mutex);
    
    auto type_idx = std::type_index(typeid(T));
    size_t id = impl_->next_ids[type_idx]++;
    
    impl_->subscribers[type_idx].push_back({
        id,
        [callback](const std::any& event) {
            callback(std::any_cast<const T&>(event));
        }
    });
    
    return id;
}

template<EventType T>
void EventBus::unsubscribe(size_t subscription_id) {
    std::unique_lock lock(impl_->mutex);
    
    auto type_idx = std::type_index(typeid(T));
    auto& subs = impl_->subscribers[type_idx];
    
    subs.erase(
        std::remove_if(subs.begin(), subs.end(),
            [subscription_id](const Impl::Subscription& sub) {
                return sub.id == subscription_id;
            }),
        subs.end()
    );
}

template<EventType T>
void EventBus::publish(const T& event) {
    std::shared_lock lock(impl_->mutex);
    
    auto type_idx = std::type_index(typeid(T));
    auto it = impl_->subscribers.find(type_idx);
    
    if (it != impl_->subscribers.end()) {
        for (const auto& sub : it->second) {
            sub.callback(std::cref(event));
        }
    }
}

template<EventType T>
void EventBus::publish_sync(const T& event) {
    // For now, sync and async are the same
    publish(event);
}

// Explicit template instantiation for common types
template MANA_API size_t EventBus::subscribe<TestEvent>(std::function<void(const TestEvent&)>);
template MANA_API void EventBus::unsubscribe<TestEvent>(size_t);
template MANA_API void EventBus::publish<TestEvent>(const TestEvent&);
template MANA_API void EventBus::publish_sync<TestEvent>(const TestEvent&);

} // namespace mana::event
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All event bus tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/event.h src/event.cpp tests/test_event.cpp
git commit -m "feat: add event bus module with type-safe pub/sub"
```

---

### Task 8: Framework Singleton and Integration

**Files:**
- Create: `src/framework.cpp`
- Create: `tests/test_framework.cpp`

**Interfaces:**
- Consumes: All modules from Tasks 2-7
- Produces: `mana::instance()`, `mana::initialize()`, `mana::shutdown()`, `mana::thread_pool()`, `mana::event()`, `mana::context()`, `mana::logger()`, `mana::file_manager()`

- [ ] **Step 1: Write failing tests**

```cpp
// tests/test_framework.cpp
#include <mana/mana.h>
#include <cassert>
#include <iostream>

void test_framework_initialize() {
    mana::initialize();
    
    // Should not crash
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
    
    auto& pool = mana::thread_pool();
    auto& bus = mana::event();
    auto& ctx = mana::context();
    auto& logger = mana::logger();
    auto& fm = mana::file_manager();
    
    // Should not crash
    assert(pool.thread_count() > 0);
    
    mana::shutdown();
}

int main() {
    test_framework_initialize();
    test_framework_singleton();
    test_framework_modules();
    std::cout << "All framework tests passed!" << std::endl;
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: Compilation error (header not found)

- [ ] **Step 3: Create mana.h header**

```cpp
#pragma once

#include "export.h"
#include "thread_pool.h"
#include "event.h"
#include "context.h"
#include "log.h"
#include "io.h"
#include "util.h"

namespace mana {

// Framework state (internal)
struct FrameworkState {
    thread_pool::ThreadPool thread_pool;
    event::EventBus event;
    context::Context context;
    log::Logger logger;
    io::FileManager file_manager;
    
    FrameworkState() = default;
    ~FrameworkState() = default;
    
    // Non-copyable, non-movable
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

// Direct module access (preferred)
MANA_API thread_pool::ThreadPool& thread_pool();
MANA_API event::EventBus& event();
MANA_API context::Context& context();
MANA_API log::Logger& logger();
MANA_API io::FileManager& file_manager();

} // namespace mana
```

- [ ] **Step 4: Create framework.cpp implementation**

```cpp
#include <mana/mana.h>
#include <memory>
#include <mutex>

namespace mana {

// Global framework state
std::unique_ptr<FrameworkState> g_state;
std::mutex g_mutex;
bool g_initialized = false;

FrameworkState& instance() {
    if (!g_state) {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_state) {
            g_state = std::make_unique<FrameworkState>();
        }
    }
    return *g_state;
}

void initialize() {
    if (g_initialized) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_initialized) {
        return;
    }
    
    // Force creation of instance
    instance();
    g_initialized = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_initialized) {
        return;
    }
    
    g_state.reset();
    g_initialized = false;
}

thread_pool::ThreadPool& thread_pool() {
    return instance().thread_pool;
}

event::EventBus& event() {
    return instance().event;
}

context::Context& context() {
    return instance().context;
}

log::Logger& logger() {
    return instance().logger;
}

io::FileManager& file_manager() {
    return instance().file_manager;
}

} // namespace mana
```

- [ ] **Step 5: Run test to verify it passes**

Run: `cmake --build build --target mana_tests && ./build/mana_tests`
Expected: "All framework tests passed!"

- [ ] **Step 6: Commit**

```bash
git add include/mana/mana.h src/framework.cpp tests/test_framework.cpp
git commit -m "feat: add framework singleton with direct module access"
```

---

### Task 9: Usage Example

**Files:**
- Create: `examples/main.cpp`

**Interfaces:**
- Consumes: All modules from Tasks 2-8

- [ ] **Step 1: Create main.cpp example**

```cpp
#include <mana/mana.h>
#include <iostream>

int main() {
    // Initialize framework
    mana::initialize();
    
    // Direct module access
    auto& logger = mana::logger();
    logger.info("Mana framework initialized");
    
    // Use thread pool
    auto& pool = mana::thread_pool();
    auto future = pool.submit([]() {
        return 42;
    });
    logger.info("Task result: {}", future.get());
    
    // Use message bus
    auto& bus = mana::event();
    
    struct MyEvent {
        int value;
    };
    
    bus.subscribe<MyEvent>([&logger](const MyEvent& e) {
        logger.info("Received event with value: {}", e.value);
    });
    
    MyEvent event{100};
    bus.publish_sync(event);
    
    // Use context
    auto& ctx = mana::context();
    ctx.put("username", std::string("panda"));
    
    auto& current_ctx = mana::context::current();
    if (auto username = current_ctx.get<std::string>("username")) {
        logger.info("Current user: {}", *username);
    }
    
    // Use utilities
    auto parts = mana::util::split("hello,world,foo", ",");
    logger.info("Split result: {} parts", parts.size());
    
    // Use file manager
    auto& fm = mana::file_manager();
    fm.write("test.txt", "Hello from Mana!");
    if (auto content = fm.read_all("test.txt")) {
        logger.info("File content: {}", *content);
    }
    
    // Shutdown framework
    mana::shutdown();
    return 0;
}
```

- [ ] **Step 2: Build and run example**

Run: `cmake --build build --target mana_example && ./build/mana_example`
Expected: Program runs without crashes, outputs log messages

- [ ] **Step 3: Commit**

```bash
git add examples/main.cpp
git commit -m "feat: add usage example demonstrating all modules"
```

---

### Task 10: Final Integration and Cleanup

**Files:**
- Modify: `CMakeLists.txt` (if needed)
- Create: `README.md`

**Interfaces:**
- All modules complete

- [ ] **Step 1: Verify all tests pass**

Run: `cmake --build build && ctest --test-dir build`
Expected: All tests pass

- [ ] **Step 2: Create README.md**

```markdown
# Mana Framework

A lightweight C++23 framework providing core infrastructure components.

## Features

- **Thread Pool**: Task queue with std::future support
- **Event Bus**: Type-safe publish-subscribe messaging
- **Context**: Thread-local context stack with any-type storage
- **Logger**: Configurable logging with std::format
- **File IO**: RAII-based file operations
- **Utilities**: Common string and timestamp functions

## Requirements

- MSVC 2019 16.10+
- C++23
- Windows

## Build

```bash
cmake -B build -S .
cmake --build build
```

## Usage

```cpp
#include <mana/mana.h>

int main() {
    mana::initialize();
    
    auto& logger = mana::logger();
    logger.info("Hello, Mana!");
    
    auto& pool = mana::thread_pool();
    auto future = pool.submit([]() { return 42; });
    logger.info("Result: {}", future.get());
    
    mana::shutdown();
    return 0;
}
```

## License

MIT
```

- [ ] **Step 3: Commit**

```bash
git add README.md
git commit -m "docs: add README with usage instructions"
```

- [ ] **Step 4: Tag release**

```bash
git tag -a v1.0.0 -m "Initial release of Mana framework"
```

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-07-10-mana-framework.md`. Two execution options:**

**1. Subagent-Driven (recommended)** - I dispatch a fresh subagent per task, review between tasks, fast iteration

**2. Inline Execution** - Execute tasks in this session using executing-plans, batch execution with checkpoints

**Which approach?**
