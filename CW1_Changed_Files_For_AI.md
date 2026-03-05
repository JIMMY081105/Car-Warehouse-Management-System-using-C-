# CW1 Changed Files (Full Code Export)

Generated for AI review of current uncommitted changes.

## Changed File Structure

```text
cw1/
  [Modified] CMakeLists.txt
  [Deleted] CW1_AI_Review_Context.md
  [Modified] include/blockchain/Block.hpp
  [Modified] include/blockchain/Blockchain.hpp
  [Modified] include/utils/AuditLog.hpp
  [Modified] src/blockchain/Block.cpp
  [Modified] src/blockchain/BlockStage.cpp
  [Modified] src/blockchain/Blockchain.cpp
  [Modified] src/blockchain/Validation.cpp
  [Modified] src/main_cli.cpp
  [Modified] src/main_gui.cpp
  [Modified] src/models/CarRecord.cpp
  [Modified] src/utils/AuditLog.cpp
  [Modified] src/utils/HashUtil.cpp
  [Modified] src/utils/TimeUtil.cpp
  [Added] include/blockchain/BlockFormatter.hpp
  [Added] include/utils/OperationTimer.hpp
```

## Full Code for Changed Files

### CMakeLists.txt

Status: **Modified**

```cmake
cmake_minimum_required(VERSION 3.16)

project(COMP2034_CW1_CarWarehouseBlockchain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Options ───────────────────────────────────────────────────────
option(BUILD_CLI "Build the terminal CLI version" ON)
option(BUILD_GUI "Build the ImGui GUI version"   ON)

# ── External packages ─────────────────────────────────────────────
find_package(OpenSSL REQUIRED)
find_package(OpenGL REQUIRED)

# GLFW: prefer system installation, fall back to the cloned subdirectory
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "System glfw3 not found -- using third_party/glfw")
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/glfw)
endif()

# ── Shared backend sources (blockchain engine) ────────────────────
set(BACKEND_SOURCES
    src/models/CarRecord.cpp
    src/blockchain/Block.cpp
    src/blockchain/BlockStage.cpp
    src/blockchain/Blockchain.cpp
    src/blockchain/Validation.cpp
    src/utils/AuditLog.cpp
    src/utils/HashUtil.cpp
    src/utils/TimeUtil.cpp
)

# ── ImGui sources ─────────────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ── CLI executable ────────────────────────────────────────────────
if(BUILD_CLI)
    add_executable(car_warehouse_cli
        src/main_cli.cpp
        ${BACKEND_SOURCES}
    )
    target_include_directories(car_warehouse_cli
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(car_warehouse_cli
        PRIVATE OpenSSL::Crypto
    )
    if(MSVC)
        target_compile_options(car_warehouse_cli PRIVATE /W4 /permissive-)
    else()
        target_compile_options(car_warehouse_cli PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# ── GUI executable ────────────────────────────────────────────────
if(BUILD_GUI)
    add_executable(car_warehouse_gui
        src/main_gui.cpp
        ${BACKEND_SOURCES}
        ${IMGUI_SOURCES}
    )
    target_include_directories(car_warehouse_gui
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
    )
    target_link_libraries(car_warehouse_gui
        PRIVATE
            OpenSSL::Crypto
            glfw
            OpenGL::GL
    )
    # Copy font file next to the executable so the runtime path works
    add_custom_command(TARGET car_warehouse_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fonts/JetBrainsMono-Regular.ttf
            $<TARGET_FILE_DIR:car_warehouse_gui>/fonts/JetBrainsMono-Regular.ttf
        COMMENT "Copying JetBrains Mono font next to executable"
    )
    if(MSVC)
        target_compile_options(car_warehouse_gui PRIVATE /W4 /permissive-)
    else()
        target_compile_options(car_warehouse_gui PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
```

### CW1_AI_Review_Context.md

Status: **Deleted**

```text
[File deleted in working tree; no current code content available.]
```

### include/blockchain/Block.hpp

Status: **Modified**

```cpp
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

    /// Debug/admin helper for tamper simulation coursework demo.
    /// Intentionally overrides the stored hash to break integrity checks.
    void debugOverrideCurrentHash(std::string forgedHash);

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
```

### include/blockchain/Blockchain.hpp

Status: **Modified**

```cpp
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

/// Hybrid blockchain: single global chain + VIN index for O(1) lookups.
///
/// Architecture:
///   chain_    -- vector<Block>  (immutable, chronological, source of truth)
///   vinIndex_ -- map<string, vector<size_t>>  (VIN -> block indices)
///
/// All blocks from all cars are stored in one chain for global integrity.
/// The VIN index provides fast per-car history lookups without scanning.
class Blockchain {
public:
    // -- Core operations -----------------------------------------------

    /// Append a new block to the global chain and update the VIN index.
    void addBlock(const CarRecord& record);

    /// Return the entire global chain (read-only).
    const std::vector<Block>& getChain() const noexcept;

    /// Total number of blocks across all cars.
    std::size_t totalBlocks() const noexcept;

    // -- Per-car lookups (powered by VIN index) ------------------------

    /// Get pointers to every block belonging to a given VIN (in order).
    std::vector<const Block*> getCarHistory(const std::string& vin) const;

    /// Check whether a VIN exists in the index.
    bool hasVin(const std::string& vin) const;

    /// Return a sorted list of all VINs currently tracked.
    std::vector<std::string> getAllVins() const;

    /// Get the latest stage a car has reached.
    BlockStage getLatestStage(const std::string& vin) const;

    // -- Search / filter -----------------------------------------------

    /// Find all blocks whose manufacturer matches (case-insensitive substring).
    std::vector<const Block*> searchByBrand(const std::string& brand) const;

    /// Find all blocks at a specific lifecycle stage.
    std::vector<const Block*> searchByStage(BlockStage stage) const;

    /// Find all blocks whose VIN, manufacturer, or model contain the query.
    std::vector<const Block*> searchGeneral(const std::string& query) const;

    // -- Integrity -----------------------------------------------------

    /// Verify the entire global chain (hashes + linkage).
    ValidationResult verifyIntegrity() const;

    /// Admin/debug coursework feature: intentionally tamper a block hash.
    /// Returns false with message if index is invalid.
    bool tamperBlockHash(std::size_t index,
                         const std::string& forgedHash,
                         std::string& message);

    // -- Audit log -----------------------------------------------------

    /// Access the audit log (const version -- works even on const Blockchain
    /// because auditLog_ is mutable; logging is a non-logical side effect).
    AuditLog& getAuditLog() const noexcept;

    /// Access the audit log (non-const version for direct mutation).
    AuditLog& getAuditLog() noexcept;

private:
    /// The single global chain -- source of truth.
    std::vector<Block> chain_;

    /// VIN index -- maps each VIN to the indices of its blocks in chain_.
    std::map<std::string, std::vector<std::size_t>> vinIndex_;

    /// Audit log -- mutable so it can be updated from const methods.
    /// Logging is a side effect that does not change the blockchain's
    /// logical state, so mutable is semantically correct here.
    mutable AuditLog auditLog_;
};

} // namespace cw1
```

### include/utils/AuditLog.hpp

Status: **Modified**

```cpp
#pragma once

// AuditLog.hpp -- Hand-written singly linked list for audit logging.
// Coursework requirement: demonstrates Linked List and Array of Pointers
// as data structures beyond the vector/map used elsewhere.

#include <cstddef>
#include <string>
#include <utility>

namespace cw1 {

// -- Audit action types ------------------------------------------------

enum class AuditAction {
    BLOCK_ADDED,        // A new block was appended to the blockchain
    INTEGRITY_CHECK,    // verifyIntegrity() was called
    SEARCH_PERFORMED,   // A search function was called
    CHAIN_VIEWED,       // The chain or a car record was displayed
    TAMPER_SIMULATED    // Debug/admin tamper simulation executed
};

/// Convert an AuditAction enum value to a human-readable string.
std::string actionToString(AuditAction action);

// -- Linked list node --------------------------------------------------

/// A single node in the audit linked list.
/// Each node holds the action, a description, a timestamp, and a raw
/// pointer to the next node -- forming a singly linked list.
struct AuditEntry {
    AuditAction action;
    std::string details;    // e.g. "Added block #5 for VIN1001 (Quality Check)"
    std::string timestamp;  // ISO-8601 from TimeUtil::nowIso8601()
    AuditEntry* next;       // Singly linked list pointer to next node

    AuditEntry(AuditAction act, std::string det, std::string ts)
        : action(act), details(std::move(det)), timestamp(std::move(ts)), next(nullptr) {}
};

// -- Singly linked list class ------------------------------------------

/// Hand-written singly linked list for audit logging.
/// New entries are appended at the tail for chronological order (O(1)).
///
/// Coursework requirement:
///   - Demonstrates Linked List: nodes, head/tail pointers, manual traversal,
///     dynamic node allocation and deallocation with new/delete.
///   - Demonstrates Array of Pointers: getRecentEntries() allocates and returns
///     a raw heap array of const AuditEntry* (caller must delete[]).
///   - Demonstrates manual memory management throughout.
class AuditLog {
public:
    AuditLog();
    ~AuditLog();

    // Non-copyable -- owns raw heap-allocated nodes.
    AuditLog(const AuditLog&)            = delete;
    AuditLog& operator=(const AuditLog&) = delete;

    /// Append a new audit entry at the tail -- O(1) due to tail pointer.
    /// Automatically captures the current UTC timestamp via TimeUtil.
    void log(AuditAction action, const std::string& details);

    /// Return the total number of entries -- O(1).
    std::size_t size() const noexcept;

    /// Return the head pointer for manual linked-list traversal (read-only).
    const AuditEntry* head() const noexcept;

    /// Return the most recent maxCount entries as a raw array of pointers.
    ///
    /// Demonstrates: Array of Pointers -- allocates a heap array of
    /// const AuditEntry* using new[]. Array slots point to nodes owned by
    /// this AuditLog (caller must NOT delete the pointed-to entries).
    /// The ARRAY ITSELF must be freed with delete[] by the caller.
    ///
    /// @param maxCount  Maximum number of entries to return.
    /// @param outCount  Set to the actual number of entries returned.
    /// @return Raw heap array of const AuditEntry* -- CALLER MUST delete[].
    const AuditEntry** getRecentEntries(std::size_t maxCount,
                                        std::size_t& outCount) const;

    /// Free all nodes and reset the list to empty.
    void clear();

private:
    AuditEntry* head_;   // First node (oldest entry)
    AuditEntry* tail_;   // Last node (newest entry) -- kept for O(1) append
    std::size_t count_;  // Total node count -- O(1) size()
};

/// RAII owner for AuditLog::getRecentEntries().
///
/// Coursework requirement remains intact:
/// - AuditLog still returns a raw heap-allocated array of pointers.
/// - This wrapper only automates delete[] to prevent call-site leaks.
class RecentEntryArray {
public:
    RecentEntryArray(const AuditLog& log, std::size_t maxCount);
    ~RecentEntryArray();

    RecentEntryArray(const RecentEntryArray&) = delete;
    RecentEntryArray& operator=(const RecentEntryArray&) = delete;

    RecentEntryArray(RecentEntryArray&& other) noexcept;
    RecentEntryArray& operator=(RecentEntryArray&& other) noexcept;

    std::size_t size() const noexcept;
    bool empty() const noexcept;

    const AuditEntry* operator[](std::size_t index) const noexcept;
    const AuditEntry* const* begin() const noexcept;
    const AuditEntry* const* end() const noexcept;

private:
    const AuditEntry** data_;
    std::size_t count_;
};

} // namespace cw1
```

