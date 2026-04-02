// Declares the fuel-price intelligence module. This is one of the coursework's extra features: it stores weekly fuel prices, forecasts trends, and helps the GUI estimate delivery-cost implications for logistics decisions.

#pragma once

#include <string>
#include <vector>

#include "utils/FuelPriceTypes.hpp"

struct sqlite3;

namespace cw1 {

// Manages the optional fuel-price dataset that enriches the dashboard.
class FuelPriceManager {
public:
    FuelPriceManager();
    ~FuelPriceManager();

    // Opens a dedicated fuel_prices.db (separate from the blockchain DB).
    bool openDatabase(const std::string& path);

    void attach(sqlite3* db);

    bool isAttached() const noexcept;

    // Loads weekly price history from SQLite so the GUI can plot it.
    bool insertRecord(const FuelWeeklyRecord& rec);

    int loadRecentHistory(int maxWeeks = 16);

    const std::vector<FuelWeeklyRecord>& history() const noexcept;

    std::string latestEffectiveDate() const;

    // Produces a small trend/forecast summary for the selected fuel series.
    FuelForecast forecast(FuelSeries series) const;

    // Coursework-style helper that turns fuel price changes into a delivery cost estimate for the warehouse logistics narrative.
    static double EstimateDeliveryCost(double fuelPricePerLitre,
                                       double distanceMiles = 10.0,
                                       double milesPerLitre  = 3.0);

    // Seeds sensible demo data so the extraordinary dashboard feature works even on a fresh database.
    void seedFallbackDataIfEmpty();



    std::string lastError() const;
    std::string statusText() const;

private:
    std::vector<double> extractSeries(FuelSeries series) const;

    static double ForecastNextWeekPrice(const std::vector<double>& prices);

    static void ClassifyTrend(double delta, bool& increasing, bool& stable);

    sqlite3* db_ = nullptr;
    bool ownsDb_ = false;   // true when we opened our own file
    std::vector<FuelWeeklyRecord> history_;
    std::string lastError_;
    std::string statusText_;

    void createFuelTables();
};

}
