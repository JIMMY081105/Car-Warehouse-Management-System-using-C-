// Implements SQLite persistence for blocks and audit entries. The database is optional at runtime, but when enabled it preserves the richer coursework workflow across sessions and supports SQL-backed search.

#include "utils/DatabaseManager.hpp"

#include <sqlite3.h>

#include <algorithm>
#include <utility>

namespace cw1 {

DatabaseManager::DatabaseManager(const std::string& dbPath)
    : db_(nullptr), dbPath_(dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        return;
    }
    createTables();
}

DatabaseManager::~DatabaseManager() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

DatabaseManager::DatabaseManager(DatabaseManager&& other) noexcept
    : db_(other.db_),
      lastError_(std::move(other.lastError_)),
      dbPath_(std::move(other.dbPath_)) {
    other.db_ = nullptr;
}

DatabaseManager& DatabaseManager::operator=(DatabaseManager&& other) noexcept {
    if (this != &other) {
        if (db_ != nullptr) {
            sqlite3_close(db_);
        }
        db_ = other.db_;
        lastError_ = std::move(other.lastError_);
        dbPath_ = std::move(other.dbPath_);
        other.db_ = nullptr;
    }
    return *this;
}



bool DatabaseManager::isOpen() const noexcept {
    return db_ != nullptr;
}

std::string DatabaseManager::lastError() const {
    return lastError_;
}

sqlite3* DatabaseManager::rawHandle() noexcept {
    return db_;
}



void DatabaseManager::createTables() {
    lastError_.clear();

    // Separate tables keep block history and audit history queryable.
    const char* blocksSQL =
        "CREATE TABLE IF NOT EXISTS blocks ("
        "    block_index     INTEGER PRIMARY KEY,"
        "    current_hash    TEXT NOT NULL,"
        "    previous_hash   TEXT NOT NULL,"
        "    timestamp       TEXT NOT NULL,"
        "    nonce           INTEGER NOT NULL,"
        "    vin             TEXT NOT NULL,"
        "    manufacturer    TEXT NOT NULL,"
        "    model           TEXT NOT NULL,"
        "    color           TEXT NOT NULL,"
        "    production_year INTEGER NOT NULL,"
        "    stage           INTEGER NOT NULL,"
        "    factory_location    TEXT,"
        "    warehouse_location  TEXT,"
        "    received_by         TEXT,"
        "    supplier_id         TEXT,"
        "    inspector_id        TEXT,"
        "    passed              INTEGER,"
        "    qc_notes            TEXT,"
        "    dealer_id           TEXT,"
        "    destination         TEXT,"
        "    transport_mode      TEXT,"
        "    buyer_id            TEXT,"
        "    retailer_id         TEXT,"
        "    sale_price          REAL,"
        "    warranty_expiry     TEXT,"
        "    manufacturer_id     TEXT,"
        "    is_tombstone        INTEGER NOT NULL DEFAULT 0,"
        "    sha3_hash           TEXT"
        ")";

    const char* auditSQL =
        "CREATE TABLE IF NOT EXISTS audit_log ("
        "    id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    action    TEXT NOT NULL,"
        "    details   TEXT NOT NULL,"
        "    timestamp TEXT NOT NULL"
        ")";

    execSQL(blocksSQL);
    execSQL(auditSQL);

    if (!execSQL("ALTER TABLE blocks ADD COLUMN sha3_hash TEXT")) {
        if (lastError_.find("duplicate") != std::string::npos) {
            lastError_.clear();
        }
    }
}

bool DatabaseManager::execSQL(const std::string& sql) {
    if (db_ == nullptr) {
        lastError_ = "Database not open";
        return false;
    }
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        lastError_ = (errMsg != nullptr) ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        return false;
    }
    lastError_.clear();
    return true;
}



static bool bindBlockToStmt(sqlite3_stmt* stmt, const Block& block) {
    // Binding every payload field keeps the persisted row faithful to the block that was hashed and shown in the GUI.
    const CarRecord& r = block.getRecord();

    sqlite3_bind_int64(stmt, bind(BlockCol::Index),        static_cast<sqlite3_int64>(block.getIndex()));
    sqlite3_bind_text(stmt,  bind(BlockCol::CurrentHash),  block.getCurrentHash().c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::PreviousHash), block.getPreviousHash().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::Timestamp),    block.getTimestamp().c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, bind(BlockCol::Nonce),        static_cast<sqlite3_int64>(block.getNonce()));
    sqlite3_bind_text(stmt,  bind(BlockCol::Vin),              r.vin.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::Manufacturer),     r.manufacturer.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::Model),            r.model.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::Color),            r.color.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,   bind(BlockCol::ProductionYear),   r.productionYear);
    sqlite3_bind_int(stmt,   bind(BlockCol::Stage),            static_cast<int>(r.stage));
    sqlite3_bind_text(stmt,  bind(BlockCol::FactoryLocation),  r.factoryLocation.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::WarehouseLocation),r.warehouseLocation.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::ReceivedBy),       r.receivedBy.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::SupplierId),       r.supplierId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::InspectorId),      r.inspectorId.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,   bind(BlockCol::Passed),           r.passed ? 1 : 0);
    sqlite3_bind_text(stmt,  bind(BlockCol::QcNotes),          r.qcNotes.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::DealerId),         r.dealerId.c_str(),           -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::Destination),      r.destination.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::TransportMode),    r.transportMode.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::BuyerId),          r.buyerId.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::RetailerId),       r.retailerId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt,bind(BlockCol::SalePrice),        r.salePrice);
    sqlite3_bind_text(stmt,  bind(BlockCol::WarrantyExpiry),   r.warrantyExpiry.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  bind(BlockCol::ManufacturerId),   r.manufacturerId.c_str(),     -1, SQLITE_TRANSIENT);
    int isTombstone = (r.manufacturer == "[DELETED]") ? 1 : 0;
    sqlite3_bind_int(stmt,   bind(BlockCol::IsTombstone),      isTombstone);
    sqlite3_bind_text(stmt,  bind(BlockCol::Sha3Hash),         block.getSha3Hash().c_str(), -1, SQLITE_TRANSIENT);

    return true;
}