### src/blockchain/Block.cpp

Status: **Modified**

```cpp
#include "blockchain/Block.hpp"

#include <random>
#include <sstream>
#include <utility>

#include "utils/HashUtil.hpp"
#include "utils/TimeUtil.hpp"

namespace cw1 {

Block::Block(std::size_t index, std::string previousHash, CarRecord record)
    : index_(index),
      previousHash_(std::move(previousHash)),
      timestamp_(TimeUtil::nowIso8601()),
      nonce_(generateNonce()),
      record_(std::move(record)) {
    currentHash_ = computeHash();

    // Coursework spec: the genesis block's previousHash must equal its
    // currentHash. After computing the hash with "0" as previousHash,
    // we update previousHash_ to match currentHash_.
    // computeHash() detects this case (index_ == 0) and always uses "0"
    // so that re-computation during integrity checks remains consistent.
    if (index_ == 0 && previousHash_ == "0") {
        previousHash_ = currentHash_;
    }
}

std::size_t Block::getIndex() const noexcept {
    return index_;
}

const std::string& Block::getCurrentHash() const noexcept {
    return currentHash_;
}

const std::string& Block::getPreviousHash() const noexcept {
    return previousHash_;
}

const std::string& Block::getTimestamp() const noexcept {
    return timestamp_;
}

std::uint64_t Block::getNonce() const noexcept {
    return nonce_;
}

const CarRecord& Block::getRecord() const noexcept {
    return record_;
}

std::string Block::computeHash() const {
    std::ostringstream payload;
    // For the genesis block (index_ == 0), always use "0" as the previousHash
    // in the hash computation, regardless of the stored previousHash_ value.
    // This is necessary because the constructor sets previousHash_ = currentHash_
    // after the hash is first computed, so we must reproduce the original "0"
    // input to verify the hash correctly during integrity checks.
    const std::string prevForHash = (index_ == 0) ? std::string("0") : previousHash_;
    payload << index_ << prevForHash << timestamp_ << nonce_ << record_.serialize();
    return HashUtil::sha256(payload.str());
}

std::string Block::toString() const {
    std::ostringstream out;
    out << "Block(index=" << index_
        << ", hash=" << currentHash_.substr(0, 12) << "..."
        << ", prev=" << (previousHash_.size() > 12 ? previousHash_.substr(0, 12) + "..." : previousHash_)
        << ", stage=" << stageToString(record_.stage)
        << ", vin=" << record_.vin << ")";
    return out.str();
}

void Block::debugOverrideCurrentHash(std::string forgedHash) {
    currentHash_ = std::move(forgedHash);
}

std::uint64_t Block::generateNonce() {
    static thread_local std::mt19937_64 engine{std::random_device{}()};
    static thread_local std::uniform_int_distribution<std::uint64_t> dist;
    return dist(engine);
}

} // namespace cw1
```

### src/blockchain/BlockStage.cpp

Status: **Modified**

```cpp
#include "blockchain/BlockStage.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace cw1 {

std::string stageToString(BlockStage stage) {
    switch (stage) {
        case BlockStage::PRODUCTION:       return "Production";
        case BlockStage::WAREHOUSE_INTAKE: return "Warehouse Intake";
        case BlockStage::QUALITY_CHECK:    return "Quality Check";
        case BlockStage::DEALER_DISPATCH:  return "Dealer Dispatch";
        case BlockStage::CUSTOMER_SALE:    return "Customer Sale";
    }
    return "Unknown";
}

BlockStage stringToStage(const std::string& text) {
    // Work with a lowercase copy for case-insensitive matching.
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lower == "production")       return BlockStage::PRODUCTION;
    if (lower == "warehouse intake") return BlockStage::WAREHOUSE_INTAKE;
    if (lower == "quality check")    return BlockStage::QUALITY_CHECK;
    if (lower == "dealer dispatch")  return BlockStage::DEALER_DISPATCH;
    if (lower == "customer sale")    return BlockStage::CUSTOMER_SALE;

    throw std::invalid_argument("Unknown stage: " + text);
}

} // namespace cw1
```

### src/blockchain/Blockchain.cpp

Status: **Modified**

```cpp
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
```

### src/blockchain/Validation.cpp

Status: **Modified**

```cpp
#include "blockchain/Validation.hpp"

namespace cw1::Validation {

ValidationResult verifyChain(const std::vector<Block>& blocks) {
    if (blocks.empty()) {
        return {true, "Blockchain is empty and therefore valid."};
    }

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const Block& current = blocks[i];

        // Check that the stored hash matches a fresh computation.
        if (current.getCurrentHash() != current.computeHash()) {
            return {false, "Hash mismatch at block index " + std::to_string(i)};
        }

        if (i == 0) {
            // Coursework spec: the genesis block's previousHash must equal
            // its own currentHash (set in the Block constructor after the
            // hash is computed with "0").
            if (current.getPreviousHash() != current.getCurrentHash()) {
                return {false, "Genesis block previousHash must equal its currentHash."};
            }
            continue;
        }

        // Every subsequent block must reference the previous block's hash.
        const Block& previous = blocks[i - 1];
        if (current.getPreviousHash() != previous.getCurrentHash()) {
            return {false, "Chain link mismatch at block index " + std::to_string(i)};
        }
    }

    return {true, "Blockchain integrity verified. " + std::to_string(blocks.size()) + " block(s) OK."};
}

} // namespace cw1::Validation
```

### src/main_cli.cpp

Status: **Modified**

