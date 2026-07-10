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

    auto& pool = mana::instance().thread_pool;
    auto future = pool.submit([]() { return 42; });
    logger.info("Result: {}", future.get());

    mana::shutdown();
    return 0;
}
```

## Testing

```bash
ctest --test-dir build -C Debug
```

## License

Apache 2.0
