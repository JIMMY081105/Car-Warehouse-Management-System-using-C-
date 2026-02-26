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

} // namespace

std::string CarRecord::serialize() const {
    std::ostringstream out;
    out << "vin=" << escapeField(vin)
        << "|manufacturer=" << escapeField(manufacturer)
        << "|model=" << escapeField(model)
        << "|color=" << escapeField(color)
        << "|productionYear=" << productionYear
        << "|warehouseLocation=" << escapeField(warehouseLocation)
        << "|status=" << escapeField(status);

    return out.str();
}

} // namespace cw1
