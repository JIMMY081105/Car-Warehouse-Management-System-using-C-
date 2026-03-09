#include "blockchain/BlockStage.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace cw1 {

std::string stageToString(BlockStage stage) {
    switch (stage) {
        case BlockStage::PRODUCTION:       return "Production";
        case BlockStage::WAREHOUSE_INTAKE: return "Warehouse Intake";
        case BlockStage::QUALITY_CHECK:    return "Quality Check";
        case BlockStage::DEALER_DISPATCH:  return "Dealer Dispatch";
        case BlockStage::CUSTOMER_SALE:    return "Customer Sale";
    }
    return "Unknown";
}

BlockStage stringToStage(const std::string& text) {
    
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lower == "production")       return BlockStage::PRODUCTION;
    if (lower == "warehouse intake") return BlockStage::WAREHOUSE_INTAKE;
    if (lower == "quality check")    return BlockStage::QUALITY_CHECK;
    if (lower == "dealer dispatch")  return BlockStage::DEALER_DISPATCH;
    if (lower == "customer sale")    return BlockStage::CUSTOMER_SALE;

    throw std::invalid_argument("Unknown stage: " + text);
}

} 
