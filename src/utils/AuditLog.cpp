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

using namespace std;

namespace cw1 {

// ── Helper ────────────────────────────────────────────────────────

string actionToString(AuditAction action) {
    switch (action) {
        case AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        default:                            return "UNKNOWN";
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

void AuditLog::log(AuditAction action, const string& details) {
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

size_t AuditLog::size() const noexcept {
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

const AuditEntry** AuditLog::getRecentEntries(size_t maxCount,
                                               size_t& outCount) const {
    // Demonstrates: Array of Pointers (raw pointer array, manual memory
    // management). The returned array is a heap-allocated array whose
    // elements are pointers into the linked list (not copies). The caller
    // must delete[] the array but must NOT delete the pointed-to nodes.

    // Determine how many entries we can actually return.
    size_t actualCount = (maxCount < count_) ? maxCount : count_;
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
    size_t skipCount = count_ - actualCount;
    const AuditEntry* node = head_;
    for (size_t i = 0; i < skipCount; ++i) {
        node = node->next;  // Advance through nodes we want to skip
    }

    // Fill the array with pointers to the remaining (most recent) nodes.
    for (size_t i = 0; i < actualCount; ++i) {
        arr[i] = node;      // Store pointer to node (not a copy of the node)
        node = node->next;  // Advance linked list pointer
    }

    return arr;  // Caller must delete[] this array
}

} // namespace cw1
