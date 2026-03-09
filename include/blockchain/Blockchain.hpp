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









class Blockchain {
public:
    

    
    void addBlock(const CarRecord& record);

    
    const std::vector<Block>& getChain() const noexcept;

    
    std::size_t totalBlocks() const noexcept;

    

    
    std::vector<const Block*> getCarHistory(const std::string& vin) const;

    
    bool hasVin(const std::string& vin) const;

    
    std::vector<std::string> getAllVins() const;

    
    BlockStage getLatestStage(const std::string& vin) const;

    

    
    std::vector<const Block*> searchByBrand(const std::string& brand) const;

    
    std::vector<const Block*> searchByStage(BlockStage stage) const;

    
    std::vector<const Block*> searchGeneral(const std::string& query) const;

    

    
    ValidationResult verifyIntegrity() const;

    
    
    
    void tamperBlock(std::size_t index);

    
    
    bool tamperBlockHash(std::size_t index,
                         const std::string& forgedHash,
                         std::string& message);

    

    
    
    
    bool softDeleteBlock(std::size_t index, std::string& outMessage);

    
    
    
    bool hardDeleteBlock(std::size_t index, std::string& outMessage);

    
    bool saveBlockchain(const std::string& path) const;

    
    bool loadBlockchain(const std::string& path);

    

    
    
    AuditLog& getAuditLog() const noexcept;

    
    AuditLog& getAuditLog() noexcept;

private:
    
    std::vector<Block> chain_;

    
    std::map<std::string, std::vector<std::size_t>> vinIndex_;

    
    
    
    mutable AuditLog auditLog_;
};

} 
