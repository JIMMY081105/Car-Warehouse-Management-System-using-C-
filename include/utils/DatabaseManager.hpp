#pragma once

#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#include "blockchain/Block.hpp"
#include "utils/AuditLog.hpp"

struct sqlite3;

namespace cw1 {

// Column indices for the blocks table (mirrors CREATE TABLE order).
enum class BlockCol : int {
    Index = 0,
    CurrentHash,        // 1
    PreviousHash,       // 2
    Timestamp,          // 3
    Nonce,              // 4
    Vin,                // 5
    Manufacturer,       // 6
    Model,              // 7
    Color,              // 8
    ProductionYear,     // 9
    Stage,              // 10
    FactoryLocation,    // 11
    WarehouseLocation,  // 12
    ReceivedBy,         // 13
    SupplierId,         // 14
    InspectorId,        // 15
    Passed,             // 16
    QcNotes,            // 17
    DealerId,           // 18
    Destination,        // 19
    TransportMode,      // 20
    BuyerId,            // 21
    RetailerId,         // 22
    SalePrice,          // 23
    WarrantyExpiry,     // 24
    ManufacturerId,     // 25
    IsTombstone,        // 26
    Sha3Hash,           // 27
    COUNT               // 28 — total number of columns
};

// Helper to convert BlockCol to int for SQLite binding (1-based) and reading (0-based).
inline int col(BlockCol c) { return static_cast<int>(c); }
inline int bind(BlockCol c) { return static_cast<int>(c) + 1; }

// Manages SQLite persistence for the blockchain.
// Uses RAII -- the constructor opens (or creates) the database and ensures
// tables exist; the destructor closes the connection.  Non-copyable, movable.
class DatabaseManager {
public:
    // Open (or create) a SQLite database at dbPath.
    explicit DatabaseManager(const std::string& dbPath);

    // Close the database connection.
    ~DatabaseManager();

    // Non-copyable.
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Movable.
    DatabaseManager(DatabaseManager&&) noexcept;
    DatabaseManager& operator=(DatabaseManager&&) noexcept;

    // Returns true if the database connection is open and usable.
    bool isOpen() const noexcept;

    // Returns the last error message from SQLite, or an empty string.
    std::string lastError() const;

    // -- Block persistence --

    // Save the entire chain vector to the blocks table (upsert by block_index).
    // Uses a transaction for performance.
    bool saveAllBlocks(const std::vector<Block>& chain);

    // Load all blocks from DB ordered by block_index.
    // Returns an empty vector on failure.
    std::vector<Block> loadAllBlocks();

    // Insert or replace a single block row (called after addBlock).
    bool upsertBlock(const Block& block);

    // Delete a block row by block_index, then renumber all rows whose
    // block_index is greater than the deleted index.
    bool deleteBlockRow(std::size_t index);

    // -- Audit log persistence --

    // Append a single audit entry to the audit_log table.
    bool insertAuditEntry(AuditAction action, const std::string& details,
                          const std::string& timestamp);

    // Load all audit entries ordered by rowid ascending.
    // Returns vector of tuples: {action string, details, timestamp}.
    std::vector<std::tuple<std::string, std::string, std::string>> loadAuditLog();

    // -- SQL search (returns matching block indices) --

    // Search blocks table WHERE vin / manufacturer / model / destination /
    // qc_notes LIKE %query% (case-insensitive).
    std::vector<std::size_t> searchBlocksSQL(const std::string& query);

    // Get all block indices for a specific VIN ordered by block_index.
    std::vector<std::size_t> getBlockIndicesByVin(const std::string& vin);

    // Get all distinct VINs stored in the database.
    std::vector<std::string> getAllVinsSQL();

    // -- Raw handle (for sub-managers that share this connection) --

    // Returns the underlying sqlite3 handle, or nullptr if not open.
    // Ownership remains with DatabaseManager.
    sqlite3* rawHandle() noexcept;

    // -- Sync --

    // Drop and recreate all tables, then repopulate from the given chain and
    // audit log.
    bool fullResync(const std::vector<Block>& chain, const AuditLog& auditLog);

private:
    // Create the blocks and audit_log tables if they don't already exist.
    void createTables();

    // Execute a simple SQL statement that takes no parameters.
    bool execSQL(const std::string& sql);

    sqlite3* db_;
    std::string lastError_;
    std::string dbPath_;
};

} // namespace cw1
