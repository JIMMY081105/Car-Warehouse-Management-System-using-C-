// Lifecycle stages used in the vehicle workflow.

#pragma once

#include <string>

namespace cw1 {

// Stage order matters because each block snapshot records one point in the warehouse journey from production to customer sale.
enum class BlockStage {
    PRODUCTION,
    WAREHOUSE_INTAKE,
    QUALITY_CHECK,
    DEALER_DISPATCH,
    CUSTOMER_SALE
};
// Converts enum values into lecturer-friendly UI text.
std::string stageToString(BlockStage stage);

// Parses saved text back into the corresponding stage value.
BlockStage stringToStage(const std::string& text);

}
