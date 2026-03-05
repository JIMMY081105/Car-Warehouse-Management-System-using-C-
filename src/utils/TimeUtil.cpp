#include "utils/TimeUtil.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

namespace cw1::TimeUtil {

string nowIso8601() {
    const auto now = chrono::system_clock::now();
    const time_t currentTime = chrono::system_clock::to_time_t(now);

    tm utcTime = {};
#ifdef _WIN32
    gmtime_s(&utcTime, &currentTime);
#else
    gmtime_r(&currentTime, &utcTime);
#endif

    ostringstream output;
    output << put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

} // namespace cw1::TimeUtil
