// Declares the linked-list audit trail used to capture user actions, database activity, integrity checks, and tamper simulations. This gives the coursework a second visible pointer-based structure besides the blockchain mirror.

#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace cw1 {

enum class AuditAction {
    BLOCK_ADDED,
    INTEGRITY_CHECK,
    SEARCH_PERFORMED,
    CHAIN_VIEWED,
    TAMPER_SIMULATED,
    PERSISTENCE_IO,
    BLOCK_DELETED,
    USER_LOGIN,        // Tracks successful and failed authentication attempts
    USER_LOGOUT,       // Records when a user ends their session
    APPROVAL_ACTION,   // Logs pending request submissions, approvals, and rejections
    ACCESS_DENIED      // Captures unauthorised action attempts for security auditing
};

std::string actionToString(AuditAction action);
AuditAction stringToAction(const std::string& str);

// Linked node stored inside the audit trail. next is an explicit raw pointer because the coursework covers dynamic memory, linked lists, and traversal.
struct AuditEntry {
    AuditAction action;
    std::string details;
    std::string timestamp;
    AuditEntry* next;

    AuditEntry(AuditAction act, std::string det, std::string ts)
        : action(act), details(std::move(det)), timestamp(std::move(ts)), next(nullptr) {}
};

// Owns the linked audit list. New entries are appended at tail_ and destroyed manually in clear()/~AuditLog() to demonstrate safe dynamic memory handling.
class AuditLog {
public:
    AuditLog();
    ~AuditLog();

    AuditLog(const AuditLog&) = delete;
    AuditLog& operator=(const AuditLog&) = delete;

    // Appends a timestamped event to the end of the linked list.
    void log(AuditAction action, const std::string& details);
    void log(AuditAction action, const std::string& details, const std::string& timestamp);

    std::size_t size() const noexcept;
    const AuditEntry* head() const noexcept;

    // Copies the last N node addresses into a vector so the GUI can render recent audit rows without changing the ownership model.
    std::vector<const AuditEntry*> getRecentEntries(std::size_t maxCount) const;

    // Deletes every node and resets head_/tail_ to avoid leaks or dangling pointers when the audit log is destroyed or reinitialised.
    void clear();

private:
    AuditEntry* head_;
    AuditEntry* tail_;
    std::size_t count_;
};

// Small adapter used by the GUI table code. It turns the linked audit log into an indexable array view without changing the underlying list structure.
class RecentEntryArray {
public:
    RecentEntryArray(const AuditLog& log, std::size_t maxCount);

    std::size_t size() const noexcept;
    bool empty() const noexcept;

    const AuditEntry* operator[](std::size_t index) const noexcept;
    const AuditEntry* const* begin() const noexcept;
    const AuditEntry* const* end() const noexcept;

private:
    std::vector<const AuditEntry*> data_;
};

}  // namespace cw1
