// Implements the fuel-price intelligence feature. It persists weekly fuel prices, loads chart history for the dashboard, and provides a simple forecast to strengthen the coursework's extra functionality.

#include "utils/FuelPriceManager.hpp"

#include <sqlite3.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace cw1 {

FuelPriceManager::FuelPriceManager() = default;

void FuelPriceManager::attach(sqlite3* db) {
    db_ = db;
    if (db_ != nullptr) {
        createFuelTables();
    }
}

bool FuelPriceManager::isAttached() const noexcept {
    return db_ != nullptr;
}




void FuelPriceManager::createFuelTables() {
    if (db_ == nullptr) return;

    // A dedicated table keeps the logistics intelligence feature separate from the blockchain tables while still sharing the same SQLite database.
    const char* sql =
        "CREATE TABLE IF NOT EXISTS fuel_prices ("
        "  id                INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  effective_date    TEXT    NOT NULL,"
        "  ron95             REAL    NOT NULL,"
        "  ron97             REAL    NOT NULL,"
        "  diesel_peninsular REAL    NOT NULL,"
        "  diesel_east       REAL    NOT NULL,"
        "  source            TEXT    NOT NULL,"
        "  fetched_at        TEXT    NOT NULL,"
        "  UNIQUE(effective_date)"
        ")";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        lastError_ = (errMsg != nullptr) ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
    }
}




static std::string nowISO() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buf);
}




bool FuelPriceManager::insertRecord(const FuelWeeklyRecord& rec) {
    if (db_ == nullptr) { lastError_ = "Not attached"; return false; }

    // Basic validation prevents obviously invalid records from distorting the dashboard chart or forecast.
    if (rec.ron95 <= 0 || rec.ron97 <= 0 ||
        rec.dieselPeninsular <= 0 || rec.dieselEast <= 0) {
        lastError_ = "Rejected: one or more fuel prices are zero or negative";
        return false;
    }

    const char* sql =
        "INSERT OR IGNORE INTO fuel_prices "
        "(effective_date, ron95, ron97, diesel_peninsular, diesel_east, source, fetched_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    std::string fetchedAt = rec.fetchedAt.empty() ? nowISO() : rec.fetchedAt;

    sqlite3_bind_text(stmt,   1, rec.effectiveDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, rec.ron95);
    sqlite3_bind_double(stmt, 3, rec.ron97);
    sqlite3_bind_double(stmt, 4, rec.dieselPeninsular);
    sqlite3_bind_double(stmt, 5, rec.dieselEast);
    sqlite3_bind_text(stmt,   6, rec.source.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   7, fetchedAt.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }
    return true;
}

int FuelPriceManager::loadRecentHistory(int maxWeeks) {
    history_.clear();
    if (db_ == nullptr) { lastError_ = "Not attached"; return 0; }

    // The query loads newest rows first for efficiency, then reverses them so the chart renders from oldest to newest on screen.
    const char* sql =
        "SELECT effective_date, ron95, ron97, diesel_peninsular, diesel_east, "
        "       source, fetched_at "
        "FROM fuel_prices "
        "ORDER BY effective_date DESC LIMIT ?";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return 0;
    }

    sqlite3_bind_int(stmt, 1, maxWeeks);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        FuelWeeklyRecord r;
        auto getText = [&](int col) -> std::string {
            const unsigned char* t = sqlite3_column_text(stmt, col);
            return (t != nullptr) ? std::string(reinterpret_cast<const char*>(t))
                                  : std::string();
        };
        r.effectiveDate    = getText(0);
        r.ron95            = sqlite3_column_double(stmt, 1);
        r.ron97            = sqlite3_column_double(stmt, 2);
        r.dieselPeninsular = sqlite3_column_double(stmt, 3);
        r.dieselEast       = sqlite3_column_double(stmt, 4);
        r.source           = getText(5);
        r.fetchedAt        = getText(6);
        history_.push_back(r);
    }
    sqlite3_finalize(stmt);


    std::reverse(history_.begin(), history_.end());

    if (!history_.empty()) {
        statusText_ = "Loaded " + std::to_string(history_.size()) +
                      " weeks | Latest: " + history_.back().effectiveDate;
    } else {
        statusText_ = "No fuel price data available";
    }

    return static_cast<int>(history_.size());
}

const std::vector<FuelWeeklyRecord>& FuelPriceManager::history() const noexcept {
    return history_;
}

