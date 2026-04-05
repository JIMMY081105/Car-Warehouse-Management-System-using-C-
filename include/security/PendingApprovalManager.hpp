// Pending request model and approval workflow manager.

#pragma once

#include <string>
#include <vector>

#include "blockchain/BlockStage.hpp"
#include "models/CarRecord.hpp"

struct sqlite3;

namespace cw1 {

// Tracks whether a pending request is waiting for review, has been approved
// and committed to the chain, or has been rejected with a reason.
enum class RequestStatus {
    PENDING  = 0,
    APPROVED = 1,
    REJECTED = 2
};

// Returns a GUI-friendly label for the request status.
std::string requestStatusToString(RequestStatus status);

// One pending block request submitted by a warehouse user. The payload is a
// complete CarRecord snapshot that will become the block data if the request
// is approved by an Admin.
struct PendingBlockRequest {
    int requestId = -1;                 // Auto-assigned unique identifier
    std::string requestedBy;            // Username of the submitting user
    std::string requestedByDisplay;     // Display name of the submitting user
    std::string requestTimestamp;       // ISO-8601 timestamp of submission
    BlockStage requestedStage = BlockStage::PRODUCTION;
    CarRecord payload;                  // Full vehicle record for the proposed block

    RequestStatus status = RequestStatus::PENDING;
    std::string approvedBy;             // Username of the approving/rejecting Admin
    std::string approvedTimestamp;      // ISO-8601 timestamp of the decision
    std::string rejectReason;           // Reason text when status is REJECTED
    std::string creatorSignature;       // Lightweight SHA-256 signature for authenticity
};

// Coordinates the pending approval lifecycle: submission, review, approval,
// rejection, and persistence to SQLite. The in-memory vector mirrors the
// database table so the GUI can render the approval queue without repeated
// SQL queries.
class PendingApprovalManager {
public:
    PendingApprovalManager();

    // Connects to the SQLite database used for persistent request storage.
    void attach(sqlite3* db);

    // Returns true when the manager has an active database connection.
    bool isAttached() const noexcept;

    // Submits a new pending request and persists it. Returns the assigned ID.
    int submitRequest(const PendingBlockRequest& request);

    // Marks a pending request as approved and records the approver identity.
    bool approveRequest(int requestId, const std::string& approverUsername);

    // Marks a pending request as rejected with the given reason text.
    bool rejectRequest(int requestId, const std::string& approverUsername,
                       const std::string& reason);

    // Returns all requests regardless of status for the approval history view.
    const std::vector<PendingBlockRequest>& getAllRequests() const noexcept;

    // Returns only requests that are still waiting for a decision.
    std::vector<PendingBlockRequest> getPendingRequests() const;

    // Finds a specific request by ID, or returns nullptr if not found.
    const PendingBlockRequest* getRequest(int requestId) const;

    // Returns the number of requests currently in PENDING status.
    int pendingCount() const;

    // Reloads all requests from SQLite into the in-memory vector.
    bool loadFromDB();

    // Returns the last error message from a failed database operation.
    std::string lastError() const;

private:
    // Creates the pending_requests table if it does not already exist.
    void createTables();

    // Persists a single request row to SQLite using INSERT OR REPLACE.
    bool saveRequestToDB(const PendingBlockRequest& request);

    // Updates an existing request row in SQLite after approval or rejection.
    bool updateRequestInDB(const PendingBlockRequest& request);

    sqlite3* db_ = nullptr;
    std::vector<PendingBlockRequest> requests_;
    int nextId_ = 1;
    std::string lastError_;
};

}  // namespace cw1
