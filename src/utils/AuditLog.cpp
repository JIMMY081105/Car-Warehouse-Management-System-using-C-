

// Implements the linked-list audit trail. This module is a direct example of manual dynamic memory management with safe append and cleanup behaviour.

#include "utils/AuditLog.hpp"
#include "utils/TimeUtil.hpp"

#include <algorithm>

namespace cw1 {

std::string actionToString(AuditAction action) {
    switch (action) {
        case AuditAction::BLOCK_ADDED:       return "BLOCK_ADDED";
        case AuditAction::INTEGRITY_CHECK:   return "INTEGRITY_CHECK";
        case AuditAction::SEARCH_PERFORMED:  return "SEARCH_PERFORMED";
        case AuditAction::CHAIN_VIEWED:      return "CHAIN_VIEWED";
        case AuditAction::TAMPER_SIMULATED:  return "TAMPER_SIMULATED";
        case AuditAction::PERSISTENCE_IO:    return "PERSISTENCE_IO";
        case AuditAction::BLOCK_DELETED:     return "BLOCK_DELETED";
        case AuditAction::USER_LOGIN:        return "USER_LOGIN";
        case AuditAction::USER_LOGOUT:       return "USER_LOGOUT";
        case AuditAction::APPROVAL_ACTION:   return "APPROVAL_ACTION";
        case AuditAction::ACCESS_DENIED:     return "ACCESS_DENIED";
        default:                             return "UNKNOWN";
    }
}

AuditAction stringToAction(const std::string& str) {
    if (str == "BLOCK_ADDED")       return AuditAction::BLOCK_ADDED;
    if (str == "INTEGRITY_CHECK")   return AuditAction::INTEGRITY_CHECK;
    if (str == "SEARCH_PERFORMED")  return AuditAction::SEARCH_PERFORMED;
    if (str == "CHAIN_VIEWED")      return AuditAction::CHAIN_VIEWED;
    if (str == "TAMPER_SIMULATED")  return AuditAction::TAMPER_SIMULATED;
    if (str == "PERSISTENCE_IO")    return AuditAction::PERSISTENCE_IO;
    if (str == "BLOCK_DELETED")     return AuditAction::BLOCK_DELETED;
    if (str == "USER_LOGIN")        return AuditAction::USER_LOGIN;
    if (str == "USER_LOGOUT")       return AuditAction::USER_LOGOUT;
    if (str == "APPROVAL_ACTION")   return AuditAction::APPROVAL_ACTION;
    if (str == "ACCESS_DENIED")     return AuditAction::ACCESS_DENIED;
    return AuditAction::PERSISTENCE_IO;
}

AuditLog::AuditLog()
    : head_(nullptr), tail_(nullptr), count_(0) {}

AuditLog::~AuditLog() {
    clear();
}

void AuditLog::log(AuditAction action, const std::string& details) {
    // Every appended node is allocated once and becomes owned by the log.
    AuditEntry* node = new AuditEntry(action, details, TimeUtil::nowIso8601());

    if (tail_ == nullptr) {
        head_ = node;
        tail_ = node;
    } else {
        // Pointer chaining is explicit here: the current tail points at the new node, then the tail pointer advances.
        tail_->next = node;
        tail_ = node;
    }

    ++count_;
}

void AuditLog::log(AuditAction action, const std::string& details, const std::string& timestamp) {
    AuditEntry* node = new AuditEntry(action, details, timestamp);

    if (tail_ == nullptr) {
        head_ = node;
        tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
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
    // Walk node-by-node so every dynamic allocation is released exactly once.
    AuditEntry* current = head_;
    while (current != nullptr) {
        AuditEntry* next = current->next;
        delete current;
        current = next;
    }
    head_  = nullptr;
    tail_  = nullptr;
    count_ = 0;
}

std::vector<const AuditEntry*> AuditLog::getRecentEntries(std::size_t maxCount) const {
    std::size_t actualCount = std::min(maxCount, count_);
    if (actualCount == 0) {
        return {};
    }

    std::vector<const AuditEntry*> result(actualCount);

    std::size_t skipCount = count_ - actualCount;
    const AuditEntry* node = head_;
    for (std::size_t i = 0; i < skipCount; ++i) {
        node = node->next;
    }

    for (std::size_t i = 0; i < actualCount; ++i) {
        result[i] = node;
        node = node->next;
    }

    return result;
}  // namespace cw1

RecentEntryArray::RecentEntryArray(const AuditLog& log, std::size_t maxCount)
    : data_(log.getRecentEntries(maxCount)) {}

std::size_t RecentEntryArray::size() const noexcept {
    return data_.size();
}

bool RecentEntryArray::empty() const noexcept {
    return data_.empty();
}

const AuditEntry* RecentEntryArray::operator[](std::size_t index) const noexcept {
    if (index >= data_.size()) {
        return nullptr;
    }
    return data_[index];
}

const AuditEntry* const* RecentEntryArray::begin() const noexcept {
    return data_.empty() ? nullptr : data_.data();
}

const AuditEntry* const* RecentEntryArray::end() const noexcept {
    return data_.empty() ? nullptr : data_.data() + data_.size();
}

}
