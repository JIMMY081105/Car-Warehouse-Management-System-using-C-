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
    BLOCK_DELETED       
};


std::string actionToString(AuditAction action);
AuditAction stringToAction(const std::string& str);






struct AuditEntry {
    AuditAction action;
    std::string details;    
    std::string timestamp;  
    AuditEntry* next;       

    AuditEntry(AuditAction act, std::string det, std::string ts)
        : action(act), details(std::move(det)), timestamp(std::move(ts)), next(nullptr) {}
};












class AuditLog {
public:
    AuditLog();
    ~AuditLog();

    
    AuditLog(const AuditLog&)            = delete;
    AuditLog& operator=(const AuditLog&) = delete;

    
    
    void log(AuditAction action, const std::string& details);
    void log(AuditAction action, const std::string& details, const std::string& timestamp);

    
    std::size_t size() const noexcept;

    
    const AuditEntry* head() const noexcept;

    
    
    
    
    
    
    
    
    
    
    std::vector<const AuditEntry*> getRecentEntries(std::size_t maxCount) const;

    
    void clear();

private:
    AuditEntry* head_;   
    AuditEntry* tail_;   
    std::size_t count_;  
};






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

} 
