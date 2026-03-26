#include "models/CarRecord.hpp"

#include <sstream>

namespace cw1 {

namespace {


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

} 

std::string CarRecord::serialize() const {
    std::ostringstream out;

    out << "v1"
        << "|vin=" << escapeField(vin)
        << "|manufacturer=" << escapeField(manufacturer)
        << "|model=" << escapeField(model)
        << "|color=" << escapeField(color)
        << "|productionYear=" << productionYear
        << "|stage=" << escapeField(stageToString(stage))
        << "|manufacturerId=" << escapeField(manufacturerId);

    
    
    out << "|factoryLocation=" << escapeField(factoryLocation)
        << "|warehouseLocation=" << escapeField(warehouseLocation)
        << "|receivedBy=" << escapeField(receivedBy)
        << "|supplierId=" << escapeField(supplierId)
        << "|inspectorId=" << escapeField(inspectorId)
        << "|passed=" << (passed ? "true" : "false")
        << "|qcNotes=" << escapeField(qcNotes)
        << "|dealerId=" << escapeField(dealerId)
        << "|destination=" << escapeField(destination)
        << "|transportMode=" << escapeField(transportMode)
        << "|buyerId=" << escapeField(buyerId)
        << "|retailerId=" << escapeField(retailerId)
        << "|salePrice=" << salePrice
        << "|warrantyExpiry=" << escapeField(warrantyExpiry);

    return out.str();
}

CarRecord CarRecord::tombstone(const std::string& vin) {
    CarRecord rec;
    rec.vin          = vin;
    rec.manufacturer = "[DELETED]";
    rec.model        = "[DELETED]";
    return rec;
}

}
