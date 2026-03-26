// Implements the main blockchain workflow for the car warehouse system. The vector-backed chain remains authoritative for indexed access and persistence, while a secondary linked-list mirror is rebuilt after structural edits so the coursework can demonstrate explicit pointer chaining safely.

#include "blockchain/Blockchain.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "utils/StringUtil.hpp"
#include "utils/TimeUtil.hpp"

namespace {

std::string BuildCaughtExceptionMessage(const std::string& context, const std::exception& ex) {
    return context + ": " + ex.what();
}

std::string BuildUnknownExceptionMessage(const std::string& context) {
    return context + ": unknown exception";
}

}  // namespace

namespace cw1 {

void Blockchain::rebuildLinkedView() {
    // The linked-list mirror stores Block addresses. Rebuild it after any structural vector change so raw node pointers never outlive valid blocks.
    linkedView_.rebuild(chain_);
}

// Adding a block updates the vector chain, the VIN index, the linked-list mirror, and the audit/database records in one place.
void Blockchain::addBlock(const CarRecord& record) {
    // The genesis block uses "0" as the seed input; later blocks point at the previous block's stored hash so any tampering breaks validation.
    const std::string prevHash =
        chain_.empty() ? "0" : chain_.back().getCurrentHash();

    chain_.emplace_back(chain_.size(), prevHash, record);
    vinIndex_[record.vin].push_back(chain_.size() - 1);
    rebuildLinkedView();

    const Block& added = chain_.back();
    std::ostringstream detail;
    detail << "Added block #" << added.getIndex()
           << " for " << record.vin
           << " (" << stageToString(record.stage) << ")";
    auditLog_.log(AuditAction::BLOCK_ADDED, detail.str());

    // SQLite is optional. When it is open we mirror the same logical event so the GUI, restarts, and SQL queries see the same blockchain state.
    if (db_ && db_->isOpen()) {
        db_->upsertBlock(chain_.back());
        db_->insertAuditEntry(AuditAction::BLOCK_ADDED, detail.str(),
                              TimeUtil::nowIso8601());
    }
}

const std::vector<Block>& Blockchain::getChain() const noexcept {
    return chain_;
}

std::size_t Blockchain::totalBlocks() const noexcept {
    return chain_.size();
}



std::vector<const Block*> Blockchain::getCarHistory(const std::string& vin) const {
    std::vector<const Block*> history;
    auto it = vinIndex_.find(vin);
    if (it != vinIndex_.end()) {
        history.reserve(it->second.size());
        for (std::size_t idx : it->second) {
            history.push_back(&chain_[idx]);
        }
    }
    return history;
}

bool Blockchain::hasVin(const std::string& vin) const {
    return vinIndex_.count(vin) > 0;
}

std::vector<std::string> Blockchain::getAllVins() const {
    std::vector<std::string> vins;
    vins.reserve(vinIndex_.size());
    for (const auto& [vin, indices] : vinIndex_) {
        vins.push_back(vin);
    }
    return vins;
}

BlockStage Blockchain::getLatestStage(const std::string& vin) const {
    auto it = vinIndex_.find(vin);
    if (it == vinIndex_.end() || it->second.empty()) {
        throw std::out_of_range("VIN not found: " + vin);
    }

    return chain_[it->second.back()].getRecord().stage;
}



std::vector<const Block*> Blockchain::searchByBrand(const std::string& brand) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        if (containsIgnoreCase(block.getRecord().manufacturer, brand)) {
            results.push_back(&block);
        }
    }

    std::ostringstream detail;
    detail << "searchByBrand(\"" << brand << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

std::vector<const Block*> Blockchain::searchByStage(BlockStage stage) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        if (block.getRecord().stage == stage) {
            results.push_back(&block);
        }
    }

    std::ostringstream detail;
    detail << "searchByStage(" << stageToString(stage) << ") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

std::vector<const Block*> Blockchain::searchGeneral(const std::string& query) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        const CarRecord& rec = block.getRecord();
        if (containsIgnoreCase(rec.vin, query) ||
            containsIgnoreCase(rec.manufacturer, query) ||
            containsIgnoreCase(rec.model, query) ||
            containsIgnoreCase(rec.color, query) ||
            containsIgnoreCase(stageToString(rec.stage), query) ||
            containsIgnoreCase(rec.destination, query) ||
            containsIgnoreCase(rec.qcNotes, query)) {
            results.push_back(&block);
        }
    }

    std::ostringstream detail;
    detail << "searchGeneral(\"" << query << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}



