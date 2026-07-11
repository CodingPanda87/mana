# Mana Framework Design Specification

**Date:** 2026-07-10
**Version:** 1.0
**Status:** Draft

## 1. Overview

Mana is a lightweight C++23 framework designed as a dynamic library (DLL) providing core infrastructure components for modern C++ applications. The framework follows a singleton pattern with a single instance per process.

### Design Philosophy

1. **Efficient**: Zero virtual functions, zero exception overhead (compile-time resolution where possible), stack allocation preferred, RAII resource management
2. **Minimal**: Extremely concise code, single-responsibility modules, no third-party dependencies, only C++23 standard library
3. **User-friendly**: Intuitive interfaces, descriptive English naming (no abbreviations), modern C++ idioms
4. **Compact**: Each module independently usable, no forced full-framework inclusion, header-as-documentation
5. **AI-friendly**: No non-standard abbreviations (e.g., use `Context` not `Ctx`, `Logger` not `Lgr`), clear namespace structure with `mana::module_name` pattern for AI intent inference

## 2. Architecture

### 2.1 Module Structure

```
mana/
├── mana.h             # Main header (includes all modules)
├── thread_pool.h      # Thread pool module
├── event.h        # Message bus module
├── context.h          # Context management module
├── log.h              # Logging module
├── io.h               # File IO module
├── util.h             # Utility functions
└── export.h           # DLL export macros
```

### 2.2 Namespace Structure

```cpp
namespace mana {
    // Direct module access functions
    thread_pool::ThreadPool& thread_pool();
    event::EventBus& event();
    context::Context& context();
    log::Logger& logger();
    io::FileManager& file_manager();
    
    // Framework lifecycle
    void initialize();
    void shutdown();
    
    // Module classes in sub-namespaces
    namespace thread_pool { class ThreadPool; }
    namespace event { class EventBus; }
    namespace context { class Context; }
    namespace log { class Logger; }
    namespace io { class FileManager; }
    namespace util { /* utility functions */ }
}
```

### 2.3 Framework Singleton

**API:**
```cpp
namespace mana {
    // Get framework instance (singleton) - returns internal state handle
    auto& instance();
    
    // Direct module access (preferred)
    thread_pool::ThreadPool& thread_pool();
    event::EventBus& event();
    context::Context& context();
    log::Logger& logger();
    io::FileManager& file_manager();
    
    // Framework lifecycle
    void initialize();
    void shutdown();
}
```

**Design Decisions:**
- Single instance per process (singleton pattern)
- Direct module access via `mana::module_name()` functions
- Lazy initialization of modules
- RAII-based lifecycle management
- No intermediate Framework class - simplified API

## 3. Module Specifications

### 3.1 Thread Pool Module

**Header:** `mana/thread_pool.h`

```cpp
namespace mana::thread_pool {

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // Submit task, returns std::future
    template<typename F, typename... Args>
    auto submit(F&& func, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>>;
    
    // Wait for all tasks to complete
    void wait_all();
    
    // Get pending task count
    size_t pending_count() const;
    
    // Get thread count
    size_t thread_count() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::thread_pool
```

**Implementation Requirements:**
- Use `std::condition_variable` for task notification
- Task queue using `std::queue<std::function<void()>>`
- Wrap tasks in `std::packaged_task` for future support
- Thread-safe with `std::mutex` protection
- Support task cancellation (optional enhancement)

### 3.2 Message Bus Module

**Header:** `mana/event.h`

```cpp
#include <concepts>

namespace mana::event {

// Event type concept (no base class required)
// Any copyable type can be used as an event
template<typename T>
concept EventType = std::copy_constructible<T>;

class EventBus {
public:
    EventBus();
    ~EventBus();
    
    // Subscribe to event (any copyable type)
    template<EventType T>
    size_t subscribe(std::function<void(const T&)> callback);
    
    // Unsubscribe
    template<EventType T>
    void unsubscribe(size_t subscription_id);
    
    // Publish event (async)
    template<EventType T>
    void publish(const T& event);
    
    // Publish event (sync, wait for all subscribers)
    template<EventType T>
    void publish_sync(const T& event);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::event
```

**Implementation Requirements:**
- Use `std::type_index` for type-safe message routing
- Separate subscriber lists per event type
- Support both sync and async publish modes
- Thread-safe with read-write lock optimization
- Use C++20 concepts for type constraints (no virtual functions)

### 3.3 Context Module

**Header:** `mana/context.h`

```cpp
namespace mana::context {

class Context {
public:
    Context();
    ~Context();
    
    // Store/retrieve any type value
    void put(const std::string& key, std::any value);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Remove key-value pair
    void remove(const std::string& key);
    
    // Clear context
    void clear();
    
    // Get all keys
    std::vector<std::string> keys() const;
};

// Global context stack management
void attach(Context context);
void detach();
Context& current();

} // namespace mana::context
```

