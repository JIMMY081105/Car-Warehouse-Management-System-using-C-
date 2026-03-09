#pragma once

#include <string>

namespace cw1 {


enum class BlockStage {
    PRODUCTION,        
    WAREHOUSE_INTAKE,  
    QUALITY_CHECK,     
    DEALER_DISPATCH,   
    CUSTOMER_SALE      
};


std::string stageToString(BlockStage stage);


BlockStage stringToStage(const std::string& text);

} 