ValidationResult Blockchain::verifyIntegrity() const {
    // Validation checks the recalculated payload hash, the stored SHA3-128 digest, the genesis rule, and previous-hash linkage for each block.
    ValidationResult result = Validation::verifyChain(chain_);

    std::ostringstream detail;
    detail << "verifyIntegrity() -> " << (result.ok ? "PASS" : "FAIL")
           << ": " << result.message;
    auditLog_.log(AuditAction::INTEGRITY_CHECK, detail.str());

    if (db_ && db_->isOpen()) {
        db_->insertAuditEntry(AuditAction::INTEGRITY_CHECK, detail.str(),
                              TimeUtil::nowIso8601());
    }
    return result;
}

void Blockchain::tamperBlock(std::size_t index) {
    if (index >= chain_.size()) {
        std::ostringstream detail;
        detail << "Debug / Simulation Feature: tamperBlock(" << index
               << ") failed (index out of range).";
        auditLog_.log(AuditAction::TAMPER_SIMULATED, detail.str());
        if (db_ && db_->isOpen()) {
            db_->insertAuditEntry(AuditAction::TAMPER_SIMULATED, detail.str(),
                                  TimeUtil::nowIso8601());
        }
        return;
    }

    chain_[index].debugTamperPayloadForSimulation("Tampered Location");

    std::ostringstream detail;
    detail << "Debug / Simulation Feature: payload tampered for block #"
           << index << " (destination set to \"Tampered Location\").";
    auditLog_.log(AuditAction::TAMPER_SIMULATED, detail.str());


    if (db_ && db_->isOpen()) {
        db_->insertAuditEntry(AuditAction::TAMPER_SIMULATED, detail.str(),
                              TimeUtil::nowIso8601());
    }
}

bool Blockchain::tamperBlockHash(std::size_t index,
                                 const std::string& forgedHash,
                                 std::string& message) {
    if (index >= chain_.size()) {
        std::ostringstream err;
        err << "Tamper failed: block index " << index
            << " out of range (0.." << (chain_.empty() ? 0 : chain_.size() - 1) << ").";
        message = err.str();
        return false;
    }

    std::string finalHash = forgedHash;
    if (finalHash.empty()) {
        finalHash = chain_[index].getCurrentHash();
        if (finalHash.empty()) {
            finalHash = "tampered_hash";
        } else {
            finalHash[0] = (finalHash[0] == '0') ? '1' : '0';
        }
    }

    if (finalHash == chain_[index].getCurrentHash()) {
        finalHash.push_back('x');
    }

    chain_[index].debugOverrideCurrentHash(finalHash);

    std::ostringstream ok;
    ok << "Tampered block #" << index
       << ". Stored currentHash overridden for integrity-failure demo.";
    message = ok.str();
    auditLog_.log(AuditAction::TAMPER_SIMULATED, message);
    return true;
}



bool Blockchain::softDeleteBlock(std::size_t index, std::string& outMessage) {
    if (index >= chain_.size()) {
        std::ostringstream err;
        err << "Soft delete failed: block index " << index
            << " out of range (0.." << (chain_.empty() ? 0 : chain_.size() - 1) << ").";
        outMessage = err.str();
        return false;
    }

    if (isDeleted(index)) {
        outMessage = "Block #" + std::to_string(index) + " is already deleted.";
        return false;
    }

    // Store the original payload so the delete remains reversible for the coursework restore flow. The chain keeps its position and linkage.
    deletedRecords_[index] = chain_[index].getRecord();

    const std::string originalVin = chain_[index].getRecord().vin;
    chain_[index].setRecord(CarRecord::tombstone(originalVin));

    // Rehash from the edited block onwards because every later block depends on the previous hash value in a blockchain.
    rehashFrom(index);

    std::ostringstream detail;
    detail << "Soft delete block #" << index
           << " for VIN " << originalVin
           << " (tombstone applied, chain rehashed)";
    outMessage = detail.str();
    auditLog_.log(AuditAction::BLOCK_DELETED, outMessage);

    if (db_ && db_->isOpen()) {
        // Every later block was rehashed, so the persisted rows must also be updated from the edit point onward.
        for (std::size_t i = index; i < chain_.size(); ++i) {
            db_->upsertBlock(chain_[i]);
        }
        db_->insertAuditEntry(AuditAction::BLOCK_DELETED, outMessage,
                              TimeUtil::nowIso8601());
    }
    return true;
}

