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

/// Verify hash integrity and chain linkage across all blocks.
ValidationResult verifyChain(const std::vector<Block>& blocks);

} // namespace Validation

} // namespace cw1
