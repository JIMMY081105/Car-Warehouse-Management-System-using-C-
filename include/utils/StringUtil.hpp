// Declares small string helpers shared by search and filtering code.

#pragma once

#include <string>

namespace cw1 {

// Case-insensitive substring match used by the in-memory blockchain search.
bool containsIgnoreCase(const std::string& haystack, const std::string& needle);

}
