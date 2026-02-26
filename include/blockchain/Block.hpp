#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"

namespace cw1 {

class Block {
public:
    Block(std::size_t index, std::string previousHash, CarRecord record);

    std::size_t getIndex() const noexcept;
    const std::string& getCurrentHash() const noexcept;
    const std::string& getPreviousHash() const noexcept;
    const std::string& getTimestamp() const noexcept;
    std::uint64_t getNonce() const noexcept;
    const CarRecord& getRecord() const noexcept;

    std::string computeHash() const;
    std::string toString() const;

private:
    static std::uint64_t generateNonce();

    std::size_t index_;
    std::string currentHash_;
    std::string previousHash_;
    std::string timestamp_;
    std::uint64_t nonce_;
    CarRecord record_;
};

} // namespace cw1