```cpp
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "blockchain/BlockFormatter.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "utils/OperationTimer.hpp"

namespace {

using std::cin;
using std::cout;
using std::fixed;
using std::getline;
using std::left;
using std::setprecision;
using std::setw;
using std::string;
using std::to_string;
using std::vector;

int parseIntOrDefault(const string& value, int fallback) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

double parseDoubleOrDefault(const string& value, double fallback) {
    try {
        return std::stod(value);
    } catch (...) {
        return fallback;
    }
}

string prompt(const string& label) {
    string value;
    cout << "  " << label << ": ";
    getline(cin, value);
    return value;
}

void printOperationDuration(double seconds) {
    cout << "  Operation took: " << cw1::formatSeconds(seconds) << " s\n";
}

void printSeparator() {
    cout << "  " << string(60, '-') << '\n';
}

void printBlockDetail(const cw1::Block& block) {
    cout << cw1::formatBlockForDisplay(block);
}

void loadDemoData(cw1::Blockchain& chain) {
    cout << "\n  Loading demo fleet...\n";

    {
        cw1::CarRecord r;
        r.vin = "VIN1001"; r.manufacturer = "Perodua"; r.model = "Axia";
        r.color = "Silver"; r.productionYear = 2024;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Shah Alam Plant";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-A1"; r.receivedBy = "Ahmad bin Ismail";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-001"; r.passed = true; r.qcNotes = "All systems nominal";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KB-01"; r.destination = "Kota Bharu"; r.transportMode = "Truck";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::CUSTOMER_SALE;
        r.buyerId = "CUST-10201"; r.salePrice = 38000.00; r.warrantyExpiry = "2029-03-01";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1002"; r.manufacturer = "Toyota"; r.model = "Vios";
        r.color = "White"; r.productionYear = 2023;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Toyota Bukit Raja";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-B2"; r.receivedBy = "Lim Wei Jie";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-003"; r.passed = true; r.qcNotes = "Minor paint scratch - touched up";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05"; r.destination = "Kuala Lumpur"; r.transportMode = "Truck";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1003"; r.manufacturer = "Honda"; r.model = "City";
        r.color = "Blue"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Honda Pegoh, Melaka";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-C3"; r.receivedBy = "Raj Kumar";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-007"; r.passed = false; r.qcNotes = "Brake pad below spec - pending replacement";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1004"; r.manufacturer = "Proton"; r.model = "X50";
        r.color = "Red"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Proton Tanjung Malim";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-D4"; r.receivedBy = "Nurul Aina";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1005"; r.manufacturer = "Perodua"; r.model = "Myvi";
        r.color = "Black"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }

    const auto vins = chain.getAllVins();
    cout << "  Loaded " << vins.size() << " cars, "
         << chain.totalBlocks() << " total blocks.\n\n";

    const auto result = chain.verifyIntegrity();
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << "\n\n";
}

void viewAllCars(const cw1::Blockchain& chain) {
    const auto vins = chain.getAllVins();
    if (vins.empty()) {
        cout << "\n  No cars in the system.\n\n";
        return;
    }

    cout << "\n  " << left
         << setw(5)  << "No."
         << setw(12) << "VIN"
         << setw(12) << "Brand"
         << setw(10) << "Model"
         << setw(8)  << "Color"
         << setw(6)  << "Year"
         << setw(20) << "Latest Stage"
         << setw(7)  << "Blocks" << '\n';
    printSeparator();

    int num = 1;
    for (const auto& vin : vins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) {
            continue;
        }

        const auto& rec = history.front()->getRecord();
        const auto latestStage = chain.getLatestStage(vin);

        cout << "  " << left
             << setw(5)  << num++
             << setw(12) << rec.vin
             << setw(12) << rec.manufacturer
             << setw(10) << rec.model
             << setw(8)  << rec.color
             << setw(6)  << rec.productionYear
             << setw(20) << cw1::stageToString(latestStage)
             << setw(7)  << history.size() << '\n';
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed all cars summary (" + to_string(vins.size()) + " VINs)");
}

void viewCarByVin(const cw1::Blockchain& chain) {
    const string vin = prompt("Enter VIN");
    if (!chain.hasVin(vin)) {
        cout << "\n  VIN \"" << vin << "\" not found.\n\n";
        return;
    }

    const auto history = chain.getCarHistory(vin);
    cout << "\n  Blockchain for " << vin << " (" << history.size() << " block(s)):\n\n";

    for (const auto* block : history) {
        printBlockDetail(*block);
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed blockchain for " + vin + " (" + to_string(history.size()) + " blocks)");
}

void addNewCarStage(cw1::Blockchain& chain) {
    cout << "\n  Select lifecycle stage to add:\n";
    cout << "  1. Production\n";
    cout << "  2. Warehouse Intake\n";
    cout << "  3. Quality Check\n";
    cout << "  4. Dealer Dispatch\n";
    cout << "  5. Customer Sale\n";

    const string choice = prompt("Stage (1-5)");
    const int stageNum = parseIntOrDefault(choice, 0);
    if (stageNum < 1 || stageNum > 5) {
        cout << "  Invalid stage.\n\n";
        return;
    }

    cw1::CarRecord r;
    r.vin = prompt("VIN");
    r.manufacturer = prompt("Manufacturer");
    r.model = prompt("Model");
    r.color = prompt("Color");
    r.productionYear = parseIntOrDefault(prompt("Production Year"), 0);

    switch (stageNum) {
        case 1:
            r.stage = cw1::BlockStage::PRODUCTION;
            r.factoryLocation = prompt("Factory Location");
            break;
        case 2:
            r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
            r.warehouseLocation = prompt("Warehouse Location");
            r.receivedBy = prompt("Received By");
            break;
        case 3:
            r.stage = cw1::BlockStage::QUALITY_CHECK;
            r.inspectorId = prompt("Inspector ID");
            r.passed = (prompt("Passed? (y/n)") == "y");
            r.qcNotes = prompt("QC Notes");
            break;
        case 4:
            r.stage = cw1::BlockStage::DEALER_DISPATCH;
            r.dealerId = prompt("Dealer ID");
            r.destination = prompt("Destination");
            r.transportMode = prompt("Transport Mode");
            break;
        case 5:
            r.stage = cw1::BlockStage::CUSTOMER_SALE;
            r.buyerId = prompt("Buyer ID");
            r.salePrice = parseDoubleOrDefault(prompt("Sale Price (MYR)"), 0.0);
            r.warrantyExpiry = prompt("Warranty Expiry (YYYY-MM-DD)");
            break;
        default:
            break;
    }

    const double seconds = cw1::measureSeconds([&]() {
        chain.addBlock(r);
    });

    cout << "\n  Block added. Total blocks: " << chain.totalBlocks() << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

void searchCars(const cw1::Blockchain& chain) {
    cout << "\n  Search by:\n";
    cout << "  1. General (VIN / Brand / Model / Color)\n";
    cout << "  2. Brand\n";
    cout << "  3. Lifecycle Stage\n";

    const string choice = prompt("Choice (1-3)");
    vector<const cw1::Block*> results;
    double seconds = 0.0;

    if (choice == "1") {
        const string query = prompt("Search query");
        seconds = cw1::measureSeconds([&]() {
            results = chain.searchGeneral(query);
        });
    } else if (choice == "2") {
        const string brand = prompt("Brand name");
        seconds = cw1::measureSeconds([&]() {
            results = chain.searchByBrand(brand);
        });
    } else if (choice == "3") {
        cout << "  1=Production  2=Intake  3=QC  4=Dispatch  5=Sale\n";
        const int s = parseIntOrDefault(prompt("Stage (1-5)"), 0);
        if (s >= 1 && s <= 5) {
            seconds = cw1::measureSeconds([&]() {
                results = chain.searchByStage(static_cast<cw1::BlockStage>(s - 1));
            });
        } else {
            cout << "  Invalid stage.\n\n";
            return;
        }
    } else {
        cout << "  Invalid choice.\n\n";
        return;
    }

    cout << "\n  Found " << results.size() << " block(s):\n\n";
    for (const auto* block : results) {
        printBlockDetail(*block);
    }
    printOperationDuration(seconds);
    cout << '\n';
}

void verifyIntegrity(const cw1::Blockchain& chain) {
    cw1::ValidationResult result{};
    const double seconds = cw1::measureSeconds([&]() {
        result = chain.verifyIntegrity();
    });

    cout << "\n  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

void printGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    if (blocks.empty()) {
        cout << "\n  Global chain is empty.\n\n";
        return;
    }

    cout << "\n  Global Chain (" << blocks.size() << " block(s)):\n\n";
    for (const auto& block : blocks) {
        printBlockDetail(block);
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed global chain (" + to_string(blocks.size()) + " blocks)");
}

void viewAuditLog(const cw1::Blockchain& chain) {
    const cw1::AuditLog& log = chain.getAuditLog();
    cout << "\n  Audit Log - Total entries: " << log.size() << '\n';

    const string input = prompt("View last how many entries? (default 10)");
    std::size_t n = 10;
    if (!input.empty()) {
        const int parsed = parseIntOrDefault(input, 10);
        n = (parsed > 0) ? static_cast<std::size_t>(parsed) : 10;
    }

    cw1::RecentEntryArray entries(log, n);
    if (entries.empty()) {
        cout << "\n  No audit entries to display.\n\n";
        return;
    }

    cout << "\n  Showing last " << entries.size() << " entries:\n";
    printSeparator();

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const cw1::AuditEntry* entry = entries[i];
        if (entry == nullptr) {
            continue;
        }
        cout << "  [" << setw(4) << (i + 1) << "] "
             << left << setw(22) << entry->timestamp
             << setw(20) << cw1::actionToString(entry->action)
             << entry->details << '\n';
    }

    printSeparator();
    cout << '\n';
}

void simulateTamper(cw1::Blockchain& chain) {
    const std::size_t total = chain.totalBlocks();
    if (total == 0) {
        cout << "\n  Cannot tamper: chain is empty.\n\n";
        return;
    }

    cout << "\n  Tamper Simulation (Debug/Admin)\n";
    cout << "  This intentionally breaks blockchain integrity.\n";

    const string promptText = "Block index to tamper (0-" + to_string(total - 1) + ", blank=last)";
    const string indexInput = prompt(promptText);

    std::size_t index = total - 1;
    if (!indexInput.empty()) {
        const int parsed = parseIntOrDefault(indexInput, -1);
        if (parsed < 0 || static_cast<std::size_t>(parsed) >= total) {
            cout << "  Invalid block index.\n\n";
            return;
        }
        index = static_cast<std::size_t>(parsed);
    }

    const string forgedHash = prompt("Optional forged hash (blank = auto-generate)");

    bool success = false;
    string message;
    const double seconds = cw1::measureSeconds([&]() {
        success = chain.tamperBlockHash(index, forgedHash, message);
    });

    cout << '\n';
    cout << "  " << (success ? "Tamper simulation applied." : "Tamper simulation failed.") << '\n';
    cout << "  " << message << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

} // namespace

int main() {
    cw1::Blockchain chain;

    cout << "\n  ========================================\n";
    cout << "    Car Warehouse Blockchain System\n";
    cout << "    Hybrid Architecture (Global + Index)\n";
    cout << "  ========================================\n\n";

    loadDemoData(chain);

    while (true) {
        cout << "  ---- MENU ----\n";
        cout << "  1. View All Cars (summary)\n";
        cout << "  2. View Car Blockchain (by VIN)\n";
        cout << "  3. Add New Block (lifecycle stage)\n";
        cout << "  4. Search\n";
        cout << "  5. View Global Chain\n";
        cout << "  6. Verify Integrity\n";
        cout << "  7. View Audit Log\n";
        cout << "  8. Tamper Simulation (Debug)\n";
        cout << "  9. Exit\n";

        const string choice = prompt("Select (1-9)");

        if (choice == "1") { viewAllCars(chain); continue; }
        if (choice == "2") { viewCarByVin(chain); continue; }
        if (choice == "3") { addNewCarStage(chain); continue; }
        if (choice == "4") { searchCars(chain); continue; }
        if (choice == "5") { printGlobalChain(chain); continue; }
        if (choice == "6") { verifyIntegrity(chain); continue; }
        if (choice == "7") { viewAuditLog(chain); continue; }
        if (choice == "8") { simulateTamper(chain); continue; }
        if (choice == "9") { cout << "\n  Exiting.\n"; break; }

        cout << "  Invalid choice.\n\n";
    }

    return 0;
}
```

### src/main_gui.cpp

Status: **Modified**

