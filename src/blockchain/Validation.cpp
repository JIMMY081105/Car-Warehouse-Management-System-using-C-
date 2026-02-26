#include "blockchain/Validation.hpp"


namespace cw1::Validation {

ValidationResult verifyChain(const std::vector<Block>& blocks) {
    if (blocks.empty()) {
        return {true, "Blockchain is empty and therefore valid."};
    }

    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const Block& current = blocks[i];

        if (current.getCurrentHash() != current.computeHash()) {
            return {false, "Hash mismatch at block index " + std::to_string(i)};
        }

        if (i == 0) {
            if (current.getPreviousHash() != "0") {
                return {false, "Genesis block previousHash must be 0."};
            }
            continue;
        }

        const Block& previous = blocks[i - 1];
        if (current.getPreviousHash() != previous.getCurrentHash()) {
            return {false, "Chain link mismatch at block index " + std::to_string(i)};
        }
    }

    return {true, "Blockchain integrity verified."};
}

} // namespace cw1::Validation

