








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
    return AuditAction::PERSISTENCE_IO; // fallback for unknown strings
}



AuditLog::AuditLog()
    : head_(nullptr), tail_(nullptr), count_(0) {
    
}

AuditLog::~AuditLog() {
    
    clear();
}



void AuditLog::log(AuditAction action, const std::string& details) {
    
    AuditEntry* node = new AuditEntry(action, details, TimeUtil::nowIso8601());

    if (tail_ == nullptr) {
        
        head_ = node;
        tail_ = node;
    } else {
        
        
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



const AuditEntry** AuditLog::getRecentEntries(std::size_t maxCount,
                                              std::size_t& outCount) const {
    
    
    
    

    
    std::size_t actualCount = (maxCount < count_) ? maxCount : count_;
    outCount = actualCount;

    if (actualCount == 0) {
        return nullptr;
    }

    
    
    const AuditEntry** arr = new const AuditEntry*[actualCount];

    
    
    
    std::size_t skipCount = count_ - actualCount;
    const AuditEntry* node = head_;
    for (std::size_t i = 0; i < skipCount; ++i) {
        node = node->next;  
    }

    
    for (std::size_t i = 0; i < actualCount; ++i) {
        arr[i] = node;      
        node = node->next;  
    }

    return arr;  
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

} 
