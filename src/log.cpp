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
