#include <mana/mana.h>
#include <optional>
#include <mutex>
#include <stdexcept>

namespace mana {

// Global framework state
std::optional<FrameworkState> g_state;
std::mutex g_mutex;
bool g_initialized = false;

FrameworkState& instance() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_state.has_value()) {
        throw std::runtime_error("Framework not initialized. Call mana::initialize() first.");
    }
    return *g_state;
}

void initialize() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_initialized) {
        return;
    }

    g_state.emplace();
    g_initialized = true;
}

void shutdown() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_initialized) {
        return;
    }

    g_state->thread_pool.wait_all();
    g_state.reset();
    g_initialized = false;
}

log::Logger& logger() {
    return instance().logger;
}

} // namespace mana