bool Blockchain::restoreBlock(std::size_t index, std::string& outMessage) {
    auto it = deletedRecords_.find(index);
    if (it == deletedRecords_.end()) {
        outMessage = "Block #" + std::to_string(index) + " has no stored original data to restore.";
        return false;
    }
    chain_[index].setRecord(it->second);

    // Restoring the original payload also changes the block hash and therefore every downstream previous-hash link.
    rehashFrom(index);

    deletedRecords_.erase(it);

    std::ostringstream detail;
    detail << "Restored block #" << index
           << " for VIN " << chain_[index].getRecord().vin
           << " (original data recovered, chain rehashed)";
    outMessage = detail.str();
    auditLog_.log(AuditAction::BLOCK_DELETED, outMessage);

    if (db_ && db_->isOpen()) {
        for (std::size_t i = index; i < chain_.size(); ++i) {
            db_->upsertBlock(chain_[i]);
        }
        db_->insertAuditEntry(AuditAction::BLOCK_DELETED, outMessage,
                              TimeUtil::nowIso8601());
    }
    return true;
}

bool Blockchain::isDeleted(std::size_t index) const {
    return deletedRecords_.find(index) != deletedRecords_.end();
}

const CarRecord* Blockchain::getDeletedOriginal(std::size_t index) const {
    auto it = deletedRecords_.find(index);
    return (it != deletedRecords_.end()) ? &it->second : nullptr;
}

std::vector<std::size_t> Blockchain::getDeletedIndices() const {
    std::vector<std::size_t> indices;
    for (const auto& pair : deletedRecords_) {
        indices.push_back(pair.first);
    }
    return indices;
}

void Blockchain::rehashFrom(std::size_t index) {
    // Rehash walks forward because each block depends on the hash stored by the block immediately before it.
    for (std::size_t i = index; i < chain_.size(); ++i) {
        if (i > 0) {
            chain_[i].setPreviousHash(chain_[i - 1].getCurrentHash());
        }
        chain_[i].rehash();
    }
}



bool Blockchain::saveBlockchain(const std::string& path) const {
    try {
        std::ofstream out;
        out.exceptions(std::ios::failbit | std::ios::badbit);
        out.open(path);

        // Stream exceptions are enabled so file access failures are handled in one catch block instead of being silently ignored.
        out << "CW1_BLOCKCHAIN_V1\n";
        out << chain_.size() << '\n';

        for (const auto& block : chain_) {
            const CarRecord& r = block.getRecord();
            out << block.getIndex() << '\t'
                << std::quoted(block.getPreviousHash()) << '\t'
                << std::quoted(block.getCurrentHash()) << '\t'
                << std::quoted(block.getTimestamp()) << '\t'
                << block.getNonce() << '\t'
                << std::quoted(r.vin) << '\t'
                << std::quoted(r.manufacturer) << '\t'
                << std::quoted(r.model) << '\t'
                << std::quoted(r.color) << '\t'
                << r.productionYear << '\t'
                << static_cast<int>(r.stage) << '\t'
                << std::quoted(r.factoryLocation) << '\t'
                << std::quoted(r.warehouseLocation) << '\t'
                << std::quoted(r.receivedBy) << '\t'
                << std::quoted(r.inspectorId) << '\t'
                << (r.passed ? 1 : 0) << '\t'
                << std::quoted(r.qcNotes) << '\t'
                << std::quoted(r.dealerId) << '\t'
                << std::quoted(r.destination) << '\t'
                << std::quoted(r.transportMode) << '\t'
                << std::quoted(r.buyerId) << '\t'
                << r.salePrice << '\t'
                << std::quoted(r.warrantyExpiry)
                << '\t' << std::quoted(r.manufacturerId)
                << '\t' << std::quoted(r.supplierId)
                << '\t' << std::quoted(r.retailerId)
                << '\t' << std::quoted(block.getSha3Hash()) << '\n';
        }

        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "saveBlockchain succeeded: " + path);
        return true;
    } catch (const std::ios_base::failure& ex) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildCaughtExceptionMessage("saveBlockchain failed", ex));
    } catch (const std::exception& ex) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildCaughtExceptionMessage("saveBlockchain failed", ex));
    } catch (...) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildUnknownExceptionMessage("saveBlockchain failed"));
    }
    return false;
}