static const char* k_upsertSQL =
    "INSERT OR REPLACE INTO blocks ("
    "  block_index, current_hash, previous_hash, timestamp, nonce,"
    "  vin, manufacturer, model, color, production_year, stage,"
    "  factory_location, warehouse_location, received_by, supplier_id,"
    "  inspector_id, passed, qc_notes, dealer_id, destination,"
    "  transport_mode, buyer_id, retailer_id, sale_price,"
    "  warranty_expiry, manufacturer_id, is_tombstone, sha3_hash"
    ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

bool DatabaseManager::saveAllBlocks(const std::vector<Block>& chain) {
    lastError_.clear();
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, k_upsertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        execSQL("ROLLBACK");
        return false;
    }

    for (const auto& block : chain) {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        bindBlockToStmt(stmt, block);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            sqlite3_finalize(stmt);
            execSQL("ROLLBACK");
            return false;
        }
    }

    sqlite3_finalize(stmt);
    return execSQL("COMMIT");
}

std::vector<Block> DatabaseManager::loadAllBlocks() {
    lastError_.clear();
    std::vector<Block> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql = "SELECT * FROM blocks ORDER BY block_index ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        auto getInt    = [&](int col) { return sqlite3_column_int(stmt, col); };
        auto getInt64  = [&](int col) { return sqlite3_column_int64(stmt, col); };
        auto getText   = [&](int col) -> std::string {
            const unsigned char* t = sqlite3_column_text(stmt, col);
            return (t != nullptr) ? std::string(reinterpret_cast<const char*>(t)) : std::string();
        };
        auto getDouble = [&](int col) { return sqlite3_column_double(stmt, col); };

        std::size_t   index        = static_cast<std::size_t>(getInt64(col(BlockCol::Index)));
        std::string   currentHash  = getText(col(BlockCol::CurrentHash));
        std::string   previousHash = getText(col(BlockCol::PreviousHash));
        std::string   timestamp    = getText(col(BlockCol::Timestamp));
        std::uint64_t nonce        = static_cast<std::uint64_t>(getInt64(col(BlockCol::Nonce)));

        CarRecord rec;
        rec.vin              = getText(col(BlockCol::Vin));
        rec.manufacturer     = getText(col(BlockCol::Manufacturer));
        rec.model            = getText(col(BlockCol::Model));
        rec.color            = getText(col(BlockCol::Color));
        rec.productionYear   = getInt(col(BlockCol::ProductionYear));
        int stageInt         = getInt(col(BlockCol::Stage));
        if (stageInt >= static_cast<int>(BlockStage::PRODUCTION) &&
            stageInt <= static_cast<int>(BlockStage::CUSTOMER_SALE)) {
            rec.stage = static_cast<BlockStage>(stageInt);
        }
        rec.factoryLocation    = getText(col(BlockCol::FactoryLocation));
        rec.warehouseLocation  = getText(col(BlockCol::WarehouseLocation));
        rec.receivedBy         = getText(col(BlockCol::ReceivedBy));
        rec.supplierId         = getText(col(BlockCol::SupplierId));
        rec.inspectorId        = getText(col(BlockCol::InspectorId));
        rec.passed             = (getInt(col(BlockCol::Passed)) != 0);
        rec.qcNotes            = getText(col(BlockCol::QcNotes));
        rec.dealerId           = getText(col(BlockCol::DealerId));
        rec.destination        = getText(col(BlockCol::Destination));
        rec.transportMode      = getText(col(BlockCol::TransportMode));
        rec.buyerId            = getText(col(BlockCol::BuyerId));
        rec.retailerId         = getText(col(BlockCol::RetailerId));
        rec.salePrice          = getDouble(col(BlockCol::SalePrice));
        rec.warrantyExpiry     = getText(col(BlockCol::WarrantyExpiry));
        rec.manufacturerId     = getText(col(BlockCol::ManufacturerId));
        std::string sha3Hash;
        if (sqlite3_column_count(stmt) > col(BlockCol::Sha3Hash)) {
            sha3Hash = getText(col(BlockCol::Sha3Hash));
        }

        result.emplace_back(index,
                            std::move(previousHash),
                            std::move(currentHash),
                            std::move(timestamp),
                            nonce,
                            std::move(rec),
                            std::move(sha3Hash));
    }

    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        result.clear();
    }

    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::upsertBlock(const Block& block) {
    lastError_.clear();
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, k_upsertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    bindBlockToStmt(stmt, block);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::deleteBlockRow(std::size_t index) {
    lastError_.clear();
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;

    {
        const char* sql = "DELETE FROM blocks WHERE block_index = ?";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(index));
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
    }

    {
        const char* sql = "UPDATE blocks SET block_index = block_index - 1 "
                          "WHERE block_index > ?";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
        sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(index));
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }
    }

    return execSQL("COMMIT");
}



