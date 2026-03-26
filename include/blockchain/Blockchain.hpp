// Declares the main blockchain service used by the coursework system. The class keeps the authoritative chain in a std::vector for efficient indexed access, search, validation, and SQLite persistence, and also maintains a companion linked-list mirror so pointer-based traversal is explicit for lecture requirements.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"
#include "blockchain/LinkedBlockchainView.hpp"
#include "blockchain/Validation.hpp"
#include "models/CarRecord.hpp"
#include "utils/AuditLog.hpp"
#include "utils/DatabaseManager.hpp"

namespace cw1 {

// Coordinates the blockchain, audit log, and persistence layer for the car warehouse workflow. Public methods are used directly by the GUI.
class Blockchain {
public:
    // Appends a new block to the end of the chain. The new block records the previous block's hash so tampering can be detected during verification.
    void addBlock(const CarRecord& record);

    // Returns the authoritative vector-backed blockchain storage. This remains the primary representation because indexed access is needed for the GUI, VIN lookup, validation, and database synchronisation.
    const std::vector<Block>& getChain() const noexcept;

    // Returns the number of blocks currently in the authoritative chain.
    std::size_t totalBlocks() const noexcept;

    // Returns all blocks recorded for one VIN in chain order.
    std::vector<const Block*> getCarHistory(const std::string& vin) const;

    // Fast lookup used by the GUI before opening a vehicle detail view.
    bool hasVin(const std::string& vin) const;

    // Returns all VIN keys currently indexed in memory.
    std::vector<std::string> getAllVins() const;

    // Reads the latest lifecycle stage for a VIN from the final indexed block. Throws std::out_of_range if the VIN has no indexed history.
    BlockStage getLatestStage(const std::string& vin) const;

    // Search helpers used by the dashboard and global chain pages.
    std::vector<const Block*> searchByBrand(const std::string& brand) const;
    std::vector<const Block*> searchByStage(BlockStage stage) const;
    std::vector<const Block*> searchGeneral(const std::string& query) const;

    // Recomputes hashes and previous-hash links to detect payload tampering, hash forgery, and broken genesis/chain-link rules.
    ValidationResult verifyIntegrity() const;

    // Debug/demo helpers used by the immutability screen. They deliberately damage stored blockchain data so validation failures can be demonstrated.
    void tamperBlock(std::size_t index);
    bool tamperBlockHash(std::size_t index,
                         const std::string& forgedHash,
                         std::string& message);

    // Soft delete keeps the block index and chain history intact by replacing the payload with a tombstone record, then rehashing later blocks.
    bool softDeleteBlock(std::size_t index, std::string& outMessage);

    // Restores the original payload for a previously soft-deleted block.
    bool restoreBlock(std::size_t index, std::string& outMessage);

    // Reports whether a block currently holds a tombstone placeholder.
    bool isDeleted(std::size_t index) const;

    // Returns the preserved original payload for lecturer-visible restore logic.
    const CarRecord* getDeletedOriginal(std::size_t index) const;

    // Lists every block index currently stored in the deleted-record cache.
    std::vector<std::size_t> getDeletedIndices() const;

    // Rebuilds hashes from one position onward after a tombstone or restoration changes the payload or previous-hash linkage.
    void rehashFrom(std::size_t index);

    // File serialisation used as an additional persistence route besides SQLite. Returns false and writes an audit entry if stream or formatting failures occur.
    bool saveBlockchain(const std::string& path) const;

    // Replaces the in-memory chain from a saved text export after validating it. Returns false if file access, parsing, conversion, or integrity checks fail.
    bool loadBlockchain(const std::string& path);

    // Audit log is intentionally exposed so the GUI can display lecturer-facing evidence of searches, validation runs, persistence, and tamper simulation.
    const AuditLog& getAuditLog() const noexcept;
    AuditLog& getAuditLog() noexcept;

    // SQLite integration keeps the richer GUI workflow persistent across runs. These methods catch backend exceptions, report them via the audit log, and return false instead of allowing GUI-triggered failures to escape.
    bool openDatabase(const std::string& dbPath);
    bool isDatabaseOpen() const noexcept;
    bool saveToDB();
    bool loadFromDB();
    std::vector<const Block*> searchBySQL(const std::string& query) const;
    const DatabaseManager* getDatabase() const noexcept;
    DatabaseManager* getDatabase() noexcept;

    // Returns the head of the companion linked-list view. The nodes form an explicit head -> next -> next chain for pointer-based traversal.
    const LinkedBlockNode* getLinkedHead() const noexcept;

    // Returns the linked-list mirror object so the GUI can report node counts without exposing ownership of the raw nodes.
    const LinkedBlockchainView& getLinkedView() const noexcept;

private:
    // Authoritative blockchain storage. Vector access is kept because many features depend on random access and stable indexing by block number.
    std::vector<Block> chain_;

    // Secondary index from VIN to all related block positions.
    std::map<std::string, std::vector<std::size_t>> vinIndex_;

    // Linked-list mirror used for coursework visibility of Node* traversal.
    LinkedBlockchainView linkedView_;

    // Linked audit trail of user/system actions.
    mutable AuditLog auditLog_;

    // Stores original records for tombstoned blocks so restore is possible.
    std::map<std::size_t, CarRecord> deletedRecords_;

    // Optional persistence backend used by the GUI and SQL search features.
    std::unique_ptr<DatabaseManager> db_;

    // Rebuilds the linked-list mirror after any structural change that may invalidate pointers into the vector-backed chain.
    void rebuildLinkedView();
};

}  // namespace cw1
