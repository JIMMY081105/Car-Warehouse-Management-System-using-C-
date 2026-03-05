#include "models/CarRecord.hpp"

#include <sstream>

namespace cw1 {

namespace {

/// Escape pipe and backslash so the serialised string is unambiguous.
std::string escapeField(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        if (ch == '\\' || ch == '|') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

} // namespace

std::string CarRecord::serialize() const {
    std::ostringstream out;

    // Common fields
    out << "vin=" << escapeField(vin)
        << "|manufacturer=" << escapeField(manufacturer)
        << "|model=" << escapeField(model)
        << "|color=" << escapeField(color)
        << "|productionYear=" << productionYear
        << "|stage=" << escapeField(stageToString(stage));

    // Stage-specific fields — include all so that the hash is deterministic
    // regardless of which fields happen to be empty.
    out << "|factoryLocation=" << escapeField(factoryLocation)
        << "|warehouseLocation=" << escapeField(warehouseLocation)
        << "|receivedBy=" << escapeField(receivedBy)
        << "|inspectorId=" << escapeField(inspectorId)
        << "|passed=" << (passed ? "true" : "false")
        << "|qcNotes=" << escapeField(qcNotes)
        << "|dealerId=" << escapeField(dealerId)
        << "|destination=" << escapeField(destination)
        << "|transportMode=" << escapeField(transportMode)
        << "|buyerId=" << escapeField(buyerId)
        << "|salePrice=" << salePrice
        << "|warrantyExpiry=" << escapeField(warrantyExpiry);

    return out.str();
}

} // namespace cw1