bool DatabaseManager::insertAuditEntry(AuditAction action,
                                       const std::string& details,
                                       const std::string& timestamp) {
    lastError_.clear();
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    const char* sql =
        "INSERT INTO audit_log (action, details, timestamp) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    std::string actionStr = actionToString(action);
    sqlite3_bind_text(stmt, 1, actionStr.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, details.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, timestamp.c_str(),  -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

std::vector<std::tuple<std::string, std::string, std::string>>
DatabaseManager::loadAuditLog() {
    lastError_.clear();
    std::vector<std::tuple<std::string, std::string, std::string>> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql = "SELECT action, details, timestamp FROM audit_log "
                      "ORDER BY id ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        auto getText = [&](int col) -> std::string {
            const unsigned char* t = sqlite3_column_text(stmt, col);
            return (t != nullptr) ? std::string(reinterpret_cast<const char*>(t))
                                  : std::string();
        };
        result.emplace_back(getText(0), getText(1), getText(2));
    }

    sqlite3_finalize(stmt);
    return result;
}



std::vector<std::size_t> DatabaseManager::searchBlocksSQL(const std::string& query) {
    lastError_.clear();
    std::vector<std::size_t> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql =
        "SELECT block_index FROM blocks WHERE "
        "LOWER(vin) LIKE LOWER(?) OR "
        "LOWER(manufacturer) LIKE LOWER(?) OR "
        "LOWER(model) LIKE LOWER(?) OR "
        "LOWER(color) LIKE LOWER(?) OR "
        "LOWER(destination) LIKE LOWER(?) OR "
        "LOWER(qc_notes) LIKE LOWER(?) "
        "ORDER BY block_index ASC";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    std::string pattern = "%" + query + "%";
    for (int i = 1; i <= 6; ++i) {
        sqlite3_bind_text(stmt, i, pattern.c_str(), -1, SQLITE_TRANSIENT);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        result.push_back(
            static_cast<std::size_t>(sqlite3_column_int64(stmt, 0)));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::size_t> DatabaseManager::getBlockIndicesByVin(const std::string& vin) {
    lastError_.clear();
    std::vector<std::size_t> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql =
        "SELECT block_index FROM blocks WHERE vin = ? ORDER BY block_index ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        result.push_back(
            static_cast<std::size_t>(sqlite3_column_int64(stmt, 0)));
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::string> DatabaseManager::getAllVinsSQL() {
    lastError_.clear();
    std::vector<std::string> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql = "SELECT DISTINCT vin FROM blocks ORDER BY vin ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return result;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* t = sqlite3_column_text(stmt, 0);
        if (t != nullptr) {
            result.emplace_back(reinterpret_cast<const char*>(t));
        }
    }

    sqlite3_finalize(stmt);
    return result;
}



bool DatabaseManager::fullResync(const std::vector<Block>& chain,
                                 const AuditLog& auditLog) {
    lastError_.clear();
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;
    if (!execSQL("DELETE FROM blocks"))    { execSQL("ROLLBACK"); return false; }
    if (!execSQL("DELETE FROM audit_log")) { execSQL("ROLLBACK"); return false; }

    {
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, k_upsertSQL, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }

        for (const auto& block : chain) {
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);
            bindBlockToStmt(stmt, block);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                lastError_ = sqlite3_errmsg(db_);
                sqlite3_finalize(stmt);
                execSQL("ROLLBACK");
                return false;
            }
        }
        sqlite3_finalize(stmt);
    }

    {
        const char* auditInsert =
            "INSERT INTO audit_log (action, details, timestamp) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, auditInsert, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            execSQL("ROLLBACK");
            return false;
        }

        const AuditEntry* entry = auditLog.head();
        while (entry != nullptr) {
            sqlite3_reset(stmt);
            sqlite3_clear_bindings(stmt);

            std::string actionStr = actionToString(entry->action);
            sqlite3_bind_text(stmt, 1, actionStr.c_str(),         -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, entry->details.c_str(),    -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, entry->timestamp.c_str(),  -1, SQLITE_TRANSIENT);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                lastError_ = sqlite3_errmsg(db_);
                sqlite3_finalize(stmt);
                execSQL("ROLLBACK");
                return false;
            }

            entry = entry->next;
        }
        sqlite3_finalize(stmt);
    }

    return execSQL("COMMIT");
}

}
