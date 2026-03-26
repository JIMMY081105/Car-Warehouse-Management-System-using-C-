// Defines one blockchain block in the car warehouse lifecycle. Each block stores the current payload snapshot, cryptographic hashes, timestamp, nonce, and the previous-hash reference that links one block to the next.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"

namespace cw1 {

// Represents one immutable-looking blockchain record. The payload is a CarRecord snapshot for a single lifecycle stage of one vehicle.
class Block {
public:
    // Constructs a new live block. Hashes and timestamp are generated here so the block can be appended directly to the authoritative chain.
    Block(std::size_t index, std::string previousHash, CarRecord record);

    // Reconstructs a block that was already persisted to disk or SQLite.
    Block(std::size_t index,
          std::string previousHash,
          std::string currentHash,
          std::string timestamp,
          std::uint64_t nonce,
          CarRecord record,
          std::string sha3Hash = "");

    std::size_t getIndex() const noexcept;
    const std::string& getCurrentHash() const noexcept;
    const std::string& getPreviousHash() const noexcept;
    const std::string& getTimestamp() const noexcept;
    std::uint64_t getNonce() const noexcept;
    const CarRecord& getRecord() const noexcept;
    const std::string& getSha3Hash() const noexcept;

    // Recomputes the primary SHA-256 digest used for chain validation.
    std::string computeHash() const;

    // Secondary hash shown in the UI and checked during validation.
    std::string computeSha3Hash() const;

    // Returns a compact lecturer-friendly summary of the block.
    std::string toString() const;

    // Debug helpers are intentionally explicit because the integrity screen uses them to demonstrate how tampering is detected later.
    void debugOverrideCurrentHash(std::string forgedHash);
    void debugTamperPayloadForSimulation(const std::string& marker);

    void setIndex(std::size_t newIndex);
    void setPreviousHash(std::string newPrev);

    // Used by tombstone delete/restore flows before downstream rehashing.
    void setRecord(CarRecord newRecord);

    // Recomputes stored hashes after payload or previous-hash changes.
    void rehash();

private:
    static std::uint64_t generateNonce();

    std::size_t index_;
    std::string currentHash_;
    std::string previousHash_;
    std::string sha3Hash_;
    std::string timestamp_;
    std::uint64_t nonce_;
    CarRecord record_;
};

}  // namespace cw1
