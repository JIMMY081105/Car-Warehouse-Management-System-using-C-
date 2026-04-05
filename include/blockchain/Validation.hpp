// Validation result types and chain-checking entry point.

#pragma once

#include <string>
#include <vector>

#include "blockchain/Block.hpp"

namespace cw1 {

// Returned by integrity checks in both the production chain and the temporary immutability demo. failedIndices makes it easy for the GUI to highlight the exact blocks that broke blockchain rules.
struct ValidationResult {
    bool ok;
    std::string message;
    std::vector<std::size_t> failedIndices;
};

namespace Validation {

// Verifies every block by recalculating hashes and checking the previous-hash linkage. The first block also follows the coursework-specific genesis rule.
ValidationResult verifyChain(const std::vector<Block>& blocks);

}  // namespace Validation

}  // namespace cw1
