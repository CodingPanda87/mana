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
    class Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace mana::log