```cpp
// main_gui.cpp -- ImGui/GLFW/OpenGL3 GUI for the Car Warehouse Blockchain.
// GitHub Dark theme, 3-panel layout (header | sidebar | content area).
// Views: Dashboard, Car Detail, Add Block, Global Chain, Audit Log, Integrity.

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "blockchain/BlockFormatter.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "utils/OperationTimer.hpp"

// =================================================================
//  SECTION 1: Color palette (GitHub Dark)
// =================================================================

/// Convert packed RGB hex to ImVec4 RGBA float.
static ImVec4 HexColor(uint32_t hex, float a = 1.0f) {
    return ImVec4(
        static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
        static_cast<float>((hex >>  8) & 0xFF) / 255.0f,
        static_cast<float>( hex        & 0xFF) / 255.0f, a);
}

static const ImVec4 COL_BG_MAIN    = HexColor(0x0d1117);  // main window
static const ImVec4 COL_BG_PANEL   = HexColor(0x161b22);  // panel / sidebar
static const ImVec4 COL_BG_CARD    = HexColor(0x21262d);  // cards / frames
static const ImVec4 COL_BG_HOVER   = HexColor(0x30363d);  // hover / borders
static const ImVec4 COL_TEXT       = HexColor(0xe6edf3);  // primary text
static const ImVec4 COL_MUTED      = HexColor(0x8b949e);  // secondary text
static const ImVec4 COL_VERY_MUTED = HexColor(0x484f58);  // very muted
static const ImVec4 COL_ACCENT     = HexColor(0x1f6feb);  // primary blue
static const ImVec4 COL_ACCENT_HO  = HexColor(0x388bfd);  // blue hover
static const ImVec4 COL_GREEN      = HexColor(0x238636);  // success / pass
static const ImVec4 COL_GREEN_BR   = HexColor(0x2ea043);  // green hover
static const ImVec4 COL_RED        = HexColor(0xda3633);  // error / fail
static const ImVec4 COL_YELLOW     = HexColor(0xd29922);  // warning / QC
static const ImVec4 COL_PURPLE     = HexColor(0x8957e5);  // hash values
static const ImVec4 COL_ORANGE     = HexColor(0xdb6d28);  // nonce / dispatch

// =================================================================
//  SECTION 2: GitHub Dark theme
// =================================================================

static void ApplyGitHubDarkTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 8.0f;  s.FrameRounding    = 6.0f;
    s.PopupRounding     = 8.0f;  s.TabRounding      = 6.0f;
    s.ScrollbarRounding = 6.0f;  s.GrabRounding     = 4.0f;
    s.ChildRounding     = 6.0f;
    s.FramePadding      = ImVec2(10.0f, 6.0f);
    s.ItemSpacing       = ImVec2(10.0f, 8.0f);
    s.WindowPadding     = ImVec2(16.0f, 16.0f);
    s.ItemInnerSpacing  = ImVec2(8.0f,  4.0f);
    s.ScrollbarSize     = 12.0f; s.GrabMinSize      = 8.0f;
    s.WindowBorderSize  = 1.0f;  s.FrameBorderSize  = 0.0f;
    s.ChildBorderSize   = 1.0f;  s.PopupBorderSize  = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                  = COL_TEXT;
    c[ImGuiCol_TextDisabled]          = COL_MUTED;
    c[ImGuiCol_WindowBg]              = COL_BG_MAIN;
    c[ImGuiCol_ChildBg]               = COL_BG_PANEL;
    c[ImGuiCol_PopupBg]               = COL_BG_PANEL;
    c[ImGuiCol_Border]                = COL_BG_HOVER;
    c[ImGuiCol_BorderShadow]          = ImVec4(0,0,0,0);
    c[ImGuiCol_FrameBg]               = COL_BG_CARD;
    c[ImGuiCol_FrameBgHovered]        = COL_BG_HOVER;
    c[ImGuiCol_FrameBgActive]         = HexColor(0x1f6feb, 0.35f);
    c[ImGuiCol_TitleBg]               = COL_BG_PANEL;
    c[ImGuiCol_TitleBgActive]         = COL_BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed]      = COL_BG_MAIN;
    c[ImGuiCol_MenuBarBg]             = COL_BG_PANEL;
    c[ImGuiCol_ScrollbarBg]           = COL_BG_MAIN;
    c[ImGuiCol_ScrollbarGrab]         = COL_VERY_MUTED;
    c[ImGuiCol_ScrollbarGrabHovered]  = COL_MUTED;
    c[ImGuiCol_ScrollbarGrabActive]   = COL_ACCENT;
    c[ImGuiCol_CheckMark]             = COL_ACCENT;
    c[ImGuiCol_SliderGrab]            = COL_ACCENT;
    c[ImGuiCol_SliderGrabActive]      = COL_ACCENT_HO;
    c[ImGuiCol_Button]                = COL_BG_CARD;
    c[ImGuiCol_ButtonHovered]         = COL_BG_HOVER;
    c[ImGuiCol_ButtonActive]          = HexColor(0x1f6feb, 0.4f);
    c[ImGuiCol_Header]                = COL_BG_CARD;
    c[ImGuiCol_HeaderHovered]         = COL_BG_HOVER;
    c[ImGuiCol_HeaderActive]          = HexColor(0x1f6feb, 0.4f);
    c[ImGuiCol_Separator]             = COL_BG_HOVER;
    c[ImGuiCol_SeparatorHovered]      = COL_MUTED;
    c[ImGuiCol_SeparatorActive]       = COL_ACCENT;
    c[ImGuiCol_ResizeGrip]            = COL_BG_HOVER;
    c[ImGuiCol_ResizeGripHovered]     = COL_MUTED;
    c[ImGuiCol_ResizeGripActive]      = COL_ACCENT;
    c[ImGuiCol_Tab]                   = COL_BG_PANEL;
    c[ImGuiCol_TabHovered]            = COL_BG_HOVER;
    c[ImGuiCol_TabSelected]           = COL_BG_CARD;
    c[ImGuiCol_TabSelectedOverline]   = COL_ACCENT;
    c[ImGuiCol_TabDimmed]             = COL_BG_PANEL;
    c[ImGuiCol_TabDimmedSelected]     = COL_BG_CARD;
    c[ImGuiCol_PlotLines]             = COL_ACCENT;
    c[ImGuiCol_PlotLinesHovered]      = COL_ACCENT_HO;
    c[ImGuiCol_PlotHistogram]         = COL_ACCENT;
    c[ImGuiCol_PlotHistogramHovered]  = COL_ACCENT_HO;
    c[ImGuiCol_TableHeaderBg]         = COL_BG_CARD;
    c[ImGuiCol_TableBorderStrong]     = COL_BG_HOVER;
    c[ImGuiCol_TableBorderLight]      = HexColor(0x21262d);
    c[ImGuiCol_TableRowBg]            = ImVec4(0,0,0,0);
    c[ImGuiCol_TableRowBgAlt]         = HexColor(0x161b22, 0.5f);
    c[ImGuiCol_TextSelectedBg]        = HexColor(0x1f6feb, 0.3f);
    c[ImGuiCol_DragDropTarget]        = COL_ACCENT;
    c[ImGuiCol_NavHighlight]          = COL_ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingDimBg]     = HexColor(0x0d1117, 0.7f);
    c[ImGuiCol_ModalWindowDimBg]      = HexColor(0x0d1117, 0.6f);
}

// =================================================================
//  SECTION 3: Application state
// =================================================================

enum class View { DASHBOARD, CAR_DETAIL, ADD_BLOCK, GLOBAL_CHAIN, AUDIT_LOG, INTEGRITY };

static View        g_view        = View::DASHBOARD;
static std::string g_selectedVin;
static char        g_searchBuf[256] = {};
static cw1::ValidationResult g_lastVerify = {false, "Not yet verified"};
static bool        g_verifyDone = false;
static double      g_lastVerifySeconds = 0.0;
static double      g_lastAddBlockSeconds = 0.0;
static double      g_lastSearchSeconds = 0.0;
static int         g_tamperIndex = -1;

// Sidebar search cache: avoids per-frame search calls and audit-log spam.
static std::string              g_cachedSearchQuery;
static std::vector<std::string> g_cachedSearchVins;

// Guard to prevent per-frame audit log spam.
// We only log a CHAIN_VIEWED event once when the user first switches to a view.
static View        g_lastLoggedView = View::DASHBOARD;
static std::string g_lastLoggedVin;

/// Log a CHAIN_VIEWED event only once per view switch (not every frame).
static void LogViewOnce(const cw1::Blockchain& chain, View currentView,
                        const std::string& vin, const std::string& details) {
    if (currentView != g_lastLoggedView || vin != g_lastLoggedVin) {
        chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED, details);
        g_lastLoggedView = currentView;
        g_lastLoggedVin  = vin;
    }
}

// Toast notification system
struct Toast { std::string msg; float remainingSecs; ImVec4 color; };
static std::vector<Toast> g_toasts;
static void PushToast(const std::string& msg, ImVec4 color, float dur = 3.0f) {
    g_toasts.push_back({msg, dur, color});
}

// Add-block form state
static int    g_formStage           = 0;
static char   g_formVin[64]         = {};
static char   g_formMfr[64]         = {};
static char   g_formModel[64]       = {};
static char   g_formColor[64]       = {};
static int    g_formYear            = 2025;
static char   g_formFactory[128]    = {};
static char   g_formWarehouse[64]   = {};
static char   g_formReceivedBy[64]  = {};
static char   g_formInspector[64]   = {};
static bool   g_formPassed          = true;
static char   g_formQcNotes[256]    = {};
static char   g_formDealerId[64]    = {};
static char   g_formDestination[64] = {};
static char   g_formTransport[64]   = {};
static char   g_formBuyerId[64]     = {};
static double g_formSalePrice       = 0.0;
static char   g_formWarranty[32]    = {};
static int    g_auditN              = 20;

// =================================================================
//  SECTION 4: Demo data (no cout, no sleep_for)
// =================================================================

static void loadDemoData(cw1::Blockchain& chain) {
    { cw1::CarRecord r;
      r.vin="VIN1001"; r.manufacturer="Perodua"; r.model="Axia";
      r.color="Silver"; r.productionYear=2024;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Shah Alam Plant";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-A1"; r.receivedBy="Ahmad bin Ismail";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-001"; r.passed=true; r.qcNotes="All systems nominal";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KB-01"; r.destination="Kota Bharu"; r.transportMode="Truck";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::CUSTOMER_SALE;
      r.buyerId="CUST-10201"; r.salePrice=38000.00; r.warrantyExpiry="2029-03-01";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1002"; r.manufacturer="Toyota"; r.model="Vios";
      r.color="White"; r.productionYear=2023;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Toyota Bukit Raja";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-B2"; r.receivedBy="Lim Wei Jie";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-003"; r.passed=true; r.qcNotes="Minor paint scratch - touched up";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KL-05"; r.destination="Kuala Lumpur"; r.transportMode="Truck";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1003"; r.manufacturer="Honda"; r.model="City";
      r.color="Blue"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Honda Pegoh, Melaka";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-C3"; r.receivedBy="Raj Kumar";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-007"; r.passed=false;
      r.qcNotes="Brake pad below spec - pending replacement";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1004"; r.manufacturer="Proton"; r.model="X50";
      r.color="Red"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Proton Tanjung Malim";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-D4"; r.receivedBy="Nurul Aina";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1005"; r.manufacturer="Perodua"; r.model="Myvi";
      r.color="Black"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Perodua Rawang";
      chain.addBlock(r); }
}

// =================================================================
//  SECTION 5: Helper utilities
// =================================================================

static ImVec4 StageColor(cw1::BlockStage stage) {
    switch (stage) {
        case cw1::BlockStage::PRODUCTION:       return COL_MUTED;
        case cw1::BlockStage::WAREHOUSE_INTAKE: return COL_ACCENT;
        case cw1::BlockStage::QUALITY_CHECK:    return COL_YELLOW;
        case cw1::BlockStage::DEALER_DISPATCH:  return COL_ORANGE;
        case cw1::BlockStage::CUSTOMER_SALE:    return COL_GREEN;
        default:                                return COL_TEXT;
    }
}

static float StageProgress(cw1::BlockStage stage) {
    return (static_cast<float>(stage) + 1.0f) / 5.0f;
}

static std::string Truncate(const std::string& s, size_t maxLen) {
    return (s.size() <= maxLen) ? s : s.substr(0, maxLen) + "...";
}

// =================================================================
//  SECTION 6: Header bar
// =================================================================

static void RenderHeader(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f));
    ImGui::BeginChild("##header", ImVec2(-1.0f, 54.0f), true);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    ImGui::TextColored(COL_ACCENT, "%s", "* ");
    ImGui::SameLine();
    ImGui::Text("Car Warehouse Blockchain");

    float statsWidth = 340.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - statsWidth
                    + ImGui::GetCursorPosX() - 16.0f);

    char badge[64];
    snprintf(badge, sizeof(badge), "  %zu blocks  ", chain.totalBlocks());
    ImGui::TextColored(COL_ACCENT, "%s", badge);
    ImGui::SameLine();
    snprintf(badge, sizeof(badge), "  %zu cars  ", chain.getAllVins().size());
    ImGui::TextColored(COL_GREEN, "%s", badge);
    ImGui::SameLine();
    if (g_verifyDone) {
        if (g_lastVerify.ok)
            ImGui::TextColored(COL_GREEN, "  Verified  ");
        else
            ImGui::TextColored(COL_RED, "  Invalid  ");
    } else {
        ImGui::TextColored(COL_MUTED, "  Checking...  ");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// =================================================================
//  SECTION 7: Left sidebar
// =================================================================

static void RenderSidebar(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##sidebar", ImVec2(280.0f, -1.0f), true);

    // Search box
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##search", "Search VIN, brand, model...",
                             g_searchBuf, sizeof(g_searchBuf));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Navigation buttons
    struct NavItem { const char* label; View view; };
    static const NavItem navItems[] = {
        { "  Dashboard",        View::DASHBOARD    },
        { "  + Add New Block",  View::ADD_BLOCK     },
        { "  Global Chain",     View::GLOBAL_CHAIN  },
        { "  Audit Log",        View::AUDIT_LOG     },
        { "  Verify Integrity", View::INTEGRITY     },
    };

    for (const auto& item : navItems) {
        bool active = (g_view == item.view);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1,1,1,1));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        }
        if (ImGui::Button(item.label, ImVec2(-1.0f, 36.0f))) {
            g_view = item.view;
            g_selectedVin.clear();
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Car list header
    auto allVins = chain.getAllVins();
    ImGui::TextColored(COL_MUTED, "VEHICLES (%zu)", allVins.size());
    ImGui::Spacing();

    // Build filtered list.
    // Search is run only when query text changes to keep timing meaningful
    // and avoid writing audit log entries every frame.
    std::vector<std::string> displayVins;
    const std::string query(g_searchBuf);
    if (!query.empty()) {
        if (query != g_cachedSearchQuery) {
            g_cachedSearchQuery = query;
            g_cachedSearchVins.clear();

            const double seconds = cw1::measureSeconds([&]() {
                const auto hits = chain.searchGeneral(query);
                std::vector<std::string> seen;
                for (const auto* blk : hits) {
                    const std::string& vin = blk->getRecord().vin;
                    if (std::find(seen.begin(), seen.end(), vin) == seen.end()) {
                        seen.push_back(vin);
                        g_cachedSearchVins.push_back(vin);
                    }
                }
            });
            g_lastSearchSeconds = seconds;
        }
        displayVins = g_cachedSearchVins;
    } else {
        g_cachedSearchQuery.clear();
        g_cachedSearchVins.clear();
        displayVins = allVins;
    }

    if (!query.empty()) {
        ImGui::TextColored(COL_VERY_MUTED, "Search operation took: %s s",
                           cw1::formatSeconds(g_lastSearchSeconds).c_str());
        ImGui::Spacing();
    }

    for (const auto& vin : displayVins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) continue;
        const auto& rec    = history.front()->getRecord();
        bool        sel    = (vin == g_selectedVin);
        cw1::BlockStage st = chain.getLatestStage(vin);

        // Blue left border for selected card
        if (sel) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                p, ImVec2(p.x + 3.0f, p.y + 72.0f),
                ImGui::ColorConvertFloat4ToU32(COL_ACCENT));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Header,        sel ? COL_BG_HOVER : COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.25f));

        char cardId[32]; snprintf(cardId, sizeof(cardId), "##car_%s", vin.c_str());
        bool clicked = ImGui::Selectable(cardId, sel, 0, ImVec2(-1.0f, 68.0f));
        ImGui::PopStyleColor(3);

        if (clicked) { g_selectedVin = vin; g_view = View::CAR_DETAIL; }

        float  topY = ImGui::GetCursorPos().y - 68.0f;
        float  offX = sel ? 22.0f : 16.0f;

        ImGui::SetCursorPos(ImVec2(offX, topY + 6.0f));
        ImGui::TextColored(sel ? COL_ACCENT : COL_TEXT, "%s", vin.c_str());

        ImGui::SetCursorPos(ImVec2(offX, topY + 24.0f));
        ImGui::TextColored(COL_MUTED, "%s %s - %s",
            rec.manufacturer.c_str(), rec.model.c_str(), rec.color.c_str());

        ImGui::SetCursorPos(ImVec2(offX, topY + 42.0f));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, StageColor(st));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,       COL_BG_MAIN);
        ImGui::ProgressBar(StageProgress(st), ImVec2(150.0f, 8.0f), "");
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, " %s", cw1::stageToString(st).c_str());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY());
        ImGui::Spacing();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}


// =================================================================

// =================================================================
//  SECTION 8: Dashboard view
// =================================================================

static void RenderDashboard(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    size_t totalBlocks = chain.totalBlocks();
    size_t totalCars   = chain.getAllVins().size();

    // ── Stat cards ────────────────────────────────────────────────
    if (ImGui::BeginTable("##stats", 4,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoPadOuterX,
            ImVec2(-1, 90))) {
        ImGui::TableNextRow();

        auto StatCard = [](const char* value, const char* label, ImVec4 col) {
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
            ImGui::BeginChild(label, ImVec2(-1, 80), true);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
            ImGui::TextColored(col, "%s", value);
            ImGui::TextColored(COL_MUTED, "%s", label);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        };

        char buf1[32], buf2[32], buf4[32];
        snprintf(buf1, sizeof(buf1), "%zu", totalBlocks);
        snprintf(buf2, sizeof(buf2), "%zu", totalCars);
        snprintf(buf4, sizeof(buf4), "%zu", chain.getAuditLog().size());

        StatCard(buf1, "Total Blocks",    COL_ACCENT);
        StatCard(buf2, "Unique Vehicles", COL_GREEN_BR);
        StatCard(g_verifyDone && g_lastVerify.ok ? "VERIFIED" : (g_verifyDone ? "TAMPERED" : "N/A"),
                 "Integrity",
                 g_verifyDone && g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
        StatCard(buf4, "Audit Events",    COL_PURPLE);

        ImGui::EndTable();
    }

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "RECENT BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    // ── Recent blocks table ────────────────────────────────────────
    ImGuiTableFlags tflags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                             ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##recent", 6, tflags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        // Most-recent first
        size_t count = blocks.size();
        for (size_t ri = 0; ri < count; ++ri) {
            size_t i = count - 1 - ri;
            const cw1::Block& b = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", b.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), 12).c_str());

            ImGui::TableNextColumn();
            char selId[80];
            snprintf(selId, sizeof(selId), "%s##rv%zu", rec.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = rec.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", rec.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(rec.stage), "%s",
                               cw1::stageToString(rec.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", b.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
}

// =================================================================
//  SECTION 9: Car Detail view
// =================================================================

static void RenderCarDetail(const cw1::Blockchain& chain) {
    if (ImGui::Button("< Back")) {
        g_view = View::DASHBOARD;
        return;
    }
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " / ");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "%s", g_selectedVin.c_str());

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    if (g_selectedVin.empty() || !chain.hasVin(g_selectedVin)) {
        ImGui::TextColored(COL_RED, "VIN not found.");
        return;
    }

    auto history = chain.getCarHistory(g_selectedVin);
    if (history.empty()) {
        ImGui::TextColored(COL_MUTED, "No blocks for this VIN.");
        return;
    }

    // Car header from first block
    const cw1::CarRecord& first = history.front()->getRecord();
    ImGui::TextColored(COL_TEXT, "%s %s  (%d)   %s",
        first.manufacturer.c_str(), first.model.c_str(),
        first.productionYear, first.color.c_str());
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "LIFECYCLE TIMELINE  (%zu block(s))", history.size());
    ImGui::Separator(); ImGui::Spacing();

    for (size_t idx = 0; idx < history.size(); ++idx) {
        const cw1::Block& blk = *history[idx];
        const cw1::CarRecord& rec = blk.getRecord();

        char hdrLabel[128];
        snprintf(hdrLabel, sizeof(hdrLabel), "Block #%zu  |  %s##det%zu",
                 blk.getIndex(), cw1::stageToString(rec.stage).c_str(), idx);

        ImGui::PushStyleColor(ImGuiCol_Header,        COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.2f));
        bool open = ImGui::CollapsingHeader(hdrLabel,
                        idx == history.size() - 1
                            ? ImGuiTreeNodeFlags_DefaultOpen
                            : ImGuiTreeNodeFlags_None);
        ImGui::PopStyleColor(3);
        if (!open) continue;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        // Use hdrLabel as unique child ID
        ImGui::BeginChild(hdrLabel, ImVec2(-1, 0), true);

        ImGui::TextColored(COL_MUTED, "Canonical Block Output");
        ImGui::Spacing();

        const std::string formatted = cw1::formatBlockForDisplay(blk);
        ImGui::PushStyleColor(ImGuiCol_Text, COL_TEXT);
        ImGui::TextUnformatted(formatted.c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }
    LogViewOnce(chain, View::CAR_DETAIL, g_selectedVin,
                "Car detail viewed: " + g_selectedVin);
}

// =================================================================
//  SECTION 10: Add Block form
// =================================================================

static const char* k_stageNames[] = {
    "PRODUCTION", "WAREHOUSE_INTAKE", "QUALITY_CHECK",
    "DEALER_DISPATCH", "CUSTOMER_SALE"
};

static void RenderAddBlock(cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Add Block to Chain");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "COMMON FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::SetNextItemWidth(240);
    ImGui::Combo("Stage##addstage", &g_formStage, k_stageNames, 5);
    ImGui::Spacing();

    // Helper: label + InputText on one row
    auto LabelText = [](const char* lbl, char* buf, size_t sz) {
        ImGui::TextColored(COL_MUTED, "%-18s", lbl);
        ImGui::SameLine();
        char id[64]; snprintf(id, sizeof(id), "##add_%s", lbl);
        ImGui::SetNextItemWidth(300);
        ImGui::InputText(id, buf, sz);
    };

    LabelText("VIN",          g_formVin,   sizeof(g_formVin));
    // VIN status badge
    ImGui::SameLine(0, 12);
    if (g_formVin[0] != '\0') {
        if (chain.hasVin(std::string(g_formVin)))
            ImGui::TextColored(COL_GREEN_BR, "[+ Existing VIN]");
        else
            ImGui::TextColored(COL_ACCENT,   "[* New VIN]");
    }

    LabelText("Manufacturer", g_formMfr,   sizeof(g_formMfr));
    LabelText("Model",        g_formModel, sizeof(g_formModel));
    LabelText("Color",        g_formColor, sizeof(g_formColor));

    ImGui::TextColored(COL_MUTED, "%-18s", "Production Year");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##add_year", &g_formYear);

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "STAGE-SPECIFIC FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    switch (g_formStage) {
    case 0: // PRODUCTION
        LabelText("Factory Location", g_formFactory, sizeof(g_formFactory));
        break;
    case 1: // WAREHOUSE_INTAKE
        LabelText("Warehouse",  g_formWarehouse,   sizeof(g_formWarehouse));
        LabelText("Received By",g_formReceivedBy,  sizeof(g_formReceivedBy));
        break;
    case 2: // QUALITY_CHECK
        LabelText("Inspector ID", g_formInspector, sizeof(g_formInspector));
        ImGui::TextColored(COL_MUTED, "%-18s", "Passed");
        ImGui::SameLine();
        ImGui::Checkbox("##add_passed", &g_formPassed);
        LabelText("QC Notes", g_formQcNotes, sizeof(g_formQcNotes));
        break;
    case 3: // DEALER_DISPATCH
        LabelText("Dealer ID",    g_formDealerId,   sizeof(g_formDealerId));
        LabelText("Destination",  g_formDestination,sizeof(g_formDestination));
        LabelText("Transport",    g_formTransport,  sizeof(g_formTransport));
        break;
    case 4: // CUSTOMER_SALE
        LabelText("Buyer ID",     g_formBuyerId, sizeof(g_formBuyerId));
        ImGui::TextColored(COL_MUTED, "%-18s", "Sale Price (MYR)");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputDouble("##add_price", &g_formSalePrice, 1000.0, 10000.0, "%.2f");
        LabelText("Warranty Expiry", g_formWarranty, sizeof(g_formWarranty));
        break;
    }

    ImGui::Spacing(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    bool submit = ImGui::Button("  Add Block  ", ImVec2(150, 36));
    ImGui::PopStyleColor(3);

    if (submit) {
        cw1::CarRecord r;
        r.vin            = g_formVin;
        r.manufacturer   = g_formMfr;
        r.model          = g_formModel;
        r.color          = g_formColor;
        r.productionYear = g_formYear;
        r.stage          = static_cast<cw1::BlockStage>(g_formStage);

        switch (g_formStage) {
        case 0:
            r.factoryLocation  = g_formFactory;
            break;
        case 1:
            r.warehouseLocation = g_formWarehouse;
            r.receivedBy        = g_formReceivedBy;
            break;
        case 2:
            r.inspectorId = g_formInspector;
            r.passed      = g_formPassed;
            r.qcNotes     = g_formQcNotes;
            break;
        case 3:
            r.dealerId      = g_formDealerId;
            r.destination   = g_formDestination;
            r.transportMode = g_formTransport;
            break;
        case 4:
            r.buyerId        = g_formBuyerId;
            r.salePrice      = g_formSalePrice;
            r.warrantyExpiry = g_formWarranty;
            break;
        }

        const double seconds = cw1::measureSeconds([&]() {
            chain.addBlock(r);
        });
        g_lastAddBlockSeconds = seconds;
        PushToast("Block #" + std::to_string(chain.totalBlocks() - 1) +
                  " added for " + std::string(g_formVin) +
                  " | Operation took: " + cw1::formatSeconds(seconds) + " s",
                  COL_GREEN_BR);

        // Clear all form buffers
        g_formVin[0]    = '\0'; g_formMfr[0]         = '\0';
        g_formModel[0]  = '\0'; g_formColor[0]        = '\0';
        g_formYear      = 2025;
        g_formFactory[0]= '\0'; g_formWarehouse[0]    = '\0';
        g_formReceivedBy[0]= '\0'; g_formInspector[0] = '\0';
        g_formPassed    = true;  g_formQcNotes[0]     = '\0';
        g_formDealerId[0]= '\0'; g_formDestination[0] = '\0';
        g_formTransport[0]= '\0'; g_formBuyerId[0]    = '\0';
        g_formSalePrice = 0.0;   g_formWarranty[0]    = '\0';
    }

    if (g_lastAddBlockSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last add-block operation took: %s s",
                           cw1::formatSeconds(g_lastAddBlockSeconds).c_str());
    }
}

// =================================================================
//  SECTION 11: Global Chain view
// =================================================================

static void RenderGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();

    ImGui::TextColored(COL_ACCENT, "Global Chain");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu block(s))", blocks.size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGuiTableFlags tf = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##global", 8, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Prev Hash",    ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Nonce",        ImGuiTableColumnFlags_WidthFixed,   90);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < blocks.size(); ++i) {
            const cw1::Block& b   = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", b.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", b.getCurrentHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getPreviousHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", b.getPreviousHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            char selId[80];
            snprintf(selId, sizeof(selId), "%s##gc%zu", rec.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = rec.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", rec.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(rec.stage), "%s",
                               cw1::stageToString(rec.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ORANGE, "%llu",
                               static_cast<unsigned long long>(b.getNonce()));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", b.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
    LogViewOnce(chain, View::GLOBAL_CHAIN, "",
                "Global chain viewed");
}

// =================================================================
//  SECTION 12: Audit Log view
// =================================================================

static void RenderAuditLog(const cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Audit Log");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu total event(s))", chain.getAuditLog().size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Show last");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##auditn", &g_auditN);
    if (g_auditN < 1)  g_auditN = 1;
    if (g_auditN > 500) g_auditN = 500;
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, "entries");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // Retrieve via Array-of-Pointers through RAII wrapper.
    cw1::RecentEntryArray entries(chain.getAuditLog(),
                                  static_cast<std::size_t>(g_auditN));

    auto ActionColor = [](cw1::AuditAction act) -> ImVec4 {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return COL_GREEN_BR;
        case cw1::AuditAction::INTEGRITY_CHECK:  return COL_YELLOW;
        case cw1::AuditAction::SEARCH_PERFORMED: return COL_ACCENT;
        case cw1::AuditAction::CHAIN_VIEWED:     return COL_PURPLE;
        case cw1::AuditAction::TAMPER_SIMULATED: return COL_RED;
        default:                                  return COL_TEXT;
        }
    };
    auto ActionName = [](cw1::AuditAction act) -> const char* {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case cw1::AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case cw1::AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case cw1::AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        case cw1::AuditAction::TAMPER_SIMULATED: return "TAMPER_SIMULATED";
        default:                                  return "UNKNOWN";
        }
    };

    ImGuiTableFlags tf = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##auditlog", 3, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Action",    ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Details",   ImGuiTableColumnFlags_WidthStretch, 4.0f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < entries.size(); ++i) {
            const cw1::AuditEntry* e = entries[i];
            if (e == nullptr) {
                continue;
            }
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", e->timestamp.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(ActionColor(e->action), "%s", ActionName(e->action));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_TEXT, "%s", e->details.c_str());
        }
        ImGui::EndTable();
    }
}

// =================================================================
//  SECTION 13: Integrity check view
// =================================================================

static void RenderIntegrity(cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Blockchain Integrity");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    if (ImGui::Button("  Run Verification  ", ImVec2(200, 36))) {
        g_lastVerifySeconds = cw1::measureSeconds([&]() {
            g_lastVerify = chain.verifyIntegrity();
        });
        g_verifyDone = true;
        PushToast((g_lastVerify.ok ? "Chain VERIFIED OK" : "Chain TAMPERED!") +
                  std::string(" | Operation took: ") +
                  cw1::formatSeconds(g_lastVerifySeconds) + " s",
                  g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("Tamper block index##tamper", &g_tamperIndex);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button,        COL_RED);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0xe5534b));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0xb62324));
    if (ImGui::Button("Simulate Tamper", ImVec2(180, 36))) {
        const std::size_t total = chain.totalBlocks();
        if (total == 0) {
            PushToast("Tamper failed: chain is empty.", COL_RED);
        } else {
            std::size_t target = total - 1;
            bool validIndex = true;
            if (g_tamperIndex >= 0) {
                const std::size_t requested = static_cast<std::size_t>(g_tamperIndex);
                if (requested < total) {
                    target = requested;
                } else {
                    PushToast("Tamper failed: index out of range.", COL_RED);
                    validIndex = false;
                }
            }

            if (validIndex) {
                std::string tamperMsg;
                bool success = false;
                const double tamperSeconds = cw1::measureSeconds([&]() {
                    success = chain.tamperBlockHash(target, "", tamperMsg);
                });

                if (success) {
                    PushToast(tamperMsg + " | Operation took: " +
                              cw1::formatSeconds(tamperSeconds) + " s",
                              COL_RED);
                } else {
                    PushToast(tamperMsg, COL_RED);
                }
            }
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing(); ImGui::Spacing();

    if (!g_verifyDone) {
        ImGui::TextColored(COL_MUTED, "Press Run Verification to check the chain.");
        return;
    }

    // Large PASS / FAIL banner
    ImVec4 bannerCol  = g_lastVerify.ok ? COL_GREEN : COL_RED;
    const char* bannerTxt = g_lastVerify.ok ? "VERIFIED" : "TAMPERED";

    ImGui::PushStyleColor(ImGuiCol_ChildBg, bannerCol);
    ImGui::BeginChild("##intbanner", ImVec2(-1, 80), true);
    ImGui::SetCursorPos(ImVec2(20, 18));
    ImGui::SetWindowFontScale(2.0f);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", bannerTxt);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "Result: ");
    ImGui::SameLine();
    ImGui::TextWrapped("%s", g_lastVerify.message.c_str());

    if (g_lastVerifySeconds > 0.0) {
        ImGui::TextColored(COL_VERY_MUTED, "Last verification operation took: %s s",
                           cw1::formatSeconds(g_lastVerifySeconds).c_str());
    }

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    size_t total = chain.totalBlocks();
    size_t cars  = chain.getAllVins().size();

    ImGui::TextColored(COL_MUTED, "Total blocks:    ");
    ImGui::SameLine(); ImGui::TextColored(COL_TEXT, "%zu", total);

    ImGui::TextColored(COL_MUTED, "Unique vehicles: ");
    ImGui::SameLine(); ImGui::TextColored(COL_TEXT, "%zu", cars);

    if (total > 0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Block range: 0 .. %zu", total - 1);
    }
}

// =================================================================
//  SECTION 14: Toast renderer
// =================================================================

static void RenderToasts(float deltaTime) {
    // Remove expired toasts first
    g_toasts.erase(
        std::remove_if(g_toasts.begin(), g_toasts.end(),
                       [](const Toast& t) { return t.remainingSecs <= 0.0f; }),
        g_toasts.end());

    const float toastW = 340.0f;
    const float toastH = 44.0f;
    const float margin = 20.0f;
    const float topY   = 70.0f;

    ImGuiIO& io = ImGui::GetIO();

    for (size_t i = 0; i < g_toasts.size(); ++i) {
        Toast& t = g_toasts[i];

        // Alpha fade-out in last 0.5 s
        float alpha = (t.remainingSecs < 0.5f) ? (t.remainingSecs / 0.5f) : 1.0f;
        if (alpha < 0.0f) alpha = 0.0f;

        float posX = io.DisplaySize.x - toastW - margin;
        float posY = topY + static_cast<float>(i) * (toastH + 8.0f);

        char wname[32];
        snprintf(wname, sizeof(wname), "##toast%zu", i);

        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH));
        ImGui::SetNextWindowBgAlpha(0.92f * alpha);

        ImGuiWindowFlags wf =
            ImGuiWindowFlags_NoDecoration       |
            ImGuiWindowFlags_NoMove             |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNav              |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, t.color);
        ImGui::PushStyleColor(ImGuiCol_Border,   t.color);
        if (ImGui::Begin(wname, nullptr, wf)) {
            ImGui::SetCursorPos(ImVec2(12.0f, 12.0f));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha),
                               "%s", t.msg.c_str());
        }
        ImGui::End();
        ImGui::PopStyleColor(2);

        t.remainingSecs -= deltaTime;
    }
}

// =================================================================
//  SECTION 15: main()
// =================================================================

int main() {
    // ── 1. GLFW ───────────────────────────────────────────────────
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1400, 900, "Car Warehouse Blockchain System", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // ── 2. ImGui ──────────────────────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ApplyGitHubDarkTheme();

    // ── 3. Font (fallback to default if file absent) ──────────────
    {
        const char* fontPath = "fonts/JetBrainsMono-Regular.ttf";
        FILE* fp = fopen(fontPath, "rb");
        if (fp) {
            fclose(fp);
            io.Fonts->AddFontFromFileTTF(fontPath, 16.0f);
        } else {
            io.Fonts->AddFontDefault();
        }
    }

    // ── 4. Blockchain + demo data ─────────────────────────────────
    cw1::Blockchain chain;
    loadDemoData(chain);
    g_lastVerifySeconds = cw1::measureSeconds([&]() {
        g_lastVerify = chain.verifyIntegrity();
    });
    g_verifyDone = true;

    // ── 5. Main loop ──────────────────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full-screen root window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_MAIN);
        ImGui::Begin("##root", nullptr,
            ImGuiWindowFlags_NoDecoration          |
            ImGuiWindowFlags_NoMove                |
            ImGuiWindowFlags_NoResize              |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar           |
            ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleColor();

        RenderHeader(chain);
        ImGui::Spacing();

        // Sidebar
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##sbwrap", ImVec2(280, -1), false);
        ImGui::PopStyleColor();
        RenderSidebar(chain);
        ImGui::EndChild();

        ImGui::SameLine();

        // Content area
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_MAIN);
        ImGui::BeginChild("##content", ImVec2(-1, -1), false,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PopStyleColor();
        ImGui::Spacing(); ImGui::Spacing();

        switch (g_view) {
        case View::DASHBOARD:    RenderDashboard(chain);    break;
        case View::CAR_DETAIL:   RenderCarDetail(chain);    break;
        case View::ADD_BLOCK:    RenderAddBlock(chain);     break;
        case View::GLOBAL_CHAIN: RenderGlobalChain(chain);  break;
        case View::AUDIT_LOG:    RenderAuditLog(chain);     break;
        case View::INTEGRITY:    RenderIntegrity(chain);    break;
        }

        ImGui::EndChild();
        ImGui::End();

        // Toast overlay (rendered after main window)
        RenderToasts(io.DeltaTime);

        // ── Draw ──────────────────────────────────────────────────
        ImGui::Render();
        int dispW, dispH;
        glfwGetFramebufferSize(window, &dispW, &dispH);
        glViewport(0, 0, dispW, dispH);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);  // #0d1117
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // ── 6. Cleanup ────────────────────────────────────────────────
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

### src/models/CarRecord.cpp

Status: **Modified**

```cpp
#include "models/CarRecord.hpp"

