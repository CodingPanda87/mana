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

    // Move-only: unique_ptr makes copy implicitly deleted
    Context(Context&&) noexcept;
    Context& operator=(Context&&) noexcept;
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

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
