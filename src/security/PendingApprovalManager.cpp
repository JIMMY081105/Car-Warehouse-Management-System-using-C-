// Implements the pending block approval workflow with SQLite persistence.
// Requests flow through three states: PENDING -> APPROVED or REJECTED.
// Only approved requests are committed to the authoritative blockchain.

#include "security/PendingApprovalManager.hpp"

#include <sqlite3.h>
#include <algorithm>

#include "utils/TimeUtil.hpp"

namespace cw1 {

// ---------------------------------------------------------------------------
// RequestStatus helpers
// ---------------------------------------------------------------------------

std::string requestStatusToString(RequestStatus status) {
    switch (status) {
        case RequestStatus::PENDING:  return "Pending";
        case RequestStatus::APPROVED: return "Approved";
        case RequestStatus::REJECTED: return "Rejected";
    }
    return "Unknown";
}

// ---------------------------------------------------------------------------
// PendingApprovalManager implementation
// ---------------------------------------------------------------------------

PendingApprovalManager::PendingApprovalManager() = default;

void PendingApprovalManager::attach(sqlite3* db) {
    db_ = db;
    if (db_ != nullptr) {
        createTables();
        loadFromDB();
    }
}

bool PendingApprovalManager::isAttached() const noexcept {
    return db_ != nullptr;
}

void PendingApprovalManager::createTables() {
    if (db_ == nullptr) return;

    // The pending_requests table stores every submission regardless of outcome
    // so the approval history is always available for audit review.
    const char* sql =
        "CREATE TABLE IF NOT EXISTS pending_requests ("
        "    request_id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    requested_by       TEXT NOT NULL,"
        "    requested_by_display TEXT NOT NULL,"
        "    request_timestamp  TEXT NOT NULL,"
        "    requested_stage    INTEGER NOT NULL,"
        "    vin                TEXT NOT NULL,"
        "    manufacturer       TEXT NOT NULL,"
        "    model              TEXT NOT NULL,"
        "    color              TEXT NOT NULL,"
        "    production_year    INTEGER NOT NULL,"
        "    manufacturer_id    TEXT,"
        "    factory_location   TEXT,"
        "    warehouse_location TEXT,"
        "    received_by        TEXT,"
        "    supplier_id        TEXT,"
        "    inspector_id       TEXT,"
        "    passed             INTEGER,"
        "    qc_notes           TEXT,"
        "    dealer_id          TEXT,"
        "    destination        TEXT,"
        "    transport_mode     TEXT,"
        "    buyer_id           TEXT,"
        "    retailer_id        TEXT,"
        "    sale_price         REAL,"
        "    warranty_expiry    TEXT,"
        "    status             INTEGER NOT NULL DEFAULT 0,"
        "    approved_by        TEXT,"
        "    approved_timestamp TEXT,"
        "    reject_reason      TEXT,"
        "    creator_signature  TEXT"
        ")";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        lastError_ = (errMsg != nullptr) ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
    }
}

int PendingApprovalManager::submitRequest(const PendingBlockRequest& request) {
    // Assign a local ID and persist to the database. The database auto-increment
    // will provide the authoritative ID if SQLite is connected.
    PendingBlockRequest req = request;
    req.status = RequestStatus::PENDING;

    if (req.requestTimestamp.empty()) {
        req.requestTimestamp = TimeUtil::nowIso8601();
    }

    if (db_ != nullptr) {
        // Insert and let SQLite assign the ID via AUTOINCREMENT.
        if (!saveRequestToDB(req)) {
            return -1;
        }
        // Retrieve the last inserted rowid as the request ID.
        req.requestId = static_cast<int>(sqlite3_last_insert_rowid(db_));
    } else {
        req.requestId = nextId_++;
    }

    requests_.push_back(req);
    return req.requestId;
}

bool PendingApprovalManager::approveRequest(int requestId,
                                            const std::string& approverUsername) {
    for (auto& req : requests_) {
        if (req.requestId == requestId && req.status == RequestStatus::PENDING) {
            req.status = RequestStatus::APPROVED;
            req.approvedBy = approverUsername;
            req.approvedTimestamp = TimeUtil::nowIso8601();

            if (db_ != nullptr) {
                return updateRequestInDB(req);
            }
            return true;
        }
    }
    lastError_ = "Request not found or not in pending state.";
    return false;
}

bool PendingApprovalManager::rejectRequest(int requestId,
                                           const std::string& approverUsername,
                                           const std::string& reason) {
    for (auto& req : requests_) {
        if (req.requestId == requestId && req.status == RequestStatus::PENDING) {
            req.status = RequestStatus::REJECTED;
            req.approvedBy = approverUsername;
            req.approvedTimestamp = TimeUtil::nowIso8601();
            req.rejectReason = reason;

            if (db_ != nullptr) {
                return updateRequestInDB(req);
            }
            return true;
        }
    }
    lastError_ = "Request not found or not in pending state.";
    return false;
}

const std::vector<PendingBlockRequest>& PendingApprovalManager::getAllRequests() const noexcept {
    return requests_;
}

std::vector<PendingBlockRequest> PendingApprovalManager::getPendingRequests() const {
    std::vector<PendingBlockRequest> result;
    for (const auto& req : requests_) {
        if (req.status == RequestStatus::PENDING) {
            result.push_back(req);
        }
    }
    return result;
}

const PendingBlockRequest* PendingApprovalManager::getRequest(int requestId) const {
    for (const auto& req : requests_) {
        if (req.requestId == requestId) {
            return &req;
        }
    }
    return nullptr;
}

