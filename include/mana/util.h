#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "export.h"

namespace mana::util {

// Split string by delimiter
MANA_API std::vector<std::string> split(const std::string& str, const std::string& delimiter);

// Get current timestamp in milliseconds
MANA_API int64_t current_timestamp_ms();

// Get current timestamp in seconds
MANA_API int64_t current_timestamp_s();

// Trim whitespace from both ends
MANA_API std::string trim(const std::string& str);

// Convert string to lowercase
MANA_API std::string to_lower(const std::string& str);

// Convert string to uppercase
MANA_API std::string to_upper(const std::string& str);

// Check if string starts with prefix
MANA_API bool starts_with(const std::string& str, const std::string& prefix);

// Check if string ends with suffix
MANA_API bool ends_with(const std::string& str, const std::string& suffix);

} // namespace mana::util
