#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"

using namespace std;

namespace {

// â”€â”€ Helper: safe integer parsing â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

int parseIntOrDefault(const string& value, int fallback) {
    try {
        return stoi(value);
    } catch (...) {
        return fallback;
    }
}

double parseDoubleOrDefault(const string& value, double fallback) {
    try {
        return stod(value);
    } catch (...) {
        return fallback;
    }
}

// â”€â”€ Helper: read a line with prompt â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

string prompt(const string& label) {
    string value;
    cout << "  " << label << ": ";
    getline(cin, value);
    return value;
}

// â”€â”€ Demo data â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

void loadDemoData(cw1::Blockchain& chain) {
    cout << "\n  Loading demo fleet...\n";

    // â”€â”€ Car 1: Perodua Axia (full lifecycle â€” all 5 stages) â”€â”€â”€â”€â”€â”€â”€â”€
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

    // Small delay so timestamps differ between cars.
    this_thread::sleep_for(chrono::milliseconds(50));

    // â”€â”€ Car 2: Toyota Vios (4 stages â€” awaiting sale) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
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
        r.inspectorId = "QC-003"; r.passed = true; r.qcNotes = "Minor paint scratch â€” touched up";
        chain.addBlock(r);

        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05"; r.destination = "Kuala Lumpur"; r.transportMode = "Truck";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // â”€â”€ Car 3: Honda City (3 stages â€” in QC) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
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
        r.inspectorId = "QC-007"; r.passed = false; r.qcNotes = "Brake pad below spec â€” pending replacement";
        chain.addBlock(r);
    }

    this_thread::sleep_for(chrono::milliseconds(50));

    // â”€â”€ Car 4: Proton X50 (2 stages â€” just arrived at warehouse) â”€â”€
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

    this_thread::sleep_for(chrono::milliseconds(50));

    // â”€â”€ Car 5: Perodua Myvi (1 stage â€” just manufactured) â”€â”€â”€â”€â”€â”€â”€â”€â”€
    {
        cw1::CarRecord r;
        r.vin = "VIN1005"; r.manufacturer = "Perodua"; r.model = "Myvi";
        r.color = "Black"; r.productionYear = 2025;

        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }

    // Print summary
    const auto vins = chain.getAllVins();
    cout << "  Loaded " << vins.size() << " cars, "
         << chain.totalBlocks() << " total blocks.\n\n";

    // Verify integrity after loading
    const auto result = chain.verifyIntegrity();
    cout << "  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " â€” " << result.message << "\n\n";
}

// â”€â”€ Display helpers â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

void printSeparator() {
    cout << "  " << string(60, '-') << '\n';
}

void printBlockDetail(const cw1::Block& block) {
    const cw1::CarRecord& r = block.getRecord();

    printSeparator();
    cout << "  Block #" << block.getIndex()
         << "  |  " << cw1::stageToString(r.stage) << '\n';
    printSeparator();
    cout << "  Timestamp   : " << block.getTimestamp() << '\n';
    cout << "  Nonce       : " << block.getNonce() << '\n';
    cout << "  PrevHash    : " << block.getPreviousHash() << '\n';
    cout << "  CurrentHash : " << block.getCurrentHash() << '\n';
    cout << "  VIN         : " << r.vin << '\n';
    cout << "  Car         : " << r.manufacturer << " " << r.model
         << " (" << r.color << ", " << r.productionYear << ")\n";

    // Print stage-specific fields
    switch (r.stage) {
        case cw1::BlockStage::PRODUCTION:
            cout << "  Factory     : " << r.factoryLocation << '\n';
            break;
        case cw1::BlockStage::WAREHOUSE_INTAKE:
            cout << "  Warehouse   : " << r.warehouseLocation << '\n';
            cout << "  Received By : " << r.receivedBy << '\n';
            break;
        case cw1::BlockStage::QUALITY_CHECK:
            cout << "  Inspector   : " << r.inspectorId << '\n';
            cout << "  QC Result   : " << (r.passed ? "PASSED" : "FAILED") << '\n';
            cout << "  QC Notes    : " << r.qcNotes << '\n';
            break;
        case cw1::BlockStage::DEALER_DISPATCH:
            cout << "  Dealer      : " << r.dealerId << '\n';
            cout << "  Destination : " << r.destination << '\n';
            cout << "  Transport   : " << r.transportMode << '\n';
            break;
        case cw1::BlockStage::CUSTOMER_SALE:
            cout << "  Buyer       : " << r.buyerId << '\n';
            cout << "  Sale Price  : MYR " << fixed << setprecision(2) << r.salePrice << '\n';
            cout << "  Warranty    : " << r.warrantyExpiry << '\n';
            break;
    }
}

// â”€â”€ Menu actions â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

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
        if (history.empty()) continue;

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

    // Log that the car summary was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed all cars summary (" + to_string(vins.size()) + " VINs)");
}

