#include "blockchain/Blockchain.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace cw1 {

namespace {

/// Case-insensitive substring search.
bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;

    auto toLower = [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    };

    std::string h, n;
    h.resize(haystack.size());
    n.resize(needle.size());
    std::transform(haystack.begin(), haystack.end(), h.begin(), toLower);
    std::transform(needle.begin(), needle.end(), n.begin(), toLower);

    return h.find(n) != std::string::npos;
}

} // namespace

// -- Core operations --------------------------------------------------

void Blockchain::addBlock(const CarRecord& record) {
    // 1. Determine the previous hash (genesis uses "0").
    const std::string prevHash =
        chain_.empty() ? "0" : chain_.back().getCurrentHash();

    // 2. Create the block and append to the global chain.
    chain_.emplace_back(chain_.size(), prevHash, record);

    // 3. Update the VIN index.
    vinIndex_[record.vin].push_back(chain_.size() - 1);

    // 4. Log the block addition to the audit trail.
    const Block& added = chain_.back();
    std::ostringstream detail;
    detail << "Added block #" << added.getIndex()
           << " for " << record.vin
           << " (" << stageToString(record.stage) << ")";
    auditLog_.log(AuditAction::BLOCK_ADDED, detail.str());
}

const std::vector<Block>& Blockchain::getChain() const noexcept {
    return chain_;
}

std::size_t Blockchain::totalBlocks() const noexcept {
    return chain_.size();
}

// -- Per-car lookups --------------------------------------------------

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
        throw std::runtime_error("VIN not found: " + vin);
    }
    // The last index in the vector is the most recent event for this car.
    return chain_[it->second.back()].getRecord().stage;
}

// -- Search / filter --------------------------------------------------

std::vector<const Block*> Blockchain::searchByBrand(const std::string& brand) const {
    std::vector<const Block*> results;
    for (const auto& block : chain_) {
        if (containsIgnoreCase(block.getRecord().manufacturer, brand)) {
            results.push_back(&block);
        }
    }
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
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
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
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
            containsIgnoreCase(stageToString(rec.stage), query)) {
            results.push_back(&block);
        }
    }
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
    std::ostringstream detail;
    detail << "searchGeneral(\"" << query << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

// -- Integrity --------------------------------------------------------

ValidationResult Blockchain::verifyIntegrity() const {
    ValidationResult result = Validation::verifyChain(chain_);
    // Log the integrity check result in the audit trail.
    std::ostringstream detail;
    detail << "verifyIntegrity() -> " << (result.ok ? "PASS" : "FAIL")
           << ": " << result.message;
    auditLog_.log(AuditAction::INTEGRITY_CHECK, detail.str());
    return result;
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

// -- Audit log --------------------------------------------------------

AuditLog& Blockchain::getAuditLog() const noexcept {
    // auditLog_ is mutable, so we can return a non-const reference from
    // a const method. This lets callers log CHAIN_VIEWED events even when
    // they only hold a const Blockchain&.
    return auditLog_;
}

AuditLog& Blockchain::getAuditLog() noexcept {
    return auditLog_;
}

} // namespace cw1
