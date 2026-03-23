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

        // SHA-256 hash recompute check
        if (current.getCurrentHash() != current.computeHash()) {
            failedIndices.push_back(i);
            continue;
        }

        // SHA3-128 hash recompute check
        if (current.getSha3Hash() != current.computeSha3Hash()) {
            failedIndices.push_back(i);
            continue;
        }

        if (i == 0) {
            // Genesis rule: previousHash must equal currentHash
            if (current.getPreviousHash() != current.getCurrentHash()) {
                failedIndices.push_back(i);
            }
            continue;
        }

        // Chain link check
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
}

}
