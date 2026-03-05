#pragma once

#include <string>

namespace cw1 {

/// Lifecycle stages a car passes through in the warehouse system.
enum class BlockStage {
    PRODUCTION,        // Factory manufacture
    WAREHOUSE_INTAKE,  // Arrival at warehouse
    QUALITY_CHECK,     // Inspection / QC
    DEALER_DISPATCH,   // Shipped to dealer
    CUSTOMER_SALE      // Sold to end customer
};

/// Convert a BlockStage value to a human-readable string.
std::string stageToString(BlockStage stage);

/// Convert a string back to BlockStage (case-insensitive). Throws on bad input.
BlockStage stringToStage(const std::string& text);

} // namespace cw1
