#include "utils/TimeUtil.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace cw1::TimeUtil {

std::string nowIso8601() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm utcTime = {};
#ifdef _WIN32
    gmtime_s(&utcTime, &currentTime);
#else
    gmtime_r(&currentTime, &utcTime);
#endif

    std::ostringstream output;
    output << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

} // namespace cw1::TimeUtil
