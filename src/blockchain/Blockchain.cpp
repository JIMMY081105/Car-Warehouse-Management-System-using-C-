#include "blockchain/Blockchain.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace cw1 {

namespace {

/// Case-insensitive substring search.
bool containsIgnoreCase(const string& haystack, const string& needle) {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;

    auto toLower = [](unsigned char c) { return static_cast<char>(tolower(c)); };

    string h, n;
    h.resize(haystack.size());
    n.resize(needle.size());
    transform(haystack.begin(), haystack.end(), h.begin(), toLower);
    transform(needle.begin(), needle.end(), n.begin(), toLower);

    return h.find(n) != string::npos;
}

} // namespace

// -- Core operations --------------------------------------------------

void Blockchain::addBlock(const CarRecord& record) {
    // 1. Determine the previous hash (genesis uses "0").
    const string prevHash =
        chain_.empty() ? "0" : chain_.back().getCurrentHash();

    // 2. Create the block and append to the global chain.
    chain_.emplace_back(chain_.size(), prevHash, record);

    // 3. Update the VIN index.
    vinIndex_[record.vin].push_back(chain_.size() - 1);

    // 4. Log the block addition to the audit trail.
    const Block& added = chain_.back();
    ostringstream detail;
    detail << "Added block #" << added.getIndex()
           << " for " << record.vin
           << " (" << stageToString(record.stage) << ")";
    auditLog_.log(AuditAction::BLOCK_ADDED, detail.str());
}

const vector<Block>& Blockchain::getChain() const noexcept {
    return chain_;
}

size_t Blockchain::totalBlocks() const noexcept {
    return chain_.size();
}

// -- Per-car lookups --------------------------------------------------

vector<const Block*> Blockchain::getCarHistory(const string& vin) const {
    vector<const Block*> history;
    auto it = vinIndex_.find(vin);
    if (it != vinIndex_.end()) {
        history.reserve(it->second.size());
        for (size_t idx : it->second) {
            history.push_back(&chain_[idx]);
        }
    }
    return history;
}

bool Blockchain::hasVin(const string& vin) const {
    return vinIndex_.count(vin) > 0;
}

vector<string> Blockchain::getAllVins() const {
    vector<string> vins;
    vins.reserve(vinIndex_.size());
    for (const auto& [vin, indices] : vinIndex_) {
        vins.push_back(vin);
    }
    return vins;
}

BlockStage Blockchain::getLatestStage(const string& vin) const {
    auto it = vinIndex_.find(vin);
    if (it == vinIndex_.end() || it->second.empty()) {
        throw runtime_error("VIN not found: " + vin);
    }
    // The last index in the vector is the most recent event for this car.
    return chain_[it->second.back()].getRecord().stage;
}

// -- Search / filter --------------------------------------------------

vector<const Block*> Blockchain::searchByBrand(const string& brand) const {
    vector<const Block*> results;
    for (const auto& block : chain_) {
        if (containsIgnoreCase(block.getRecord().manufacturer, brand)) {
            results.push_back(&block);
        }
    }
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
    ostringstream detail;
    detail << "searchByBrand(\"" << brand << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

vector<const Block*> Blockchain::searchByStage(BlockStage stage) const {
    vector<const Block*> results;
    for (const auto& block : chain_) {
        if (block.getRecord().stage == stage) {
            results.push_back(&block);
        }
    }
    // Log the search in the audit trail (auditLog_ is mutable -- see header).
    ostringstream detail;
    detail << "searchByStage(" << stageToString(stage) << ") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

vector<const Block*> Blockchain::searchGeneral(const string& query) const {
    vector<const Block*> results;
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
    ostringstream detail;
    detail << "searchGeneral(\"" << query << "\") -> " << results.size() << " result(s)";
    auditLog_.log(AuditAction::SEARCH_PERFORMED, detail.str());
    return results;
}

// -- Integrity --------------------------------------------------------

ValidationResult Blockchain::verifyIntegrity() const {
    ValidationResult result = Validation::verifyChain(chain_);
    // Log the integrity check result in the audit trail.
    ostringstream detail;
    detail << "verifyIntegrity() -> " << (result.ok ? "PASS" : "FAIL")
           << ": " << result.message;
    auditLog_.log(AuditAction::INTEGRITY_CHECK, detail.str());
    return result;
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
