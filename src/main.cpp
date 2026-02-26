#include <iostream>
#include <string>

#include "blockchain/Blockchain.hpp"

namespace {

int parseYearOrDefault(const std::string& value, int fallback) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

cw1::CarRecord inputRecordFromUser() {
    cw1::CarRecord record;

    std::string yearText;

    std::cout << "VIN: ";
    std::getline(std::cin, record.vin);

    std::cout << "Manufacturer: ";
    std::getline(std::cin, record.manufacturer);

    std::cout << "Model: ";
    std::getline(std::cin, record.model);

    std::cout << "Color: ";
    std::getline(std::cin, record.color);

    std::cout << "Production Year: ";
    std::getline(std::cin, yearText);
    record.productionYear = parseYearOrDefault(yearText, 0);

    std::cout << "Warehouse Location: ";
    std::getline(std::cin, record.warehouseLocation);

    std::cout << "Status: ";
    std::getline(std::cin, record.status);

    return record;
}

void printBlockSummary(const cw1::Block& block) {
    const cw1::CarRecord& record = block.getRecord();

    std::cout << "----------------------------------------\n";
    std::cout << "Index       : " << block.getIndex() << '\n';
    std::cout << "Timestamp   : " << block.getTimestamp() << '\n';
    std::cout << "Nonce       : " << block.getNonce() << '\n';
    std::cout << "PreviousHash: " << block.getPreviousHash() << '\n';
    std::cout << "CurrentHash : " << block.getCurrentHash() << '\n';
    std::cout << "Record      : "
              << "VIN=" << record.vin
              << ", Manufacturer=" << record.manufacturer
              << ", Model=" << record.model
              << ", Color=" << record.color
              << ", Year=" << record.productionYear
              << ", Warehouse=" << record.warehouseLocation
              << ", Status=" << record.status << '\n';
}

void printAllBlocks(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getBlocks();
    if (blocks.empty()) {
        std::cout << "No blocks in chain.\n";
        return;
    }

    for (const auto& block : blocks) {
        printBlockSummary(block);
    }

    std::cout << "----------------------------------------\n";
}

void runStartupSelfCheck(cw1::Blockchain& chain) {
    std::cout << "=== Self-check: create 3 blocks ===\n";

    chain.addBlock({"VIN1001", "Perodua", "Axia", "Silver", 2024, "WH-A1", "In Stock"});
    chain.addBlock({"VIN1002", "Toyota", "Vios", "White", 2023, "WH-B2", "Quality Checked"});
    chain.addBlock({"VIN1003", "Honda", "City", "Blue", 2022, "WH-C3", "Ready for Delivery"});

    printAllBlocks(chain);

    const cw1::ValidationResult result = chain.verifyIntegrity();
    std::cout << "Integrity: " << (result.ok ? "PASS" : "FAIL")
              << " - " << result.message << "\n\n";
}

} // namespace

int main() {
    cw1::Blockchain chain;
    runStartupSelfCheck(chain);

    while (true) {
        std::cout << "Car Warehouse Blockchain CLI\n";
        std::cout << "1. Add block\n";
        std::cout << "2. Print blocks\n";
        std::cout << "3. Verify integrity\n";
        std::cout << "4. Exit\n";
        std::cout << "Select: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            cw1::CarRecord record = inputRecordFromUser();
            chain.addBlock(record);
            std::cout << "Block added.\n\n";
            continue;
        }

        if (choice == "2") {
            printAllBlocks(chain);
            std::cout << '\n';
            continue;
        }

        if (choice == "3") {
            const cw1::ValidationResult result = chain.verifyIntegrity();
            std::cout << "Integrity: " << (result.ok ? "PASS" : "FAIL")
                      << " - " << result.message << "\n\n";
            continue;
        }

        if (choice == "4") {
            std::cout << "Exiting.\n";
            break;
        }

        std::cout << "Invalid choice. Try again.\n\n";
    }

    return 0;
}

