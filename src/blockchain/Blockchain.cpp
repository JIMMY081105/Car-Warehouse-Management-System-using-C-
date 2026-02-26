#include "blockchain/Blockchain.hpp"

namespace cw1 {

void Blockchain::addBlock(const CarRecord& record) {
    const std::string previousHash = blocks_.empty() ? "0" : blocks_.back().getCurrentHash();
    blocks_.emplace_back(blocks_.size(), previousHash, record);
}

const std::vector<Block>& Blockchain::getBlocks() const noexcept {
    return blocks_;
}

ValidationResult Blockchain::verifyIntegrity() const {
    return Validation::verifyChain(blocks_);
}

} // namespace cw1
