#pragma once

#include <string>
#include <vector>

#include "utils/FuelPriceTypes.hpp"

struct sqlite3;

namespace cw1 {

// ============================================================================
// FuelPriceManager
// ============================================================================
// Manages Malaysia weekly retail fuel prices:
//   - SQLite storage (fuel_prices table)
//   - In-memory cache for fast UI access
//   - Simple trend-based forecasting (no ML)
//   - Delivery cost estimation for logistics
//
// Design:
//   The manager owns its own sqlite3* connection (shared with the main DB file)
//   via a raw pointer passed in from the DatabaseManager.  It does NOT own or
//   close the connection -- that remains the DatabaseManager's responsibility.
// ============================================================================

class FuelPriceManager {
public:
    FuelPriceManager();

    // Attach to an existing open sqlite3 database handle.
    // Creates the fuel_prices table if it does not exist.
    void attach(sqlite3* db);

    // Returns true if attached to a database.
    bool isAttached() const noexcept;

    // ---- Data loading / saving ----

    // Insert a weekly record.  Ignores duplicates for the same effective_date.
    bool insertRecord(const FuelWeeklyRecord& rec);

    // Load the most recent N weekly records from DB into the in-memory cache.
    // Returns the number of records loaded.
    int loadRecentHistory(int maxWeeks = 16);

    // Return the cached history (most recent last).
    const std::vector<FuelWeeklyRecord>& history() const noexcept;

    // Return the latest effective date in the DB, or "" if none.
    std::string latestEffectiveDate() const;

    // ---- Forecasting ----

    // Lightweight trend extrapolation for a given fuel series.
    // Uses the cached history.  See ForecastNextWeekPrice() for the formula.
    FuelForecast forecast(FuelSeries series) const;

    // ---- Delivery cost estimation ----

    // Estimate truck fuel cost for a given distance and efficiency.
    //   litres_used = distanceMiles / milesPerLitre
    //   cost = litres_used * fuelPricePerLitre
    static double EstimateDeliveryCost(double fuelPricePerLitre,
                                       double distanceMiles = 10.0,
                                       double milesPerLitre  = 3.0);

    // ---- Fallback data ----

    // Seed the DB with representative Malaysia fuel data if the table is empty.
    // Marked clearly as fallback/demo data.
    void seedFallbackDataIfEmpty();

    // ---- Status ----

    std::string lastError() const;
    std::string statusText() const;

private:
    // Extract a price series vector from the cached history.
    std::vector<double> extractSeries(FuelSeries series) const;

    // Predict next week's price from a price series.
    // Lightweight momentum model:
    //   size==0 -> 0
    //   size==1 -> last
    //   size==2 -> last + (last - prev)
    //   size>=3 -> last + avg of last two deltas
    // Clamped to >= 0.
    static double ForecastNextWeekPrice(const std::vector<double>& prices);

    // Classify trend direction.
    static void ClassifyTrend(double delta, bool& increasing, bool& stable);

    sqlite3* db_ = nullptr;
    std::vector<FuelWeeklyRecord> history_;
    std::string lastError_;
    std::string statusText_;

    void createFuelTables();
};

} // namespace cw1