bool Blockchain::loadBlockchain(const std::string& path) {
    try {
        std::ifstream in(path);
        in.exceptions(std::ios::badbit);
        if (!in) {
            throw std::runtime_error("cannot open file " + path);
        }

        std::string header;
        if (!std::getline(in, header) || header != "CW1_BLOCKCHAIN_V1") {
            throw std::runtime_error("invalid file header in " + path);
        }

        std::string countLine;
        if (!std::getline(in, countLine)) {
            throw std::runtime_error("missing block count in " + path);
        }

        std::size_t expectedCount = 0;
        try {
            expectedCount = static_cast<std::size_t>(std::stoull(countLine));
        } catch (const std::invalid_argument&) {
            throw std::invalid_argument("invalid block count in " + path);
        } catch (const std::out_of_range&) {
            throw std::out_of_range("block count out of range in " + path);
        }

        std::vector<Block> loadedChain;
        loadedChain.reserve(expectedCount);
        std::map<std::string, std::vector<std::size_t>> loadedIndex;

        std::string line;
        std::size_t lineNo = 2;
        while (std::getline(in, line)) {
            ++lineNo;
            if (line.empty()) {
                continue;
            }

            std::istringstream row(line);
            std::size_t index = 0;
            std::string previousHash;
            std::string currentHash;
            std::string timestamp;
            std::uint64_t nonce = 0;
            CarRecord record;
            int stageInt = 0;
            int passedInt = 0;

            if (!(row >> index
                      >> std::quoted(previousHash)
                      >> std::quoted(currentHash)
                      >> std::quoted(timestamp)
                      >> nonce
                      >> std::quoted(record.vin)
                      >> std::quoted(record.manufacturer)
                      >> std::quoted(record.model)
                      >> std::quoted(record.color)
                      >> record.productionYear
                      >> stageInt
                      >> std::quoted(record.factoryLocation)
                      >> std::quoted(record.warehouseLocation)
                      >> std::quoted(record.receivedBy)
                      >> std::quoted(record.inspectorId)
                      >> passedInt
                      >> std::quoted(record.qcNotes)
                      >> std::quoted(record.dealerId)
                      >> std::quoted(record.destination)
                      >> std::quoted(record.transportMode)
                      >> std::quoted(record.buyerId)
                      >> record.salePrice
                      >> std::quoted(record.warrantyExpiry)
                      >> std::quoted(record.manufacturerId)
                      >> std::quoted(record.supplierId)
                      >> std::quoted(record.retailerId))) {
                std::ostringstream detail;
                detail << "parse error at line " << lineNo;
                throw std::runtime_error(detail.str());
            }

            if (stageInt < static_cast<int>(BlockStage::PRODUCTION) ||
                stageInt > static_cast<int>(BlockStage::CUSTOMER_SALE)) {
                std::ostringstream detail;
                detail << "invalid stage value at line " << lineNo;
                throw std::out_of_range(detail.str());
            }
            record.stage = static_cast<BlockStage>(stageInt);
            record.passed = (passedInt != 0);

            std::string sha3Hash;
            row >> std::quoted(sha3Hash);

            loadedChain.emplace_back(index,
                                     std::move(previousHash),
                                     std::move(currentHash),
                                     std::move(timestamp),
                                     nonce,
                                     record,
                                     std::move(sha3Hash));
            loadedIndex[record.vin].push_back(loadedChain.size() - 1);
        }

        if (loadedChain.size() != expectedCount) {
            std::ostringstream detail;
            detail << "expected " << expectedCount
                   << " blocks, parsed " << loadedChain.size();
            throw std::runtime_error(detail.str());
        }

        // Parsing throws standard exceptions internally so a single catch block can convert format/conversion failures into a user-safe audit entry.
        const ValidationResult loadedCheck = Validation::verifyChain(loadedChain);
        if (!loadedCheck.ok) {
            throw std::runtime_error("integrity validation failed: " + loadedCheck.message);
        }

        chain_ = std::move(loadedChain);
        vinIndex_ = std::move(loadedIndex);
        rebuildLinkedView();
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadBlockchain succeeded: " + path);
        return true;
    } catch (const std::exception& ex) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildCaughtExceptionMessage("loadBlockchain failed", ex));
    } catch (...) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildUnknownExceptionMessage("loadBlockchain failed"));
    }
    return false;
}



const AuditLog& Blockchain::getAuditLog() const noexcept {
    return auditLog_;
}

AuditLog& Blockchain::getAuditLog() noexcept {
    return auditLog_;
}





bool Blockchain::openDatabase(const std::string& dbPath) {
    try {
        db_ = std::make_unique<DatabaseManager>(dbPath);
        const std::string initError = db_->lastError();
        if (!db_->isOpen() || !initError.empty()) {
            throw std::runtime_error(db_->lastError().empty()
                                         ? "database open failed"
                                         : db_->lastError());
        }
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "openDatabase succeeded: " + dbPath);
        return true;
    } catch (const std::exception& ex) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildCaughtExceptionMessage("openDatabase failed", ex));
        db_.reset();
    } catch (...) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildUnknownExceptionMessage("openDatabase failed"));
        db_.reset();
    }
    return false;
}

