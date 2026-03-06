#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "blockchain/BlockFormatter.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "utils/OperationTimer.hpp"

namespace {

using std::cin;
using std::cout;
using std::fixed;
using std::getline;
using std::left;
using std::setprecision;
using std::setw;
using std::string;
using std::to_string;
using std::vector;

int parseIntOrDefault(const string& value, int fallback) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

double parseDoubleOrDefault(const string& value, double fallback) {
    try {
        return std::stod(value);
    } catch (...) {
        return fallback;
    }
}

string prompt(const string& label) {
    string value;
    cout << "  " << label << ": ";
    getline(cin, value);
    return value;
}

void printOperationDuration(double seconds) {
    cout << "  Operation took: " << cw1::formatSeconds(seconds) << " s\n";
}

void printSeparator() {
    cout << "  " << string(60, '-') << '\n';
}

void printBlockDetail(const cw1::Block& block) {
    cout << cw1::formatBlockForDisplay(block);
}

void loadDemoData(cw1::Blockchain& chain) {
    cout << "\n  Loading demo fleet...\n";

    {
        cw1::CarRecord r;
        r.vin = "VIN1001"; r.manufacturer = "Perodua"; r.model = "Axia";
        r.color = "Silver"; r.productionYear = 2024;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Shah Alam Plant";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-A1"; r.receivedBy = "Ahmad bin Ismail";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-001"; r.passed = true; r.qcNotes = "All systems nominal";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KB-01"; r.destination = "Kota Bharu"; r.transportMode = "Truck";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::CUSTOMER_SALE;
        r.buyerId = "CUST-10201"; r.salePrice = 38000.00; r.warrantyExpiry = "2029-03-01";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1002"; r.manufacturer = "Toyota"; r.model = "Vios";
        r.color = "White"; r.productionYear = 2023;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Toyota Bukit Raja";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-B2"; r.receivedBy = "Lim Wei Jie";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-003"; r.passed = true; r.qcNotes = "Minor paint scratch - touched up";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05"; r.destination = "Kuala Lumpur"; r.transportMode = "Truck";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1003"; r.manufacturer = "Honda"; r.model = "City";
        r.color = "Blue"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Honda Pegoh, Melaka";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-C3"; r.receivedBy = "Raj Kumar";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-007"; r.passed = false; r.qcNotes = "Brake pad below spec - pending replacement";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1004"; r.manufacturer = "Proton"; r.model = "X50";
        r.color = "Red"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Proton Tanjung Malim";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-D4"; r.receivedBy = "Nurul Aina";
        chain.addBlock(r);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    {
        cw1::CarRecord r;
        r.vin = "VIN1005"; r.manufacturer = "Perodua"; r.model = "Myvi";
        r.color = "Black"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }

    const auto vins = chain.getAllVins();
    cout << "  Loaded " << vins.size() << " cars, "
         << chain.totalBlocks() << " total blocks.\n\n";

    const auto result = chain.verifyIntegrity();
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << "\n\n";
}

void viewAllCars(const cw1::Blockchain& chain) {
    const auto vins = chain.getAllVins();
    if (vins.empty()) {
        cout << "\n  No cars in the system.\n\n";
        return;
    }

    cout << "\n  " << left
         << setw(5)  << "No."
         << setw(12) << "VIN"
         << setw(12) << "Brand"
         << setw(10) << "Model"
         << setw(8)  << "Color"
         << setw(6)  << "Year"
         << setw(20) << "Latest Stage"
         << setw(7)  << "Blocks" << '\n';
    printSeparator();

    int num = 1;
    for (const auto& vin : vins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) {
            continue;
        }

        const auto& rec = history.front()->getRecord();
        const auto latestStage = chain.getLatestStage(vin);

        cout << "  " << left
             << setw(5)  << num++
             << setw(12) << rec.vin
             << setw(12) << rec.manufacturer
             << setw(10) << rec.model
             << setw(8)  << rec.color
             << setw(6)  << rec.productionYear
             << setw(20) << cw1::stageToString(latestStage)
             << setw(7)  << history.size() << '\n';
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed all cars summary (" + to_string(vins.size()) + " VINs)");
}

void viewCarByVin(const cw1::Blockchain& chain) {
    const string vin = prompt("Enter VIN");
    if (!chain.hasVin(vin)) {
        cout << "\n  VIN \"" << vin << "\" not found.\n\n";
        return;
    }

    const auto history = chain.getCarHistory(vin);
    cout << "\n  Blockchain for " << vin << " (" << history.size() << " block(s)):\n\n";

    for (const auto* block : history) {
        printBlockDetail(*block);
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed blockchain for " + vin + " (" + to_string(history.size()) + " blocks)");
}

void addNewCarStage(cw1::Blockchain& chain) {
    cout << "\n  Select lifecycle stage to add:\n";
    cout << "  1. Production\n";
    cout << "  2. Warehouse Intake\n";
    cout << "  3. Quality Check\n";
    cout << "  4. Dealer Dispatch\n";
    cout << "  5. Customer Sale\n";

    const string choice = prompt("Stage (1-5)");
    const int stageNum = parseIntOrDefault(choice, 0);
    if (stageNum < 1 || stageNum > 5) {
        cout << "  Invalid stage.\n\n";
        return;
    }

    cw1::CarRecord r;
    r.vin = prompt("VIN");
    r.manufacturer = prompt("Manufacturer");
    r.model = prompt("Model");
    r.color = prompt("Color");
    r.productionYear = parseIntOrDefault(prompt("Production Year"), 0);

    switch (stageNum) {
        case 1:
            r.stage = cw1::BlockStage::PRODUCTION;
            r.factoryLocation = prompt("Factory Location");
            break;
        case 2:
            r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
            r.warehouseLocation = prompt("Warehouse Location");
            r.receivedBy = prompt("Received By");
            break;
        case 3:
            r.stage = cw1::BlockStage::QUALITY_CHECK;
            r.inspectorId = prompt("Inspector ID");
            r.passed = (prompt("Passed? (y/n)") == "y");
            r.qcNotes = prompt("QC Notes");
            break;
        case 4:
            r.stage = cw1::BlockStage::DEALER_DISPATCH;
            r.dealerId = prompt("Dealer ID");
            r.destination = prompt("Destination");
            r.transportMode = prompt("Transport Mode");
            break;
        case 5:
            r.stage = cw1::BlockStage::CUSTOMER_SALE;
            r.buyerId = prompt("Buyer ID");
            r.salePrice = parseDoubleOrDefault(prompt("Sale Price (MYR)"), 0.0);
            r.warrantyExpiry = prompt("Warranty Expiry (YYYY-MM-DD)");
            break;
        default:
            break;
    }

    const double seconds = cw1::measureSeconds([&]() {
        chain.addBlock(r);
    });

    cout << "\n  Block added. Total blocks: " << chain.totalBlocks() << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

void searchCars(const cw1::Blockchain& chain) {
    cout << "\n  Search by:\n";
    cout << "  1. General (VIN / Brand / Model / Color)\n";
    cout << "  2. Brand\n";
    cout << "  3. Lifecycle Stage\n";

    const string choice = prompt("Choice (1-3)");
    vector<const cw1::Block*> results;
    double seconds = 0.0;

    if (choice == "1") {
        const string query = prompt("Search query");
        seconds = cw1::measureSeconds([&]() {
            results = chain.searchGeneral(query);
        });
    } else if (choice == "2") {
        const string brand = prompt("Brand name");
        seconds = cw1::measureSeconds([&]() {
            results = chain.searchByBrand(brand);
        });
    } else if (choice == "3") {
        cout << "  1=Production  2=Intake  3=QC  4=Dispatch  5=Sale\n";
        const int s = parseIntOrDefault(prompt("Stage (1-5)"), 0);
        if (s >= 1 && s <= 5) {
            seconds = cw1::measureSeconds([&]() {
                results = chain.searchByStage(static_cast<cw1::BlockStage>(s - 1));
            });
        } else {
            cout << "  Invalid stage.\n\n";
            return;
        }
    } else {
        cout << "  Invalid choice.\n\n";
        return;
    }

    cout << "\n  Found " << results.size() << " block(s):\n\n";
    for (const auto* block : results) {
        printBlockDetail(*block);
    }
    printOperationDuration(seconds);
    cout << '\n';
}

void verifyIntegrity(const cw1::Blockchain& chain) {
    cw1::ValidationResult result{};
    const double seconds = cw1::measureSeconds([&]() {
        result = chain.verifyIntegrity();
    });

    cout << "\n  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " - " << result.message << '\n';
    printOperationDuration(seconds);
    cout << '\n';
}

void printGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    if (blocks.empty()) {
        cout << "\n  Global chain is empty.\n\n";
        return;
    }

    cout << "\n  Global Chain (" << blocks.size() << " block(s)):\n\n";
    for (const auto& block : blocks) {
        printBlockDetail(block);
    }
    cout << '\n';

    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed global chain (" + to_string(blocks.size()) + " blocks)");
}

void viewAuditLog(const cw1::Blockchain& chain) {
    const cw1::AuditLog& log = chain.getAuditLog();
    cout << "\n  Audit Log - Total entries: " << log.size() << '\n';

    const string input = prompt("View last how many entries? (default 10)");
    std::size_t n = 10;
    if (!input.empty()) {
        const int parsed = parseIntOrDefault(input, 10);
        n = (parsed > 0) ? static_cast<std::size_t>(parsed) : 10;
    }

    cw1::RecentEntryArray entries(log, n);
    if (entries.empty()) {
        cout << "\n  No audit entries to display.\n\n";
        return;
    }

    cout << "\n  Showing last " << entries.size() << " entries:\n";
    printSeparator();

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const cw1::AuditEntry* entry = entries[i];
        if (entry == nullptr) {
            continue;
        }
        cout << "  [" << setw(4) << (i + 1) << "] "
             << left << setw(22) << entry->timestamp
             << setw(20) << cw1::actionToString(entry->action)
             << entry->details << '\n';
    }

    printSeparator();
    cout << '\n';
}

void simulateTamper(cw1::Blockchain& chain) {
    const std::size_t total = chain.totalBlocks();
    if (total == 0) {
        cout << "\n  Cannot tamper: chain is empty.\n\n";
        return;
    }

    cout << "\n  Tamper Simulation (Debug/Admin)\n";
    cout << "  This intentionally breaks blockchain integrity.\n";

    const string promptText = "Block index to tamper (0-" + to_string(total - 1) + ", blank=last)";
    const string indexInput = prompt(promptText);

    std::size_t index = total - 1;
    if (!indexInput.empty()) {
        const int parsed = parseIntOrDefault(indexInput, -1);
        if (parsed < 0 || static_cast<std::size_t>(parsed) >= total) {
            cout << "  Invalid block index.\n\n";
            return;
        }
        index = static_cast<std::size_t>(parsed);
    }

    const double seconds = cw1::measureSeconds([&]() {
        chain.tamperBlock(index);
    });

    cout << '\n';
    cout << "  Debug / Simulation Feature executed.\n";
    cout << "  Payload for block #" << index
         << " was modified without recomputing hashes.\n";
    printOperationDuration(seconds);
    cout << '\n';
}

} // namespace

int main() {
    cw1::Blockchain chain;

    cout << "\n  ========================================\n";
    cout << "    Car Warehouse Blockchain System\n";
    cout << "    Hybrid Architecture (Global + Index)\n";
    cout << "  ========================================\n\n";

    loadDemoData(chain);

    while (true) {
        cout << "  ---- MENU ----\n";
        cout << "  1. View All Cars (summary)\n";
        cout << "  2. View Car Blockchain (by VIN)\n";
        cout << "  3. Add New Block (lifecycle stage)\n";
        cout << "  4. Search\n";
        cout << "  5. View Global Chain\n";
        cout << "  6. Verify Integrity\n";
        cout << "  7. View Audit Log\n";
        cout << "  8. Tamper Simulation (Debug)\n";
        cout << "  9. Exit\n";

        const string choice = prompt("Select (1-9)");

        if (choice == "1") { viewAllCars(chain); continue; }
        if (choice == "2") { viewCarByVin(chain); continue; }
        if (choice == "3") { addNewCarStage(chain); continue; }
        if (choice == "4") { searchCars(chain); continue; }
        if (choice == "5") { printGlobalChain(chain); continue; }
        if (choice == "6") { verifyIntegrity(chain); continue; }
        if (choice == "7") { viewAuditLog(chain); continue; }
        if (choice == "8") { simulateTamper(chain); continue; }
        if (choice == "9") { cout << "\n  Exiting.\n"; break; }

        cout << "  Invalid choice.\n\n";
    }

    return 0;
}
