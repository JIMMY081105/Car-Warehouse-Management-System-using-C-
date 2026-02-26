#pragma once

#include <string>

namespace cw1 {

struct CarRecord {
    std::string vin;
    std::string manufacturer;
    std::string model;
    std::string color;
    int productionYear = 0;
    std::string warehouseLocation;
    std::string status;

    std::string serialize() const;
};

} // namespace cw1
