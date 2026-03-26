// Defines the structured payload stored inside each blockchain block. Fields are grouped by stage so a marker can see how one vehicle progresses from production to customer sale through multiple immutable snapshots.

#pragma once

#include <string>

#include "blockchain/BlockStage.hpp"

namespace cw1 {

// One vehicle state snapshot. Different subsets of fields are populated depending on the lifecycle stage stored in the surrounding Block.
struct CarRecord {
    std::string vin;
    std::string manufacturer;
    std::string model;
    std::string color;
    int productionYear = 0;
    BlockStage stage = BlockStage::PRODUCTION;
    std::string manufacturerId;

    // Production stage fields.
    std::string factoryLocation;

    // Warehouse intake stage fields.
    std::string warehouseLocation;
    std::string receivedBy;
    std::string supplierId;

    // Quality-check stage fields.
    std::string inspectorId;
    bool passed = false;
    std::string qcNotes;

    // Dealer dispatch stage fields.
    std::string dealerId;
    std::string destination;
    std::string transportMode;

    // Customer sale stage fields.
    std::string buyerId;
    std::string retailerId;
    double salePrice = 0.0;
    std::string warrantyExpiry;

    // Produces a stable serialised payload for hashing.
    std::string serialize() const;

    // Generates the placeholder payload used by soft delete. The original payload is stored elsewhere so the block can be restored later.
    static CarRecord tombstone(const std::string& vin);
};

}  // namespace cw1
