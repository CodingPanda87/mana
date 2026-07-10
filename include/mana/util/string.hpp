#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <sstream>

namespace mana::util::string {

inline std::vector<std::string> split(std::string_view str, std::string_view delimiter) {
    std::vector<std::string> result;
    if (delimiter.empty()) {
        result.emplace_back(str);
        return result;
    }
    if (str.empty()) {
        result.emplace_back("");
        return result;
    }

    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string_view::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    result.emplace_back(str.substr(start));
    return result;
}

inline std::string trim(std::string_view str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }
    if (start == str.end()) {
        return "";
    }
    auto end = str.end() - 1;
    while (end != start && std::isspace(static_cast<unsigned char>(*end))) {
        --end;
    }
    return std::string(start, end + 1);
}

inline std::string to_lower(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

inline std::string to_upper(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

inline bool starts_with(std::string_view str, std::string_view prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

inline bool ends_with(std::string_view str, std::string_view suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

inline bool contains(std::string_view str, std::string_view sub) {
    return str.find(sub) != std::string_view::npos;
}

inline std::string replace(std::string_view str, std::string_view from, std::string_view to) {
    if (from.empty()) {
        return std::string(str);
    }
    std::string result(str);
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

inline std::string join(const std::vector<std::string>& parts, std::string_view delimiter) {
    if (parts.empty()) {
        return "";
    }
    std::ostringstream oss;
    oss << parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        oss << delimiter << parts[i];
    }
    return oss.str();
}

} // namespace mana::util::string
