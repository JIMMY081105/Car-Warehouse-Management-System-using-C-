// Chain validation rules used by both the real chain and the temp demo.

#include "blockchain/Validation.hpp"

#include <sstream>

namespace cw1::Validation {

ValidationResult verifyChain(const std::vector<Block>& blocks) {
    if (blocks.empty()) {
        return {true, "Blockchain is empty and therefore valid.", {}};
    }

    std::vector<std::size_t> failedIndices;

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const Block& current = blocks[i];

        // Recompute the payload hash to detect direct edits to stored data.
        if (current.getCurrentHash() != current.computeHash()) {///
            failedIndices.push_back(i);
            continue;
        }

        // The coursework stores a second digest so the marker can see another integrity signal besides the main SHA-256 chain hash.
        if (current.getSha3Hash() != current.computeSha3Hash()) {
            failedIndices.push_back(i);
            continue;
        }

        if (i == 0) {
            // Custom genesis rule: the first block keeps its own hash in the previousHash field after construction/rehash.
            if (current.getPreviousHash() != current.getCurrentHash()) {
                failedIndices.push_back(i);
            }
            continue;
        }

        // Every later block must point at the exact hash stored by the block immediately before it, otherwise the chain has been broken.
        const Block& previous = blocks[i - 1];
        if (current.getPreviousHash() != previous.getCurrentHash()) {
            failedIndices.push_back(i);
        }
    }

    if (failedIndices.empty()) {
        return {true,
                "Blockchain integrity verified. " + std::to_string(blocks.size()) + " block(s) OK.",
                {}};
    }

    std::ostringstream msg;
    msg << "Integrity failure at block(s): ";
    for (std::size_t j = 0; j < failedIndices.size(); ++j) {
        if (j > 0) msg << ", ";
        msg << "#" << failedIndices[j];
    }
    return {false, msg.str(), failedIndices};
}  // namespace cw1::Validation

}
