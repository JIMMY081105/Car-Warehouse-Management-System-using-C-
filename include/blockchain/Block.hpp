#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"
#include "blockchain/BlockBase.hpp"

namespace cw1 {

/// A single block in the car warehouse blockchain.
/// Inherits from BlockBase (demonstrates single inheritance, Lecture 7).
/// Overrides all pure virtual methods from the base class.
class Block : public BlockBase {
public:
    Block(std::size_t index, std::string previousHash, CarRecord record);
    Block(std::size_t index,
          std::string previousHash,
          std::string currentHash,
          std::string timestamp,
          std::uint64_t nonce,
          CarRecord record);

    std::size_t         getIndex()        const noexcept override;
    const std::string&  getCurrentHash()  const noexcept override;
    const std::string&  getPreviousHash() const noexcept override;
    const std::string&  getTimestamp()    const noexcept override;
    std::uint64_t       getNonce()        const noexcept;
    const CarRecord&    getRecord()       const noexcept;

    /// Recompute the hash from the block's data (used for integrity checks).
    std::string computeHash() const override;

    /// Human-readable one-line summary.
    std::string toString() const override;

    /// Debug/admin helper for tamper simulation coursework demo.
    /// Intentionally overrides the stored hash to break integrity checks.
    void debugOverrideCurrentHash(std::string forgedHash);

    /// Debug / Simulation Feature:
    /// intentionally mutates payload data without updating hash.
    void debugTamperPayloadForSimulation(const std::string& marker);

    /// Admin/chain-repair mutators for hard-delete re-indexing.
    void setIndex(std::size_t newIndex);
    void setPreviousHash(std::string newPrev);

    /// Replace the stored record (used by soft-delete tombstone).
    void setRecord(CarRecord newRecord);

private:
    static std::uint64_t generateNonce();

    std::size_t    index_;
    std::string    currentHash_;
    std::string    previousHash_;
    std::string    timestamp_;
    std::uint64_t  nonce_;
    CarRecord      record_;
};

} // namespace cw1
