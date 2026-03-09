#pragma once








#include <cstddef>
#include <string>

namespace cw1 {




class BlockBase {
public:
    
    
    
    virtual ~BlockBase() = default;

    

    
    virtual std::string computeHash() const = 0;

    
    virtual std::string toString() const = 0;

    
    virtual std::size_t getIndex() const noexcept = 0;

    
    virtual const std::string& getCurrentHash() const noexcept = 0;

    
    virtual const std::string& getPreviousHash() const noexcept = 0;

    
    virtual const std::string& getTimestamp() const noexcept = 0;
};

} 
