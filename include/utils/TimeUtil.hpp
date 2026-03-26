// Small helper for consistent ISO-8601 timestamps across blocks, audit entries, and persistence logs.

#pragma once

#include <string>

namespace cw1::TimeUtil {

// Returns the current local time in a stable string format for storage/display.
std::string nowIso8601();

}