void viewCarByVin(const cw1::Blockchain& chain) {
    string vin = prompt("Enter VIN");
    if (!chain.hasVin(vin)) {
        cout << "\n  VIN \"" << vin << "\" not found.\n\n";
        return;
    }

    const auto history = chain.getCarHistory(vin);
    cout << "\n  Blockchain for " << vin << " (" << history.size() << " blocks):\n";

    for (const auto* block : history) {
        printBlockDetail(*block);
    }
    printSeparator();
    cout << '\n';

    // Log that this VIN chain was viewed.
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

    string choice = prompt("Stage (1-5)");
    int stageNum = parseIntOrDefault(choice, 0);
    if (stageNum < 1 || stageNum > 5) {
        cout << "  Invalid stage.\n\n";
        return;
    }

    cw1::CarRecord r;

    // Common fields
    r.vin              = prompt("VIN");
    r.manufacturer     = prompt("Manufacturer");
    r.model            = prompt("Model");
    r.color            = prompt("Color");
    r.productionYear   = parseIntOrDefault(prompt("Production Year"), 0);

    // Stage-specific fields
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
    }

    chain.addBlock(r);
    cout << "\n  Block added. Total blocks: " << chain.totalBlocks() << "\n\n";
}

void searchCars(const cw1::Blockchain& chain) {
    cout << "\n  Search by:\n";
    cout << "  1. General (VIN / Brand / Model / Color)\n";
    cout << "  2. Brand\n";
    cout << "  3. Lifecycle Stage\n";

    string choice = prompt("Choice (1-3)");

    vector<const cw1::Block*> results;

    if (choice == "1") {
        string query = prompt("Search query");
        results = chain.searchGeneral(query);
    } else if (choice == "2") {
        string brand = prompt("Brand name");
        results = chain.searchByBrand(brand);
    } else if (choice == "3") {
        cout << "  1=Production  2=Intake  3=QC  4=Dispatch  5=Sale\n";
        int s = parseIntOrDefault(prompt("Stage (1-5)"), 0);
        if (s >= 1 && s <= 5) {
            results = chain.searchByStage(static_cast<cw1::BlockStage>(s - 1));
        }
    } else {
        cout << "  Invalid choice.\n\n";
        return;
    }

    cout << "\n  Found " << results.size() << " block(s):\n";
    for (const auto* block : results) {
        printBlockDetail(*block);
    }
    if (!results.empty()) printSeparator();
    cout << '\n';
}

void verifyIntegrity(const cw1::Blockchain& chain) {
    const auto result = chain.verifyIntegrity();
    cout << "\n  Integrity: " << (result.ok ? "PASS" : "FAIL")
         << " â€” " << result.message << "\n\n";
}

void printGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    if (blocks.empty()) {
        cout << "\n  Global chain is empty.\n\n";
        return;
    }

    cout << "\n  Global Chain (" << blocks.size() << " blocks):\n";
    for (const auto& block : blocks) {
        const auto& r = block.getRecord();
        cout << "  [" << setw(3) << block.getIndex() << "] "
             << left << setw(10) << r.vin
             << setw(18) << cw1::stageToString(r.stage)
             << setw(10) << r.manufacturer
             << block.getCurrentHash().substr(0, 16) << "...\n";
    }
    cout << '\n';

    // Log that the global chain was viewed.
    chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED,
        "Viewed global chain (" + to_string(blocks.size()) + " blocks)");
}
/// Display the most recent N entries from the audit log.
/// Demonstrates Array of Pointers: getRecentEntries() returns a heap-
/// allocated array of const AuditEntry* that we iterate and then delete[].
void viewAuditLog(const cw1::Blockchain& chain) {
    const cw1::AuditLog& log = chain.getAuditLog();
    cout << "\n  Audit Log -- Total entries: " << log.size() << "\n";

    string input = prompt("View last how many entries? (default 10)");
    size_t n = 10;
    if (!input.empty()) {
        int parsed = parseIntOrDefault(input, 10);
        n = (parsed > 0) ? static_cast<size_t>(parsed) : 10;
    }

    size_t actualCount = 0;
    // getRecentEntries demonstrates Array of Pointers: a heap-allocated
    // raw array (const AuditEntry**) where each slot is a pointer to a
    // linked list node. We must delete[] the array after use.
    const cw1::AuditEntry** arr = log.getRecentEntries(n, actualCount);

    if (arr == nullptr || actualCount == 0) {
        cout << "\n  No audit entries to display.\n\n";
        return;
    }

    cout << "\n  Showing last " << actualCount << " entries:\n";
    printSeparator();

    // Iterate the raw pointer array -- demonstrates array-of-pointers access.
    for (size_t i = 0; i < actualCount; ++i) {
        const cw1::AuditEntry* entry = arr[i];
        cout << "  [" << setw(4) << (i + 1) << "] "
             << left << setw(22) << entry->timestamp
             << setw(20) << cw1::actionToString(entry->action)
             << entry->details << "\n";
    }

    printSeparator();
    cout << "\n";

    // Free the array of pointers (entries themselves are owned by AuditLog).
    delete[] arr;
}

} // namespace

// â”€â”€ Main â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

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
    cout << "  8. Exit\n";

        string choice = prompt("Select (1-8)");

        if (choice == "1") { viewAllCars(chain); continue; }
        if (choice == "2") { viewCarByVin(chain); continue; }
        if (choice == "3") { addNewCarStage(chain); continue; }
        if (choice == "4") { searchCars(chain); continue; }
        if (choice == "5") { printGlobalChain(chain); continue; }
        if (choice == "6") { verifyIntegrity(chain); continue; }
        if (choice == "7") { viewAuditLog(chain); continue; }
        if (choice == "8") { cout << "\n  Exiting.\n"; break; }

        cout << "  Invalid choice.\n\n";
    }

    return 0;
}
