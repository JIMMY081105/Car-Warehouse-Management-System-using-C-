cmake_minimum_required(VERSION 3.16)

project(COMP2034_CW1_CarWarehouseBlockchain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenSSL REQUIRED)

add_executable(car_warehouse_blockchain
    src/main.cpp
    src/models/CarRecord.cpp
    src/blockchain/Block.cpp
    src/blockchain/Blockchain.cpp
    src/blockchain/Validation.cpp
    src/utils/HashUtil.cpp
    src/utils/TimeUtil.cpp
)

target_include_directories(car_warehouse_blockchain
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(car_warehouse_blockchain
    PRIVATE
        OpenSSL::Crypto
)

if(MSVC)
    target_compile_options(car_warehouse_blockchain PRIVATE /W4 /permissive-)
else()
    target_compile_options(car_warehouse_blockchain PRIVATE -Wall -Wextra -Wpedantic)
endif()
