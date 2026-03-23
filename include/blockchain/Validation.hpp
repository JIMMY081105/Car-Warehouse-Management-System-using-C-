#pragma once

#include <string>
#include <vector>

#include "blockchain/Block.hpp"

namespace cw1 {

struct ValidationResult {
    bool        ok;
    std::string message;
    std::vector<std::size_t> failedIndices;
};

namespace Validation {


ValidationResult verifyChain(const std::vector<Block>& blocks);

} 

} 
