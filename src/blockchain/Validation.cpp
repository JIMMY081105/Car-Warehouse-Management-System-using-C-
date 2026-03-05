#include "blockchain/Validation.hpp"

namespace cw1::Validation {

ValidationResult verifyChain(const std::vector<Block>& blocks) {
    if (blocks.empty()) {
        return {true, "Blockchain is empty and therefore valid."};
    }

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const Block& current = blocks[i];

        // Check that the stored hash matches a fresh computation.
        if (current.getCurrentHash() != current.computeHash()) {
            return {false, "Hash mismatch at block index " + std::to_string(i)};
        }

        if (i == 0) {
            // Coursework spec: the genesis block's previousHash must equal
            // its own currentHash (set in the Block constructor after the
            // hash is computed with "0").
            if (current.getPreviousHash() != current.getCurrentHash()) {
                return {false, "Genesis block previousHash must equal its currentHash."};
            }
            continue;
        }

        // Every subsequent block must reference the previous block's hash.
        const Block& previous = blocks[i - 1];
        if (current.getPreviousHash() != previous.getCurrentHash()) {
            return {false, "Chain link mismatch at block index " + std::to_string(i)};
        }
    }

    return {true, "Blockchain integrity verified. " + std::to_string(blocks.size()) + " block(s) OK."};
}

} // namespace cw1::Validation
