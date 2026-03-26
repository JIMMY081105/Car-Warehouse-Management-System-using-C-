#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"

namespace cw1 {

class Block {
public:
    Block(std::size_t index, std::string previousHash, CarRecord record);
    Block(std::size_t index,
          std::string previousHash,
          std::string currentHash,
          std::string timestamp,
          std::uint64_t nonce,
          CarRecord record,
          std::string sha3Hash = "");

    std::size_t         getIndex()        const noexcept;
    const std::string&  getCurrentHash()  const noexcept;
    const std::string&  getPreviousHash() const noexcept;
    const std::string&  getTimestamp()     const noexcept;
    std::uint64_t       getNonce()        const noexcept;
    const CarRecord&    getRecord()       const noexcept;
    const std::string&  getSha3Hash()     const noexcept;

    std::string computeHash() const;

    std::string computeSha3Hash() const;

    std::string toString() const;

    
    
    void debugOverrideCurrentHash(std::string forgedHash);

    
    
    void debugTamperPayloadForSimulation(const std::string& marker);

    
    void setIndex(std::size_t newIndex);
    void setPreviousHash(std::string newPrev);

    
    void setRecord(CarRecord newRecord);

    // Recompute currentHash_ and sha3Hash_ from current block data.
    void rehash();

private:
    static std::uint64_t generateNonce();

    std::size_t    index_;
    std::string    currentHash_;
    std::string    previousHash_;
    std::string    sha3Hash_;
    std::string    timestamp_;
    std::uint64_t  nonce_;
    CarRecord      record_;
};

} 
