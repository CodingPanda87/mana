#include <mana/io.h>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace mana::io {

class FileManager::Implementation {
public:
    std::mutex mutex_;
};

FileManager::FileManager() : impl_(std::make_unique<Implementation>()) {}

FileManager::~FileManager() = default;

std::optional<std::string> FileManager::read_all(const std::string& path) {
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    auto pos = file.tellg();
    if (pos == std::streampos(-1)) {
        return std::nullopt;
    }
    size_t size = static_cast<size_t>(pos);
    file.seekg(0, std::ios::beg);

    // Read content
    std::string content(size, '\0');
    file.read(content.data(), size);

    return content;
}

bool FileManager::write(const std::string& path, const std::string& content) {
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(content.data(), content.size());
    return file.good();
}

bool FileManager::exists(const std::string& path) {
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    return std::filesystem::exists(path);
}

std::optional<size_t> FileManager::file_size(const std::string& path) {
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    if (ec) {
        return std::nullopt;
    }
    return size;
}

} // namespace mana::io