#include <sstream>

namespace cw1 {

namespace {

/// Escape pipe and backslash so the serialised string is unambiguous.
std::string escapeField(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '\\' || ch == '|') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

} // namespace

std::string CarRecord::serialize() const {
    std::ostringstream out;

    // Common fields
    out << "vin=" << escapeField(vin)
        << "|manufacturer=" << escapeField(manufacturer)
        << "|model=" << escapeField(model)
        << "|color=" << escapeField(color)
        << "|productionYear=" << productionYear
        << "|stage=" << escapeField(stageToString(stage));

    // Stage-specific fields — include all so that the hash is deterministic
    // regardless of which fields happen to be empty.
    out << "|factoryLocation=" << escapeField(factoryLocation)
        << "|warehouseLocation=" << escapeField(warehouseLocation)
        << "|receivedBy=" << escapeField(receivedBy)
        << "|inspectorId=" << escapeField(inspectorId)
        << "|passed=" << (passed ? "true" : "false")
        << "|qcNotes=" << escapeField(qcNotes)
        << "|dealerId=" << escapeField(dealerId)
        << "|destination=" << escapeField(destination)
        << "|transportMode=" << escapeField(transportMode)
        << "|buyerId=" << escapeField(buyerId)
        << "|salePrice=" << salePrice
        << "|warrantyExpiry=" << escapeField(warrantyExpiry);

    return out.str();
}

} // namespace cw1
```

### src/utils/AuditLog.cpp

Status: **Modified**

```cpp
// AuditLog.cpp -- Implementation of the hand-written singly linked list
// used for audit logging in the Car Warehouse Blockchain system.
//
// Coursework requirement: demonstrates:
//   1. Singly Linked List  -- manual node management, head/tail pointers,
//      O(1) tail append, O(n) traversal/deletion.
//   2. Array of Pointers   -- getRecentEntries() returns a raw new[] array
//      of const AuditEntry* that the caller must delete[].