int PendingApprovalManager::pendingCount() const {
    int count = 0;
    for (const auto& req : requests_) {
        if (req.status == RequestStatus::PENDING) {
            ++count;
        }
    }
    return count;
}

std::string PendingApprovalManager::lastError() const {
    return lastError_;
}

// ---------------------------------------------------------------------------
// SQLite persistence
// ---------------------------------------------------------------------------

bool PendingApprovalManager::saveRequestToDB(const PendingBlockRequest& req) {
    if (db_ == nullptr) { lastError_ = "Database not attached"; return false; }

    const char* sql =
        "INSERT INTO pending_requests ("
        "  requested_by, requested_by_display, request_timestamp, requested_stage,"
        "  vin, manufacturer, model, color, production_year, manufacturer_id,"
        "  factory_location, warehouse_location, received_by, supplier_id,"
        "  inspector_id, passed, qc_notes, dealer_id, destination,"
        "  transport_mode, buyer_id, retailer_id, sale_price, warranty_expiry,"
        "  status, approved_by, approved_timestamp, reject_reason, creator_signature"
        ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    const CarRecord& r = req.payload;

    sqlite3_bind_text(stmt,   1,  req.requestedBy.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   2,  req.requestedByDisplay.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   3,  req.requestTimestamp.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,    4,  static_cast<int>(req.requestedStage));
    sqlite3_bind_text(stmt,   5,  r.vin.c_str(),                 -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   6,  r.manufacturer.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   7,  r.model.c_str(),               -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   8,  r.color.c_str(),               -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,    9,  r.productionYear);
    sqlite3_bind_text(stmt,   10, r.manufacturerId.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   11, r.factoryLocation.c_str(),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   12, r.warehouseLocation.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   13, r.receivedBy.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   14, r.supplierId.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   15, r.inspectorId.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,    16, r.passed ? 1 : 0);
    sqlite3_bind_text(stmt,   17, r.qcNotes.c_str(),             -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   18, r.dealerId.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   19, r.destination.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   20, r.transportMode.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   21, r.buyerId.c_str(),             -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   22, r.retailerId.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 23, r.salePrice);
    sqlite3_bind_text(stmt,   24, r.warrantyExpiry.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,    25, static_cast<int>(req.status));
    sqlite3_bind_text(stmt,   26, req.approvedBy.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   27, req.approvedTimestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   28, req.rejectReason.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,   29, req.creatorSignature.c_str(),  -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }
    return true;
}

bool PendingApprovalManager::updateRequestInDB(const PendingBlockRequest& req) {
    if (db_ == nullptr) { lastError_ = "Database not attached"; return false; }

    const char* sql =
        "UPDATE pending_requests SET "
        "  status = ?, approved_by = ?, approved_timestamp = ?, reject_reason = ? "
        "WHERE request_id = ?";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    sqlite3_bind_int(stmt,  1, static_cast<int>(req.status));
    sqlite3_bind_text(stmt, 2, req.approvedBy.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, req.approvedTimestamp.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, req.rejectReason.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt,  5, req.requestId);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }
    return true;
}

bool PendingApprovalManager::loadFromDB() {
    if (db_ == nullptr) { lastError_ = "Database not attached"; return false; }

    requests_.clear();

    const char* sql = "SELECT * FROM pending_requests ORDER BY request_id ASC";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    auto getText = [&](int col) -> std::string {
        const unsigned char* t = sqlite3_column_text(stmt, col);
        return (t != nullptr) ? std::string(reinterpret_cast<const char*>(t))
                              : std::string();
    };

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        PendingBlockRequest req;
        req.requestId          = sqlite3_column_int(stmt, 0);
        req.requestedBy        = getText(1);
        req.requestedByDisplay = getText(2);
        req.requestTimestamp   = getText(3);
        int stageInt           = sqlite3_column_int(stmt, 4);
        req.requestedStage     = static_cast<BlockStage>(stageInt);

        CarRecord& r = req.payload;
        r.vin               = getText(5);
        r.manufacturer      = getText(6);
        r.model             = getText(7);
        r.color             = getText(8);
        r.productionYear    = sqlite3_column_int(stmt, 9);
        r.manufacturerId    = getText(10);
        r.factoryLocation   = getText(11);
        r.warehouseLocation = getText(12);
        r.receivedBy        = getText(13);
        r.supplierId        = getText(14);
        r.inspectorId       = getText(15);
        r.passed            = (sqlite3_column_int(stmt, 16) != 0);
        r.qcNotes           = getText(17);
        r.dealerId          = getText(18);
        r.destination       = getText(19);
        r.transportMode     = getText(20);
        r.buyerId           = getText(21);
        r.retailerId        = getText(22);
        r.salePrice         = sqlite3_column_double(stmt, 23);
        r.warrantyExpiry    = getText(24);
        r.stage             = req.requestedStage;

        req.status             = static_cast<RequestStatus>(sqlite3_column_int(stmt, 25));
        req.approvedBy         = getText(26);
        req.approvedTimestamp   = getText(27);
        req.rejectReason       = getText(28);
        req.creatorSignature   = getText(29);

        // Track the highest ID so locally-assigned IDs never collide.
        if (req.requestId >= nextId_) {
            nextId_ = req.requestId + 1;
        }

        requests_.push_back(std::move(req));
    }

    sqlite3_finalize(stmt);
    return true;
}

}  // namespace cw1
