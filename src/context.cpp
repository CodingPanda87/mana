#include <mana/context.h>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace mana::context {

class Context::Implementation {
public:
    mutable std::mutex mutex;
    std::unordered_map<std::string, std::any> storage;
};

Context::Context() : impl_(std::make_unique<Implementation>()) {}

Context::~Context() = default;

Context::Context(Context&&) noexcept = default;
Context& Context::operator=(Context&&) noexcept = default;

void Context::put(const std::string& key, std::any value) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->storage[key] = std::move(value);
}

template<typename T>
std::optional<T> Context::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
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
    std::lock_guard<std::mutex> lock(impl_->mutex);
    return impl_->storage.find(key) != impl_->storage.end();
}

void Context::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->storage.erase(key);
}

void Context::clear() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->storage.clear();
}

std::vector<std::string> Context::keys() const {
    std::lock_guard<std::mutex> lock(impl_->mutex);
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
        context_stack.push_back(Context());
    }
    return context_stack.back();
}

} // namespace mana::context
