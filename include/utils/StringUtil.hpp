#pragma once

#include <string>

namespace cw1 {

// Case-insensitive substring search.
// Returns true if needle appears anywhere inside haystack, ignoring case.
bool containsIgnoreCase(const std::string& haystack, const std::string& needle);

} // namespace cw1