#include "utils/AuditLog.hpp"
#include "utils/TimeUtil.hpp"

#include <algorithm>  // std::min

namespace cw1 {

// ── Helper ────────────────────────────────────────────────────────

std::string actionToString(AuditAction action) {
    switch (action) {
        case AuditAction::BLOCK_ADDED:       return "BLOCK_ADDED";
        case AuditAction::INTEGRITY_CHECK:   return "INTEGRITY_CHECK";
        case AuditAction::SEARCH_PERFORMED:  return "SEARCH_PERFORMED";
        case AuditAction::CHAIN_VIEWED:      return "CHAIN_VIEWED";
        case AuditAction::TAMPER_SIMULATED:  return "TAMPER_SIMULATED";
        default:                             return "UNKNOWN";
    }
}

// ── AuditLog constructor / destructor ─────────────────────────────

AuditLog::AuditLog()
    : head_(nullptr), tail_(nullptr), count_(0) {
    // Linked list starts empty: both head and tail are null.
}

AuditLog::~AuditLog() {
    // Traverse the linked list and delete every node to avoid memory leaks.
    clear();
}

// ── Core linked list operations ───────────────────────────────────

void AuditLog::log(AuditAction action, const std::string& details) {
    // Allocate a new node on the heap (demonstrates new/delete pattern).
    AuditEntry* node = new AuditEntry(action, details, TimeUtil::nowIso8601());

    if (tail_ == nullptr) {
        // List is empty: new node becomes both head and tail.
        head_ = node;
        tail_ = node;
    } else {
        // Append at the tail -- O(1) because we maintain a tail pointer.
        // Without the tail pointer this would be O(n) traversal.
        tail_->next = node;  // Link old tail to new node
        tail_ = node;        // Advance tail to the new node
    }

    ++count_;
}

std::size_t AuditLog::size() const noexcept {
    return count_;
}

const AuditEntry* AuditLog::head() const noexcept {
    return head_;
}

void AuditLog::clear() {
    // Traverse linked list from head, deleting each node.
    // Classic linked-list deletion: save next before freeing current.
    AuditEntry* current = head_;
    while (current != nullptr) {
        AuditEntry* next = current->next;  // Save next before delete
        delete current;                    // Free the current node
        current = next;                    // Advance to saved next
    }
    head_  = nullptr;
    tail_  = nullptr;
    count_ = 0;
}

// ── Array of Pointers ─────────────────────────────────────────────

const AuditEntry** AuditLog::getRecentEntries(std::size_t maxCount,
                                              std::size_t& outCount) const {
    // Demonstrates: Array of Pointers (raw pointer array, manual memory
    // management). The returned array is a heap-allocated array whose
    // elements are pointers into the linked list (not copies). The caller
    // must delete[] the array but must NOT delete the pointed-to nodes.

    // Determine how many entries we can actually return.
    std::size_t actualCount = (maxCount < count_) ? maxCount : count_;
    outCount = actualCount;

    if (actualCount == 0) {
        return nullptr;
    }

    // Allocate the raw array of pointers on the heap.
    // This is the "Array of Pointers" data structure for the coursework.
    const AuditEntry** arr = new const AuditEntry*[actualCount];

    // Skip the first (count_ - actualCount) nodes to reach the start of
    // the last actualCount entries. This requires a linear traversal from
    // the head because we only have a singly linked list (no random access).
    std::size_t skipCount = count_ - actualCount;
    const AuditEntry* node = head_;
    for (std::size_t i = 0; i < skipCount; ++i) {
        node = node->next;  // Advance through nodes we want to skip
    }

    // Fill the array with pointers to the remaining (most recent) nodes.
    for (std::size_t i = 0; i < actualCount; ++i) {
        arr[i] = node;      // Store pointer to node (not a copy of the node)
        node = node->next;  // Advance linked list pointer
    }

    return arr;  // Caller must delete[] this array
}

RecentEntryArray::RecentEntryArray(const AuditLog& log, std::size_t maxCount)
    : data_(nullptr), count_(0) {
    data_ = log.getRecentEntries(maxCount, count_);
}

RecentEntryArray::~RecentEntryArray() {
    delete[] data_;
}

RecentEntryArray::RecentEntryArray(RecentEntryArray&& other) noexcept
    : data_(other.data_), count_(other.count_) {
    other.data_ = nullptr;
    other.count_ = 0;
}

RecentEntryArray& RecentEntryArray::operator=(RecentEntryArray&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] data_;
    data_ = other.data_;
    count_ = other.count_;
    other.data_ = nullptr;
    other.count_ = 0;
    return *this;
}

