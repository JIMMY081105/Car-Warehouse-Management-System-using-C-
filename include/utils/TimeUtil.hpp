// Timestamp helper used across the app.

#pragma once

#include <string>

namespace cw1::TimeUtil {

// Returns the current local time in a stable string format for storage/display.
std::string nowIso8601();

}
