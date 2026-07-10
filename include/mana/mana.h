#pragma once

#include "export.h"
#include "thread_pool.h"
#include "event_bus.h"
#include "context.h"
#include "log.h"
#include "io.h"
#include "util.h"

namespace mana {

// Framework state (internal)
struct MANA_API FrameworkState {
    event_bus::EventBus event_bus;
    context::Context context;
    log::Logger logger;
    io::FileManager file_manager;
    thread_pool::ThreadPool thread_pool;  // Destroyed first - joins threads before other modules die

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

// Direct module access
// For thread_pool, event_bus, context: use instance().member (names conflict with namespaces)
MANA_API log::Logger& logger();
MANA_API io::FileManager& file_manager();

} // namespace mana
