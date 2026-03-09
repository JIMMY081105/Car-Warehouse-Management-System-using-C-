#pragma once

#include <string>

#include "blockchain/BlockStage.hpp"

namespace cw1 {




struct CarRecord {
    
    std::string vin;
    std::string manufacturer;
    std::string model;
    std::string color;
    int         productionYear = 0;
    BlockStage  stage          = BlockStage::PRODUCTION;
    std::string manufacturerId;  

    
    
    std::string factoryLocation;   

    
    std::string warehouseLocation; 
    std::string receivedBy;        
    std::string supplierId;        

    
    std::string inspectorId;       
    bool        passed = false;    
    std::string qcNotes;           

    
    std::string dealerId;          
    std::string destination;       
    std::string transportMode;     

    
    std::string buyerId;           
    std::string retailerId;        
    double      salePrice = 0.0;   
    std::string warrantyExpiry;    

    
    
    std::string serialize() const;
};

} 
