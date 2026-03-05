#pragma once

#include <string>

#include "blockchain/BlockStage.hpp"

namespace cw1 {

/// Represents a single event/record attached to a block.
/// Common fields are always present; stage-specific fields are filled
/// depending on which lifecycle stage the block represents.
struct CarRecord {
    // ── Common fields (every block has these) ──────────────────────
    std::string vin;
    std::string manufacturer;
    std::string model;
    std::string color;
    int         productionYear = 0;
    BlockStage  stage          = BlockStage::PRODUCTION;

    // ── Stage-specific fields (filled as needed) ───────────────────
    // PRODUCTION
    std::string factoryLocation;   // e.g. "Shah Alam Plant"

    // WAREHOUSE_INTAKE
    std::string warehouseLocation; // e.g. "WH-A1"
    std::string receivedBy;        // staff name / ID

    // QUALITY_CHECK
    std::string inspectorId;       // e.g. "QC-007"
    bool        passed = false;    // QC result
    std::string qcNotes;           // remarks

    // DEALER_DISPATCH
    std::string dealerId;          // e.g. "DLR-KL-01"
    std::string destination;       // city / address
    std::string transportMode;     // "Truck", "Rail", etc.

    // CUSTOMER_SALE
    std::string buyerId;           // e.g. "CUST-88201"
    double      salePrice = 0.0;   // MYR
    std::string warrantyExpiry;    // ISO date string

    // ── Serialisation ──────────────────────────────────────────────
    /// Deterministic pipe-delimited string used as hash input.
    std::string serialize() const;
};

} // namespace cw1
