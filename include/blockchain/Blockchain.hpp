#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"
#include "blockchain/Validation.hpp"
#include "models/CarRecord.hpp"
#include "utils/AuditLog.hpp"
#include "utils/DatabaseManager.hpp"

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

    bool saveBlockchain(const std::string& path) const;

    
    bool loadBlockchain(const std::string& path);

    

    

    const AuditLog& getAuditLog() const noexcept;
    AuditLog& getAuditLog() noexcept;

    // -- SQLite database integration --

    // Open (or create) a SQLite database at dbPath.
    // Returns false if it fails to open.
    bool openDatabase(const std::string& dbPath);

    // Returns true if a database is currently open.
    bool isDatabaseOpen() const noexcept;

    // Save entire current chain to DB (full resync).
    bool saveToDB();

    // Load chain from DB, replacing in-memory chain_ and vinIndex_.
    // Runs verifyIntegrity() after load -- returns false if chain is invalid.
    bool loadFromDB();

    // SQL-powered search -- returns Block pointers for matching blocks.
    // Falls back to searchGeneral() if no DB is open.
    std::vector<const Block*> searchBySQL(const std::string& query) const;

    // Returns a const pointer to the DatabaseManager, or nullptr if not open.
    const DatabaseManager* getDatabase() const noexcept;

    // Returns a mutable pointer to the DatabaseManager, or nullptr if not open.
    DatabaseManager* getDatabase() noexcept;

private:
    
    std::vector<Block> chain_;

    
    std::map<std::string, std::vector<std::size_t>> vinIndex_;




    mutable AuditLog auditLog_;

    // Optional SQLite database for persistence and SQL queries.
    std::unique_ptr<DatabaseManager> db_;
};

} 
