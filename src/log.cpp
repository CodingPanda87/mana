#include <mana/log.h>
#include <mana/util.h>
#include <iostream>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace mana::log {

class Logger::Implementation {
public:
    std::atomic<Level> level{Level::Info};
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

    std::string format_timestamp() const {
        int64_t timestamp_ms = mana::util::current_timestamp_ms();
        auto time_s = static_cast<time_t>(timestamp_ms / 1000);
        auto ms = timestamp_ms % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_s), "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms;
        return oss.str();
    }
};

Logger::Logger() : impl_(std::make_unique<Implementation>()) {}

Logger::~Logger() = default;

void Logger::set_level(Level level) {
    impl_->level.store(level);
}

Level Logger::level() const {
    return impl_->level.load();
}

void Logger::log(Level level, const std::string& message) {
    if (level < impl_->level.load()) {
        return;
    }

    std::lock_guard<std::mutex> lock(impl_->mutex);
    std::cerr << "[" << impl_->format_timestamp() << "] "
              << "[" << impl_->level_string(level) << "] "
              << message << std::endl;
}

} // namespace mana::log
