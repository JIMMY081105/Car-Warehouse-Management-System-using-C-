// Small timing helpers for GUI measurements.

#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace cw1 {

// Measures elapsed time for one operation using a steady clock.
class OperationTimer {
public:
    using Clock = std::chrono::steady_clock;

    OperationTimer() : start_(Clock::now()) {}

    double elapsedSeconds() const {
        const auto elapsed = Clock::now() - start_;
        return std::chrono::duration<double>(elapsed).count();
    }

private:
    Clock::time_point start_;
};

template <typename Callable>
double measureSeconds(Callable&& callable) {
    OperationTimer timer;
    std::forward<Callable>(callable)();
    return timer.elapsedSeconds();
}

inline std::string formatSeconds(double seconds) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4) << seconds;
    return out.str();
}

}
