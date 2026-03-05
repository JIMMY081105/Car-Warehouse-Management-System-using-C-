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
