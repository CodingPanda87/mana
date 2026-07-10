#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <fstream>
#include <filesystem>

namespace mana::util::io {

inline std::optional<std::string> read_all(std::string_view path) {
    std::ifstream file(std::string(path), std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    if (size < 0) {
        return std::nullopt;
    }
    file.seekg(0, std::ios::beg);
    std::string content(static_cast<size_t>(size), '\0');
    file.read(content.data(), size);
    return content;
}

inline bool write(std::string_view path, std::string_view content) {
    std::ofstream file(std::string(path), std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(content.data(), static_cast<std::streamsize>(content.size()));
    return file.good();
}

inline bool append(std::string_view path, std::string_view content) {
    std::ofstream file(std::string(path), std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    file.write(content.data(), static_cast<std::streamsize>(content.size()));
    return file.good();
}

inline bool exists(std::string_view path) {
    return std::filesystem::exists(path);
}

inline std::optional<size_t> file_size(std::string_view path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(std::string(path), ec);
    if (ec) {
        return std::nullopt;
    }
    return size;
}

inline std::vector<std::string> read_lines(std::string_view path) {
    std::vector<std::string> lines;
    std::ifstream file{std::string(path)};
    if (!file.is_open()) {
        return lines;
    }
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(std::move(line));
    }
    return lines;
}

} // namespace mana::util::io
