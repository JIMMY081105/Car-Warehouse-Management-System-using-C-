#pragma once

#include <map>
#include <string>
#include <vector>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"
#include "blockchain/Validation.hpp"
#include "models/CarRecord.hpp"
#include "utils/AuditLog.hpp"

namespace cw1 {

/// Hybrid blockchain: single global chain + VIN index for O(1) lookups.
///
/// Architecture:
///   chain_    -- vector<Block>  (immutable, chronological, source of truth)
///   vinIndex_ -- map<string, vector<size_t>>  (VIN -> block indices)
///
/// All blocks from all cars are stored in one chain for global integrity.
/// The VIN index provides fast per-car history lookups without scanning.
class Blockchain {
public:
    // -- Core operations -----------------------------------------------

    /// Append a new block to the global chain and update the VIN index.
    void addBlock(const CarRecord& record);

    /// Return the entire global chain (read-only).
    const std::vector<Block>& getChain() const noexcept;

    /// Total number of blocks across all cars.
    std::size_t totalBlocks() const noexcept;

    // -- Per-car lookups (powered by VIN index) ------------------------

    /// Get pointers to every block belonging to a given VIN (in order).
    std::vector<const Block*> getCarHistory(const std::string& vin) const;

    /// Check whether a VIN exists in the index.
    bool hasVin(const std::string& vin) const;

    /// Return a sorted list of all VINs currently tracked.
    std::vector<std::string> getAllVins() const;

    /// Get the latest stage a car has reached.
    BlockStage getLatestStage(const std::string& vin) const;

    // -- Search / filter -----------------------------------------------

    /// Find all blocks whose manufacturer matches (case-insensitive substring).
    std::vector<const Block*> searchByBrand(const std::string& brand) const;

    /// Find all blocks at a specific lifecycle stage.
    std::vector<const Block*> searchByStage(BlockStage stage) const;

    /// Find all blocks whose VIN, manufacturer, or model contain the query.
    std::vector<const Block*> searchGeneral(const std::string& query) const;

    // -- Integrity -----------------------------------------------------

    /// Verify the entire global chain (hashes + linkage).
    ValidationResult verifyIntegrity() const;

    /// Debug / Simulation Feature:
    /// intentionally modifies only block payload data and does not re-hash.
    /// This is used to demonstrate immutability and integrity failure.
    void tamperBlock(std::size_t index);

    /// Admin/debug coursework feature: intentionally tamper a block hash.
    /// Returns false with message if index is invalid.
    bool tamperBlockHash(std::size_t index,
                         const std::string& forgedHash,
                         std::string& message);

    // -- Block deletion --------------------------------------------------

    /// Soft delete: replace block payload with a DELETED tombstone record.
    /// The block stays in the chain. Hash chain is NOT broken.
    /// Returns false if index is out of range.
    bool softDeleteBlock(std::size_t index, std::string& outMessage);

    /// Hard delete: physically erase the block from chain_ at the given index.
    /// Re-indexes and re-hashes all subsequent blocks. Rebuilds vinIndex_.
    /// Returns false if index is out of range or chain would become empty.
    bool hardDeleteBlock(std::size_t index, std::string& outMessage);

    /// Save blockchain to disk. Returns true on success.
    bool saveBlockchain(const std::string& path) const;

    /// Load blockchain from disk. Returns true on success.
    bool loadBlockchain(const std::string& path);

    // -- Audit log -----------------------------------------------------

    /// Access the audit log (const version -- works even on const Blockchain
    /// because auditLog_ is mutable; logging is a non-logical side effect).
    AuditLog& getAuditLog() const noexcept;

    /// Access the audit log (non-const version for direct mutation).
    AuditLog& getAuditLog() noexcept;

private:
    /// The single global chain -- source of truth.
    std::vector<Block> chain_;

    /// VIN index -- maps each VIN to the indices of its blocks in chain_.
    std::map<std::string, std::vector<std::size_t>> vinIndex_;

    /// Audit log -- mutable so it can be updated from const methods.
    /// Logging is a side effect that does not change the blockchain's
    /// logical state, so mutable is semantically correct here.
    mutable AuditLog auditLog_;
};

} // namespace cw1
