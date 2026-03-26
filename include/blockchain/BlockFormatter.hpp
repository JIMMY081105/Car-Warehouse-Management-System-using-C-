// Helper for producing lecturer-friendly block summaries in text form. The formatter makes the hash links, genesis rule, and stage-specific payload fields easy to inspect quickly.

#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "blockchain/Block.hpp"
#include "blockchain/BlockStage.hpp"

namespace cw1 {

// Produces a rich multi-line description of one block for display or debugging.
inline std::string formatBlockForDisplay(const Block& block) {
    const CarRecord& record = block.getRecord();
    const bool isGenesis = (block.getIndex() == 0);
    const bool genesisRuleSatisfied =
        (block.getPreviousHash() == block.getCurrentHash());

    std::ostringstream out;
    out << "======================================================================\n";
    out << "Block #" << block.getIndex()
        << " | Stage: " << stageToString(record.stage) << '\n';
    out << "----------------------------------------------------------------------\n";
    out << "Current Hash : " << block.getCurrentHash() << '\n';
    out << "SHA3-128 Hash: " << block.getSha3Hash() << '\n';
    out << "Previous Hash: " << block.getPreviousHash() << '\n';
    if (isGenesis) {
        out << "Genesis Rule : previousHash == currentHash -> "
            << (genesisRuleSatisfied ? "YES" : "NO") << '\n';
    }
    out << "Timestamp    : " << block.getTimestamp() << '\n';
    out << "Nonce        : " << block.getNonce() << '\n';
    out << "VIN          : " << record.vin << '\n';
    out << "Mfr. ID      : " << record.manufacturerId << '\n';
    out << "Vehicle      : " << record.manufacturer << ' ' << record.model
        << " (" << record.color << ", " << record.productionYear << ")\n";

    out << "Payload      :\n";
    switch (record.stage) {
        case BlockStage::PRODUCTION:
            out << "  - Factory Location: " << record.factoryLocation << '\n';
            break;
        case BlockStage::WAREHOUSE_INTAKE:
            out << "  - Warehouse Location: " << record.warehouseLocation << '\n';
            out << "  - Received By       : " << record.receivedBy << '\n';
            out << "  - Supplier ID       : " << record.supplierId << '\n';
            break;
        case BlockStage::QUALITY_CHECK:
            out << "  - Inspector ID      : " << record.inspectorId << '\n';
            out << "  - QC Result         : " << (record.passed ? "PASSED" : "FAILED") << '\n';
            out << "  - QC Notes          : " << record.qcNotes << '\n';
            break;
        case BlockStage::DEALER_DISPATCH:
            out << "  - Dealer ID         : " << record.dealerId << '\n';
            out << "  - Destination       : " << record.destination << '\n';
            out << "  - Transport Mode    : " << record.transportMode << '\n';
            break;
        case BlockStage::CUSTOMER_SALE:
            out << "  - Buyer ID          : " << record.buyerId << '\n';
            out << "  - Retailer ID       : " << record.retailerId << '\n';
            out << "  - Sale Price        : MYR " << std::fixed
                << std::setprecision(2) << record.salePrice << '\n';
            out << "  - Warranty Expiry   : " << record.warrantyExpiry << '\n';
            break;
    }

    out << "======================================================================\n";
    return out.str();
}

}
