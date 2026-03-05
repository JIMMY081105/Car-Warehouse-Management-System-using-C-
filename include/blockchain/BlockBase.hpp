#pragma once

// BlockBase.hpp -- Abstract base class for blockchain blocks.
//
// Coursework requirement: demonstrates single inheritance (COMP2034 Lecture 7).
// Block (derived class) inherits from BlockBase (base class).
// BlockBase declares pure virtual functions that Block must override,
// showing polymorphism and the use of virtual/override keywords.

#include <cstddef>
#include <string>

namespace cw1 {

/// Abstract base class for all block types in the blockchain.
/// Cannot be instantiated directly -- must be subclassed.
/// Demonstrates: base class, pure virtual functions, virtual destructor.
class BlockBase {
public:
    /// Virtual destructor -- required for safe polymorphic deletion.
    /// Without this, deleting a derived object through a base pointer
    /// would cause undefined behaviour (Lecture 7 concept).
    virtual ~BlockBase() = default;

    // -- Pure virtual functions (derived class MUST override these) --

    /// Recompute the hash from the block's data.
    virtual std::string computeHash() const = 0;

    /// Human-readable one-line summary.
    virtual std::string toString() const = 0;

    /// Access the block index.
    virtual std::size_t getIndex() const noexcept = 0;

    /// Access the current hash.
    virtual const std::string& getCurrentHash() const noexcept = 0;

    /// Access the previous block's hash.
    virtual const std::string& getPreviousHash() const noexcept = 0;

    /// Access the timestamp.
    virtual const std::string& getTimestamp() const noexcept = 0;
};

} // namespace cw1