bool Blockchain::isDatabaseOpen() const noexcept {
    return db_ && db_->isOpen();
}

bool Blockchain::saveToDB() {
    try {
        if (!isDatabaseOpen()) {
            throw std::runtime_error("database not open");
        }

        if (!db_->fullResync(chain_, auditLog_)) {
            throw std::runtime_error(db_->lastError().empty()
                                         ? "fullResync returned false"
                                         : db_->lastError());
        }

        const std::string msg = "saveToDB succeeded";
        auditLog_.log(AuditAction::PERSISTENCE_IO, msg);
        db_->insertAuditEntry(AuditAction::PERSISTENCE_IO, msg,
                              TimeUtil::nowIso8601());
        return true;
    } catch (const std::exception& ex) {
        const std::string msg =
            BuildCaughtExceptionMessage("saveToDB failed", ex);
        auditLog_.log(AuditAction::PERSISTENCE_IO, msg);
        if (isDatabaseOpen()) {
            db_->insertAuditEntry(AuditAction::PERSISTENCE_IO, msg,
                                  TimeUtil::nowIso8601());
        }
    } catch (...) {
        const std::string msg = BuildUnknownExceptionMessage("saveToDB failed");
        auditLog_.log(AuditAction::PERSISTENCE_IO, msg);
        if (isDatabaseOpen()) {
            db_->insertAuditEntry(AuditAction::PERSISTENCE_IO, msg,
                                  TimeUtil::nowIso8601());
        }
    }
    return false;
}

bool Blockchain::loadFromDB() {
    try {
        if (!isDatabaseOpen()) {
            throw std::runtime_error("database not open");
        }

        std::vector<Block> loaded = db_->loadAllBlocks();
        if (loaded.empty() && !db_->lastError().empty()) {
            throw std::runtime_error(db_->lastError());
        }

        // Rebuild the VIN index because the authoritative vector has been replaced by the blocks loaded from SQLite.
        std::map<std::string, std::vector<std::size_t>> loadedIndex;
        for (std::size_t i = 0; i < loaded.size(); ++i) {
            loadedIndex[loaded[i].getRecord().vin].push_back(i);
        }

        chain_ = std::move(loaded);
        vinIndex_ = std::move(loadedIndex);
        rebuildLinkedView();

        // Audit entries are stored separately in SQLite, so they are replayed into the in-memory linked audit log after the chain itself is restored.
        auto dbAudit = db_->loadAuditLog();
        if (dbAudit.empty() && !db_->lastError().empty()) {
            throw std::runtime_error("audit log load failed: " + db_->lastError());
        }
        for (const auto& entry : dbAudit) {
            AuditAction act = stringToAction(std::get<0>(entry));
            auditLog_.log(act, std::get<1>(entry), std::get<2>(entry));
        }

        ValidationResult result = Validation::verifyChain(chain_);
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      "loadFromDB: " + std::string(result.ok ? "PASS" : "FAIL") +
                      " - " + result.message);
        return result.ok;
    } catch (const std::exception& ex) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildCaughtExceptionMessage("loadFromDB failed", ex));
    } catch (...) {
        auditLog_.log(AuditAction::PERSISTENCE_IO,
                      BuildUnknownExceptionMessage("loadFromDB failed"));
    }
    return false;
}

std::vector<const Block*> Blockchain::searchBySQL(const std::string& query) const {

    if (!isDatabaseOpen()) {
        return searchGeneral(query);
    }

    std::vector<std::size_t> indices = db_->searchBlocksSQL(query);
    if (indices.empty() && !db_->lastError().empty()) {
        throw std::runtime_error("SQL search failed: " + db_->lastError());
    }

    std::vector<const Block*> results;
    results.reserve(indices.size());
    for (std::size_t idx : indices) {
        if (idx < chain_.size()) {
            results.push_back(&chain_[idx]);
        }
    }

    std::ostringstream detail;
    detail << "searchBySQL(\"" << query << "\") -> " << results.size()
           << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    if (isDatabaseOpen()) {
        db_->insertAuditEntry(AuditAction::SEARCH_PERFORMED, detail.str(),
                              TimeUtil::nowIso8601());
    }
    return results;
}

const DatabaseManager* Blockchain::getDatabase() const noexcept {
    return db_.get();
}

DatabaseManager* Blockchain::getDatabase() noexcept {
    return db_.get();
}

const LinkedBlockNode* Blockchain::getLinkedHead() const noexcept {
    return linkedView_.head();
}

const LinkedBlockchainView& Blockchain::getLinkedView() const noexcept {
    return linkedView_;
}

}  // namespace cw1
