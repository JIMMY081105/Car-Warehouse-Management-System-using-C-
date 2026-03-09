#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "models/CarRecord.hpp"
#include "blockchain/BlockBase.hpp"

namespace cw1 {




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

    
    std::string computeHash() const override;

    
    std::string toString() const override;

    
    
    void debugOverrideCurrentHash(std::string forgedHash);

    
    
    void debugTamperPayloadForSimulation(const std::string& marker);

    
    void setIndex(std::size_t newIndex);
    void setPreviousHash(std::string newPrev);

    
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

} 