std::string FuelPriceManager::latestEffectiveDate() const {
    if (db_ == nullptr) return "";

    const char* sql =
        "SELECT MAX(effective_date) FROM fuel_prices";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return "";

    std::string result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* t = sqlite3_column_text(stmt, 0);
        if (t != nullptr) result = reinterpret_cast<const char*>(t);
    }
    sqlite3_finalize(stmt);
    return result;
}





std::vector<double> FuelPriceManager::extractSeries(FuelSeries series) const {
    std::vector<double> prices;
    prices.reserve(history_.size());
    for (const auto& rec : history_) {
        switch (series) {
        case FuelSeries::RON95:             prices.push_back(rec.ron95); break;
        case FuelSeries::RON97:             prices.push_back(rec.ron97); break;
        case FuelSeries::DIESEL_PENINSULAR: prices.push_back(rec.dieselPeninsular); break;
        case FuelSeries::DIESEL_EAST:       prices.push_back(rec.dieselEast); break;
        }
    }
    return prices;
}

















double FuelPriceManager::ForecastNextWeekPrice(const std::vector<double>& prices) {
    const size_t n = prices.size();
    if (n == 0) return 0.0;
    if (n == 1) return prices.back();

    if (n == 2) {
        double prediction = prices[1] + (prices[1] - prices[0]);
        return std::max(0.0, prediction);
    }


    double trend1 = prices[n - 1] - prices[n - 2];
    double trend2 = prices[n - 2] - prices[n - 3];
    double avgTrend = (trend1 + trend2) / 2.0;
    double prediction = prices[n - 1] + avgTrend;
    return std::max(0.0, prediction);
}





void FuelPriceManager::ClassifyTrend(double delta, bool& increasing, bool& stable) {
    constexpr double kStableThreshold = 0.01;
    stable = (std::fabs(delta) < kStableThreshold);
    increasing = (!stable && delta > 0);
}

FuelForecast FuelPriceManager::forecast(FuelSeries series) const {
    FuelForecast f;
    std::vector<double> prices = extractSeries(series);
    if (prices.empty()) return f;

    f.currentPrice      = prices.back();
    f.predictedNextPrice = ForecastNextWeekPrice(prices);
    f.delta             = f.predictedNextPrice - f.currentPrice;
    ClassifyTrend(f.delta, f.increasing, f.stable);
    return f;
}












double FuelPriceManager::EstimateDeliveryCost(double fuelPricePerLitre,
                                               double distanceMiles,
                                               double milesPerLitre) {
    if (milesPerLitre <= 0.0) return 0.0;
    double litresUsed = distanceMiles / milesPerLitre;
    return litresUsed * fuelPricePerLitre;
}










void FuelPriceManager::seedFallbackDataIfEmpty() {
    if (db_ == nullptr) return;


    {
        const char* sql = "SELECT COUNT(*) FROM fuel_prices";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            if (count > 0) return;
        } else {
            sqlite3_finalize(stmt);
            return;
        }
    }






    struct FallbackEntry {
        const char* date;
        double ron95, ron97, dieselPen, dieselEast;
    };

    static const FallbackEntry fallback[] = {
        {"2025-01-02", 2.05, 3.47, 3.35, 3.35},
        {"2025-01-09", 2.05, 3.47, 3.35, 3.35},
        {"2025-01-16", 2.05, 3.35, 3.23, 3.23},
        {"2025-01-23", 2.05, 3.22, 3.13, 3.13},
        {"2025-01-30", 2.05, 3.22, 3.05, 3.05},
        {"2025-02-06", 2.05, 3.17, 2.95, 2.95},
        {"2025-02-13", 2.05, 3.10, 2.88, 2.88},
        {"2025-02-20", 2.05, 3.10, 2.85, 2.85},
        {"2025-02-27", 2.05, 3.04, 2.80, 2.80},
        {"2025-03-06", 2.05, 3.04, 2.77, 2.77},
        {"2025-03-13", 2.05, 2.99, 2.72, 2.72},
        {"2025-03-20", 2.05, 2.93, 2.65, 2.65},
    };

    std::string fetchedNow = nowISO();

    for (const auto& e : fallback) {
        FuelWeeklyRecord rec;
        rec.effectiveDate    = e.date;
        rec.ron95            = e.ron95;
        rec.ron97            = e.ron97;
        rec.dieselPeninsular = e.dieselPen;
        rec.dieselEast       = e.dieselEast;
        rec.source           = "fallback";
        rec.fetchedAt        = fetchedNow;
        insertRecord(rec);
    }

    statusText_ = "Seeded with fallback Malaysia fuel price data";
}




std::string FuelPriceManager::lastError() const {
    return lastError_;
}

std::string FuelPriceManager::statusText() const {
    return statusText_;
}

}
