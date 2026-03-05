#pragma once

#include <string>

namespace cw1::TimeUtil {

/// Return the current UTC time as an ISO-8601 string (e.g. "2026-03-02T12:30:00Z").
std::string nowIso8601();

} // namespace cw1::TimeUtil
