#pragma once

#include <string>
#include <vector>

namespace cw1 {

// ============================================================================
// Malaysia weekly retail fuel types.
// Only real Malaysian fuel grades are included:
//   RON95, RON97, Diesel (Peninsular), Diesel (East Malaysia).
// ============================================================================

enum class FuelSeries {
    RON95,
    RON97,
    DIESEL_PENINSULAR,
    DIESEL_EAST
};

inline const char* fuelSeriesToString(FuelSeries s) {
    switch (s) {
    case FuelSeries::RON95:             return "RON95";
    case FuelSeries::RON97:             return "RON97";
    case FuelSeries::DIESEL_PENINSULAR: return "Diesel (Peninsular)";
    case FuelSeries::DIESEL_EAST:       return "Diesel (East Malaysia)";
    }
    return "Unknown";
}

// One weekly fuel price record as published by the Malaysian government.
struct FuelWeeklyRecord {
    std::string effectiveDate;       // ISO date, e.g. "2025-03-20"
    double ron95            = 0.0;   // RM per litre
    double ron97            = 0.0;
    double dieselPeninsular = 0.0;
    double dieselEast       = 0.0;
    std::string source;              // e.g. "data.gov.my", "manual", "fallback"
    std::string fetchedAt;           // ISO timestamp of when the record was fetched
};

// Simple forecast result for a single fuel series.
struct FuelForecast {
    double currentPrice      = 0.0;
    double predictedNextPrice = 0.0;
    double delta             = 0.0;
    bool   increasing        = false;
    bool   stable            = true;   // |delta| < threshold
};

} // namespace cw1