std::size_t RecentEntryArray::size() const noexcept {
    return count_;
}

bool RecentEntryArray::empty() const noexcept {
    return count_ == 0;
}

const AuditEntry* RecentEntryArray::operator[](std::size_t index) const noexcept {
    if (data_ == nullptr || index >= count_) {
        return nullptr;
    }
    return data_[index];
}

const AuditEntry* const* RecentEntryArray::begin() const noexcept {
    return data_;
}

const AuditEntry* const* RecentEntryArray::end() const noexcept {
    return data_ == nullptr ? nullptr : (data_ + count_);
}

} // namespace cw1
```

### src/utils/HashUtil.cpp

Status: **Modified**

```cpp
#include "utils/HashUtil.hpp"

#include <iomanip>
#include <openssl/sha.h>
#include <sstream>

namespace cw1::HashUtil {

std::string sha256(const std::string& input) {
    unsigned char digest[SHA256_DIGEST_LENGTH] = {0};

    SHA256(
        reinterpret_cast<const unsigned char*>(input.data()),
        input.size(),
        digest
    );

    std::ostringstream output;
    output << std::hex << std::setfill('0');

    for (unsigned char byte : digest) {
        output << std::setw(2) << static_cast<int>(byte);
    }

    return output.str();
}

} // namespace cw1::HashUtil
```

### src/utils/TimeUtil.cpp

Status: **Modified**

```cpp
#include "utils/TimeUtil.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace cw1::TimeUtil {

