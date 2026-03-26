// Shared data types for the fuel-price intelligence feature.

#pragma once

#include <string>
#include <vector>

namespace cw1 {

// The four fuel series shown in the dashboard chart and table.
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

struct FuelWeeklyRecord {
    std::string effectiveDate;
    double ron95            = 0.0;
    double ron97            = 0.0;
    double dieselPeninsular = 0.0;
    double dieselEast       = 0.0;
    std::string source;
    std::string fetchedAt;
};

struct FuelForecast {
    double currentPrice      = 0.0;
    double predictedNextPrice = 0.0;
    double delta             = 0.0;
    bool   increasing        = false;
    bool   stable            = true;
};

}
