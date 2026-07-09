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
    class Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace mana::io
