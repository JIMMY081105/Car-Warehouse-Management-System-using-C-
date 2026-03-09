#pragma once

#include <string>
#include <vector>

#include "blockchain/Block.hpp"

namespace cw1 {

struct ValidationResult {
    bool        ok;
    std::string message;
};

namespace Validation {


ValidationResult verifyChain(const std::vector<Block>& blocks);

} 

} 
