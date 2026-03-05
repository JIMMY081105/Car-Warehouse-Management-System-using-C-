#pragma once

// AuditLog.hpp -- Hand-written singly linked list for audit logging.
// Coursework requirement: demonstrates Linked List and Array of Pointers
// as data structures beyond the vector/map used elsewhere.

#include <cstddef>
#include <string>

namespace cw1 {

// -- Audit action types ------------------------------------------------

enum class AuditAction {
    BLOCK_ADDED,        // A new block was appended to the blockchain
    INTEGRITY_CHECK,    // verifyIntegrity() was called
    SEARCH_PERFORMED,   // A search function was called
    CHAIN_VIEWED        // The chain or a car record was displayed
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

} // namespace cw1