**Implementation Requirements:**
- Thread-local context stack
- `std::any` for type-erased storage
- `std::optional` for safe retrieval
- Stack-based context management (attach/detach pattern)

### 3.4 Logging Module

**Header:** `mana/log.h`

```cpp
namespace mana::log {

enum class Level {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    Logger();
    ~Logger();
    
    // Set minimum log level
    void set_level(Level level);
    
    // Get current log level
    Level level() const;
    
    // Log message
    void log(Level level, const std::string& message);
    
    // Convenience functions
    template<typename... Args>
    void debug(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void info(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void warning(const std::string& format, Args&&... args);
    
    template<typename... Args>
    void error(const std::string& format, Args&&... args);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::log
```

**Implementation Requirements:**
- Use `std::format` for string formatting (C++20/23)
- Output to stderr by default
- Configurable log level filtering
- Thread-safe logging
- Optional: Custom output targets (file, callback)

### 3.5 IO Module

**Header:** `mana/io.h`

```cpp
namespace mana::io {

class FileManager {
public:
    FileManager();
    ~FileManager();
    
    // Read entire file
    std::optional<std::string> read_all(const std::string& path);
    
    // Write to file
    bool write(const std::string& path, const std::string& content);
    
    // Check if file exists
    bool exists(const std::string& path);
    
    // Get file size
    std::optional<size_t> file_size(const std::string& path);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mana::io
```

**Implementation Requirements:**
- RAII-based file handling
- `std::ifstream`/`std::ofstream` with proper error handling
- `std::optional` for "not found" cases
- Exceptions for actual I/O errors
- Binary and text mode support

### 3.6 Utility Module

**Header:** `mana/util.h`

```cpp
namespace mana::util {

// String split
std::vector<std::string> split(const std::string& str, const std::string& delimiter);

// Get current timestamp (milliseconds)
int64_t current_timestamp_ms();

// Get current timestamp (seconds)
int64_t current_timestamp_s();

// String trim (whitespace)
std::string trim(const std::string& str);

// String to lowercase
std::string to_lower(const std::string& str);

// String to uppercase
std::string to_upper(const std::string& str);

// Check if string starts with prefix
bool starts_with(const std::string& str, const std::string& prefix);

// Check if string ends with suffix
bool ends_with(const std::string& str, const std::string& suffix);

} // namespace mana::util
```

## 4. DLL Export Configuration

**Header:** `mana/export.h`

```cpp
#ifdef MANA_EXPORTS
    #define MANA_API __declspec(dllexport)
#else
    #define MANA_API __declspec(dllimport)
#endif

// Usage example
class MANA_API Framework { ... };
```

**Build Configuration:**
- Define `MANA_EXPORTS` when building the DLL
- Clients include headers without defining `MANA_EXPORTS`
- Use `.def` file or `__declspec` for symbol export

## 5. Error Handling Strategy

- **Not Found Cases**: Use `std::optional` (e.g., file not found, key not in context)
- **Actual Errors**: Throw exceptions (e.g., I/O failures, invalid arguments)
- **Noexcept**: Mark functions that cannot throw as `noexcept`
- **RAII**: Automatic resource cleanup on scope exit

## 6. Thread Safety

- **Framework Singleton**: Thread-safe initialization (C++11 magic statics)
- **Thread Pool**: Internal synchronization for task submission and execution
- **Event Bus**: Read-write locks for concurrent subscribe/publish
- **Context**: Thread-local storage for context stack
- **Logger**: Thread-safe message output

## 7. Usage Example

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
    
    // Shutdown framework
    mana::shutdown();
    return 0;
}
```

## 8. Build Requirements

- **Compiler**: MSVC 2019 16.10+ (for C++20 std::format and concepts support)
- **Standard**: C++23 (with C++20 concepts)
- **Platform**: Windows
- **Dependencies**: C++23 Standard Library only (including `<concepts>` header)

## 9. Future Enhancements

- Configuration file parsing
- Async logging with queue
- Task cancellation in thread pool
- Event filtering and routing
- Context serialization
- Custom memory allocators

## 10. Design Decisions Log

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Library Type | Dynamic Library (DLL) | Framework features require shared state |
| Singleton Pattern | Single instance per process | Centralized resource management |
| API Design | Direct module access | Simplified API, no intermediate classes |
| Error Handling | Optional + Exceptions | Modern C++ idioms, clear semantics |
| Thread Safety | Internal synchronization | Safe concurrent access |
| Naming Convention | Full English words | AI-friendly, descriptive |
| Namespace Structure | `mana::module_name` | Clear hierarchy, easy discovery |

---

**Document Author:** Claude (Brainstorming Mode)
**Last Updated:** 2026-07-10