std::string nowIso8601() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm utcTime = {};
#ifdef _WIN32
    gmtime_s(&utcTime, &currentTime);
#else
    gmtime_r(&currentTime, &utcTime);
#endif

    std::ostringstream output;
    output << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return output.str();
}

} // namespace cw1::TimeUtil
```

### include/blockchain/BlockFormatter.hpp

Status: **Added**

```cpp
#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"

namespace cw1 {

/// Canonical block formatter used by both CLI and GUI.
/// Keeps screenshot output consistent with coursework block requirements.
inline std::string formatBlockForDisplay(const Block& block) {
    const CarRecord& record = block.getRecord();
    const bool isGenesis = (block.getIndex() == 0);
    const bool genesisRuleSatisfied =
        (block.getPreviousHash() == block.getCurrentHash());

    std::ostringstream out;
    out << "======================================================================\n";
    out << "Block #" << block.getIndex()
        << " | Stage: " << stageToString(record.stage) << '\n';
    out << "----------------------------------------------------------------------\n";
    out << "Current Hash : " << block.getCurrentHash() << '\n';
    out << "Previous Hash: " << block.getPreviousHash() << '\n';
    if (isGenesis) {
        out << "Genesis Rule : previousHash == currentHash -> "
            << (genesisRuleSatisfied ? "YES" : "NO") << '\n';
    }
    out << "Timestamp    : " << block.getTimestamp() << '\n';
    out << "Nonce        : " << block.getNonce() << '\n';
    out << "VIN          : " << record.vin << '\n';
    out << "Vehicle      : " << record.manufacturer << ' ' << record.model
        << " (" << record.color << ", " << record.productionYear << ")\n";

    out << "Payload      :\n";
    switch (record.stage) {
        case BlockStage::PRODUCTION:
            out << "  - Factory Location: " << record.factoryLocation << '\n';
            break;
        case BlockStage::WAREHOUSE_INTAKE:
            out << "  - Warehouse Location: " << record.warehouseLocation << '\n';
            out << "  - Received By       : " << record.receivedBy << '\n';
            break;
        case BlockStage::QUALITY_CHECK:
            out << "  - Inspector ID      : " << record.inspectorId << '\n';
            out << "  - QC Result         : " << (record.passed ? "PASSED" : "FAILED") << '\n';
            out << "  - QC Notes          : " << record.qcNotes << '\n';
            break;
        case BlockStage::DEALER_DISPATCH:
            out << "  - Dealer ID         : " << record.dealerId << '\n';
            out << "  - Destination       : " << record.destination << '\n';
            out << "  - Transport Mode    : " << record.transportMode << '\n';
            break;
        case BlockStage::CUSTOMER_SALE:
            out << "  - Buyer ID          : " << record.buyerId << '\n';
            out << "  - Sale Price        : MYR " << std::fixed
                << std::setprecision(2) << record.salePrice << '\n';
            out << "  - Warranty Expiry   : " << record.warrantyExpiry << '\n';
            break;
    }

    out << "======================================================================\n";
    return out.str();
}

} // namespace cw1
```

### include/utils/OperationTimer.hpp

Status: **Added**

```cpp
#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace cw1 {

/// Lightweight stopwatch for operation runtime reporting.
class OperationTimer {
public:
    using Clock = std::chrono::steady_clock;

    OperationTimer() : start_(Clock::now()) {}

    double elapsedSeconds() const {
        const auto elapsed = Clock::now() - start_;
        return std::chrono::duration<double>(elapsed).count();
    }

private:
    Clock::time_point start_;
};

template <typename Callable>
double measureSeconds(Callable&& callable) {
    OperationTimer timer;
    std::forward<Callable>(callable)();
    return timer.elapsedSeconds();
}

inline std::string formatSeconds(double seconds) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(4) << seconds;
    return out.str();
}

} // namespace cw1
```

