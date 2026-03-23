#include "utils/DatabaseManager.hpp"

#include <sqlite3.h>

#include <algorithm>
#include <utility>

namespace cw1 {

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Status helpers
// ---------------------------------------------------------------------------

bool DatabaseManager::isOpen() const noexcept {
    return db_ != nullptr;
}

std::string DatabaseManager::lastError() const {
    return lastError_;
}

// ---------------------------------------------------------------------------
// Table creation
// ---------------------------------------------------------------------------

void DatabaseManager::createTables() {
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

    // Migration: add sha3_hash column to existing databases that lack it.
    execSQL("ALTER TABLE blocks ADD COLUMN sha3_hash TEXT");
    // Ignore error if column already exists.
    lastError_.clear();
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
    return true;
}

// ---------------------------------------------------------------------------
// Block persistence
// ---------------------------------------------------------------------------

// Helper: bind all block columns to a prepared INSERT OR REPLACE statement.
static bool bindBlockToStmt(sqlite3_stmt* stmt, const Block& block) {
    const CarRecord& r = block.getRecord();

    sqlite3_bind_int64(stmt,  1, static_cast<sqlite3_int64>(block.getIndex()));
    sqlite3_bind_text(stmt,   2, block.getCurrentHash().c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   3, block.getPreviousHash().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   4, block.getTimestamp().c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt,  5, static_cast<sqlite3_int64>(block.getNonce()));
    sqlite3_bind_text(stmt,   6, r.vin.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   7, r.manufacturer.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   8, r.model.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   9, r.color.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,   10, r.productionYear);
    sqlite3_bind_int(stmt,   11, static_cast<int>(r.stage));
    sqlite3_bind_text(stmt,  12, r.factoryLocation.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  13, r.warehouseLocation.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  14, r.receivedBy.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  15, r.supplierId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  16, r.inspectorId.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,   17, r.passed ? 1 : 0);
    sqlite3_bind_text(stmt,  18, r.qcNotes.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  19, r.dealerId.c_str(),           -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  20, r.destination.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  21, r.transportMode.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  22, r.buyerId.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  23, r.retailerId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt,24, r.salePrice);
    sqlite3_bind_text(stmt,  25, r.warrantyExpiry.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,  26, r.manufacturerId.c_str(),     -1, SQLITE_TRANSIENT);
    // is_tombstone: true when manufacturer is "[DELETED]"
    int isTombstone = (r.manufacturer == "[DELETED]") ? 1 : 0;
    sqlite3_bind_int(stmt,   27, isTombstone);
    sqlite3_bind_text(stmt,  28, block.getSha3Hash().c_str(), -1, SQLITE_TRANSIENT);

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

        std::size_t   index        = static_cast<std::size_t>(getInt64(0));
        std::string   currentHash  = getText(1);
        std::string   previousHash = getText(2);
        std::string   timestamp    = getText(3);
        std::uint64_t nonce        = static_cast<std::uint64_t>(getInt64(4));

        CarRecord rec;
        rec.vin              = getText(5);
        rec.manufacturer     = getText(6);
        rec.model            = getText(7);
        rec.color            = getText(8);
        rec.productionYear   = getInt(9);
        int stageInt         = getInt(10);
        if (stageInt >= static_cast<int>(BlockStage::PRODUCTION) &&
            stageInt <= static_cast<int>(BlockStage::CUSTOMER_SALE)) {
            rec.stage = static_cast<BlockStage>(stageInt);
        }
        rec.factoryLocation    = getText(11);
        rec.warehouseLocation  = getText(12);
        rec.receivedBy         = getText(13);
        rec.supplierId         = getText(14);
        rec.inspectorId        = getText(15);
        rec.passed             = (getInt(16) != 0);
        rec.qcNotes            = getText(17);
        rec.dealerId           = getText(18);
        rec.destination        = getText(19);
        rec.transportMode      = getText(20);
        rec.buyerId            = getText(21);
        rec.retailerId         = getText(22);
        rec.salePrice          = getDouble(23);
        rec.warrantyExpiry     = getText(24);
        rec.manufacturerId     = getText(25);
        // column 26 = is_tombstone (informational only, already reflected in manufacturer)
        // column 27 = sha3_hash
        std::string sha3Hash;
        if (sqlite3_column_count(stmt) > 27) {
            sha3Hash = getText(27);
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
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;

    // Delete the target row.
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

    // Renumber all subsequent rows.
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

// ---------------------------------------------------------------------------
// Audit log persistence
// ---------------------------------------------------------------------------

bool DatabaseManager::insertAuditEntry(AuditAction action,
                                       const std::string& details,
                                       const std::string& timestamp) {
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

// ---------------------------------------------------------------------------
// SQL search
// ---------------------------------------------------------------------------

std::vector<std::size_t> DatabaseManager::searchBlocksSQL(const std::string& query) {
    std::vector<std::size_t> result;
    if (db_ == nullptr) { lastError_ = "Database not open"; return result; }

    const char* sql =
        "SELECT block_index FROM blocks WHERE "
        "LOWER(vin) LIKE LOWER(?) OR "
        "LOWER(manufacturer) LIKE LOWER(?) OR "
        "LOWER(model) LIKE LOWER(?) OR "
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
    for (int i = 1; i <= 5; ++i) {
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

// ---------------------------------------------------------------------------
// Full resync
// ---------------------------------------------------------------------------

bool DatabaseManager::fullResync(const std::vector<Block>& chain,
                                 const AuditLog& auditLog) {
    if (db_ == nullptr) { lastError_ = "Database not open"; return false; }

    if (!execSQL("BEGIN TRANSACTION")) return false;
    if (!execSQL("DELETE FROM blocks"))    { execSQL("ROLLBACK"); return false; }
    if (!execSQL("DELETE FROM audit_log")) { execSQL("ROLLBACK"); return false; }

    // Re-insert all blocks.
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

    // Re-insert all audit entries by traversing the linked list.
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

} // namespace cw1
