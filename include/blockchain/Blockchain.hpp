#pragma once

#include <vector>

#include "blockchain/Block.hpp"
#include "blockchain/Validation.hpp"
#include "models/CarRecord.hpp"

namespace cw1 {

class Blockchain {
public:
    void addBlock(const CarRecord& record);
    const std::vector<Block>& getBlocks() const noexcept;
    ValidationResult verifyIntegrity() const;

private:
    std::vector<Block> blocks_;
};

} // namespace cw1
