#include <mana/io.h>
#include <fstream>
#include <filesystem>

namespace mana::io {

class FileManager::Implementation {
public:
    // Implementation details
};

FileManager::FileManager() : impl_(std::make_unique<Implementation>()) {}

FileManager::~FileManager() = default;

std::optional<std::string> FileManager::read_all(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read content
    std::string content(size, '\0');
    file.read(content.data(), size);

    return content;
}

bool FileManager::write(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(content.data(), content.size());
    return file.good();
}

bool FileManager::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::optional<size_t> FileManager::file_size(const std::string& path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    if (ec) {
        return std::nullopt;
    }
    return size;
}

} // namespace mana::io
