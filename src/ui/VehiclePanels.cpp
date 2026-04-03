// Car detail view and the add-block form.

#include "ui/GuiApp.hpp"

#include <cstdio>
#include <string>

#include "security/AccessControl.hpp"
#include "security/SecurityUtil.hpp"
#include "utils/BranchMap.hpp"
#include "utils/OperationTimer.hpp"
#include "utils/TimeUtil.hpp"
#include "utils/VehicleData.hpp"

namespace {

void DetailRow(const char* label, const char* value, ImVec4 valueColor = COL_TEXT) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextColored(COL_MUTED, "%s", label);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextColored(valueColor, "%s", value);
}

}  // namespace

void RenderCarDetail(cw1::Blockchain& chain) {
    if (ImGui::Button("< Back")) {
        g_view = View::DASHBOARD;
        return;
    }
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " / ");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "%s", g_selectedVin.c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (g_selectedVin.empty() || !chain.hasVin(g_selectedVin)) {
        ImGui::TextColored(COL_RED, "VIN not found.");
        return;
    }

    const auto history = chain.getCarHistory(g_selectedVin);
    if (history.empty()) {
        ImGui::TextColored(COL_MUTED, "No blocks for this VIN.");
        return;
    }

    const cw1::CarRecord& first = history.front()->getRecord();
    const cw1::BlockStage latestStage = chain.getLatestStage(g_selectedVin);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
    ImGui::BeginChild("##vehHeader", ImVec2(-1, 80), true);
    if (g_fontSection != nullptr) {
        ImGui::PushFont(g_fontSection);
    }
    ImGui::TextColored(COL_TEXT, "%s %s", first.manufacturer.c_str(), first.model.c_str());
    if (g_fontSection != nullptr) {
        ImGui::PopFont();
    }
    ImGui::SameLine(0.0f, 16.0f);
    ImGui::TextColored(COL_MUTED, "(%d)", first.productionYear);
    ImGui::SameLine(0.0f, 16.0f);
    ImGui::TextColored(COL_MUTED, "%s", first.color.c_str());
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "VIN:");
    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextColored(COL_TEXT, "%s", g_selectedVin.c_str());
    ImGui::SameLine(0.0f, 24.0f);
    ImGui::TextColored(COL_MUTED, "Current Stage:");
    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextColored(StageColor(latestStage), "%s", cw1::stageToString(latestStage).c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "LIFECYCLE TIMELINE  (%zu block(s))", history.size());
    ImGui::Separator();
    ImGui::Spacing();

    const cw1::Role detailRole = CurrentUserRole();

    for (std::size_t index = 0; index < history.size(); ++index) {
        const cw1::Block& block = *history[index];
        const cw1::CarRecord& record = block.getRecord();
        const ImVec4 stageColor = StageColor(record.stage);

        char headerLabel[128];
        std::snprintf(headerLabel, sizeof(headerLabel), "Block #%zu  |  %s##det%zu",
                      block.getIndex(), cw1::stageToString(record.stage).c_str(), index);

        const ImVec2 headerPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            headerPos,
            ImVec2(headerPos.x + 4.0f, headerPos.y + ImGui::GetFrameHeight()),
            ImGui::ColorConvertFloat4ToU32(stageColor), 2.0f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

        ImGui::PushStyleColor(ImGuiCol_Header, COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, HexColor(0x1f6feb, 0.2f));
        const bool open = ImGui::CollapsingHeader(
            headerLabel,
            index == history.size() - 1 ? ImGuiTreeNodeFlags_DefaultOpen
                                        : ImGuiTreeNodeFlags_None);
        ImGui::PopStyleColor(3);
        if (!open) {
            continue;
        }

        int rows = 7;
        const bool isGenesis = (block.getIndex() == 0);
        if (isGenesis) {
            ++rows;
        }
        switch (record.stage) {
            case cw1::BlockStage::PRODUCTION: rows += 1; break;
            case cw1::BlockStage::WAREHOUSE_INTAKE: rows += 3; break;
            case cw1::BlockStage::QUALITY_CHECK: rows += 3; break;
            case cw1::BlockStage::DEALER_DISPATCH: rows += 3; break;
            case cw1::BlockStage::CUSTOMER_SALE: rows += 4; break;
        }

        const float rowHeight = ImGui::GetTextLineHeightWithSpacing() + 4.0f;
        const float childHeight = rows * rowHeight + 72.0f;

        char childId[64];
        std::snprintf(childId, sizeof(childId), "##blkdet%zu", index);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        ImGui::BeginChild(childId, ImVec2(-1, childHeight), true);

        ImGui::TextColored(COL_ACCENT, "Block Information");
        ImGui::Spacing();
        if (ImGui::BeginTable("##binfo", 2, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            DetailRow("Current Hash", block.getCurrentHash().c_str(), COL_TEXT);
            DetailRow("SHA3-128 Hash", block.getSha3Hash().c_str(), COL_TEXT);
            DetailRow("Previous Hash", block.getPreviousHash().c_str(), COL_TEXT);
            if (isGenesis) {
                const bool ok = (block.getPreviousHash() == block.getCurrentHash());
                DetailRow("Genesis Rule",
                          ok ? "previousHash == currentHash  YES"
                             : "previousHash == currentHash  NO",
                          ok ? COL_GREEN_BR : COL_RED);
            }
            DetailRow("Timestamp", block.getTimestamp().c_str(), COL_TEXT);
            {
                char nonce[32];
                std::snprintf(nonce, sizeof(nonce), "%llu",
                              static_cast<unsigned long long>(block.getNonce()));
                DetailRow("Nonce", nonce, COL_TEXT);
            }
            DetailRow("VIN", record.vin.c_str(), COL_ACCENT);
            DetailRow("Mfr. ID", record.manufacturerId.c_str(), COL_TEXT);
            {
                char vehicle[128];
                std::snprintf(vehicle, sizeof(vehicle), "%s %s (%s, %d)",
                              record.manufacturer.c_str(),
                              record.model.c_str(),
                              record.color.c_str(),
                              record.productionYear);
                DetailRow("Vehicle", vehicle, COL_TEXT);
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::TextColored(stageColor, "Payload  -  %s",
                           cw1::stageToString(record.stage).c_str());
        ImGui::Spacing();

        if (ImGui::BeginTable("##payload", 2, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("PLabel", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("PValue", ImGuiTableColumnFlags_WidthStretch);

            switch (record.stage) {
                case cw1::BlockStage::PRODUCTION:
                    DetailRow("Factory Location", record.factoryLocation.c_str());
                    break;
                case cw1::BlockStage::WAREHOUSE_INTAKE:
                    DetailRow("Warehouse Location", record.warehouseLocation.c_str());
                    DetailRow("Received By", record.receivedBy.c_str());
                    DetailRow("Supplier ID",
                              cw1::SecurityUtil::visibleValueForRole(
                                  detailRole, "supplierId", record.supplierId).c_str());
                    break;
                case cw1::BlockStage::QUALITY_CHECK:
                    DetailRow("Inspector ID",
                              cw1::SecurityUtil::visibleValueForRole(
                                  detailRole, "inspectorId", record.inspectorId).c_str());
                    DetailRow("QC Result", record.passed ? "PASSED" : "FAILED",
                              record.passed ? COL_GREEN_BR : COL_RED);
                    DetailRow("QC Notes", record.qcNotes.c_str());
                    break;
                case cw1::BlockStage::DEALER_DISPATCH:
                    DetailRow("Dealer ID",
                              cw1::SecurityUtil::visibleValueForRole(
                                  detailRole, "dealerId", record.dealerId).c_str());
                    DetailRow("Destination", record.destination.c_str());
                    DetailRow("Transport Mode", record.transportMode.c_str());
                    break;
                case cw1::BlockStage::CUSTOMER_SALE: {
                    DetailRow("Buyer ID",
                              cw1::SecurityUtil::visibleValueForRole(
                                  detailRole, "buyerId", record.buyerId).c_str());
                    DetailRow("Retailer ID",
                              cw1::SecurityUtil::visibleValueForRole(
                                  detailRole, "retailerId", record.retailerId).c_str());
                    char price[64];
                    std::snprintf(price, sizeof(price), "MYR %.2f", record.salePrice);
                    DetailRow("Sale Price", price, COL_GREEN_BR);
                    DetailRow("Warranty Expiry", record.warrantyExpiry.c_str());
                    break;
                }
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    LogViewOnce(chain, View::CAR_DETAIL, g_selectedVin,
                "Car detail viewed: " + g_selectedVin);
}

void RenderAddBlock(cw1::Blockchain& chain) {
    DrawSectionHeading("Add Block to Chain");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);

    ImGui::TextColored(COL_MUTED, "COMMON FIELDS");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(240);
    {
        const cw1::Role userRole = CurrentUserRole();
        static int allowedStageIndices[5];
        static const char* allowedStageNames[5];
        int allowedCount = 0;
        for (int stageIndex = 0; stageIndex < 5; ++stageIndex) {
            if (cw1::AccessControl::canCreateStage(
                    userRole, static_cast<cw1::BlockStage>(stageIndex))) {
                allowedStageIndices[allowedCount] = stageIndex;
                allowedStageNames[allowedCount] = k_stageNames[stageIndex];
                ++allowedCount;
            }
        }

        if (allowedCount == 0) {
            ImGui::TextColored(COL_RED, "Your role does not permit creating blocks.");
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();
            return;
        }

        static int filteredIndex = 0;
        if (filteredIndex >= allowedCount) {
            filteredIndex = 0;
        }
        ImGui::Combo("Stage##addstage", &filteredIndex, allowedStageNames, allowedCount);
        g_formStage = allowedStageIndices[filteredIndex];
    }
    ImGui::Spacing();

    const auto labelText = [](const char* label, char* buffer, std::size_t size) {
        ImGui::TextColored(COL_MUTED, "%-18s", label);
        ImGui::SameLine();
        char id[64];
        std::snprintf(id, sizeof(id), "##add_%s", label);
        ImGui::SetNextItemWidth(300);
        ImGui::InputText(id, buffer, size);
    };

    labelText("Car Plate Number", g_formVin, sizeof(g_formVin));

    ImGui::SameLine(0, 12);
    if (g_formVin[0] != '\0') {
        const std::string vin = g_formVin;
        if (chain.hasVin(vin)) {
            ImGui::TextColored(COL_GREEN_BR, "[+ Existing: %s]", vin.c_str());
        } else {
            ImGui::TextColored(COL_ACCENT, "[* New: %s]", vin.c_str());
        }
    }

    ImGui::TextColored(COL_MUTED, "%-18s", "Manufacturer");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    if (ImGui::Combo("##add_Manufacturer", &g_formMfrIndex,
                     k_manufacturers, k_manufacturerCount)) {
        g_formModelIndex = 0;
        g_formBranchIndex = 0;
    }

    ImGui::TextColored(COL_MUTED, "%-18s", "Model");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    const ModelList& models = k_modelsByMfr[g_formMfrIndex];
    ImGui::Combo("##add_Model", &g_formModelIndex, models.models, models.count);

    ImGui::TextColored(COL_MUTED, "%-18s", "Branch");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(380);
    const BranchList& branches = k_branchesByMfr[g_formMfrIndex];
    ImGui::Combo("##add_Branch", &g_formBranchIndex, branches.branches, branches.count);

    ImGui::TextColored(COL_MUTED, "%-18s", "Color");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::Combo("##add_Color", &g_formColorIndex, k_colors, k_colorCount);

    ImGui::TextColored(COL_MUTED, "%-18s", "Production Year");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##add_year", &g_formYear);
    if (g_formYear < 1900) {
        g_formYear = 1900;
    }
    if (g_formYear > 2026) {
        g_formYear = 2026;
    }

    ImGui::Spacing();
    ImGui::Spacing();
    labelText("Manufacturer ID", g_formMfrId, sizeof(g_formMfrId));
    ImGui::TextColored(COL_MUTED, "STAGE-SPECIFIC FIELDS");
    ImGui::Separator();
    ImGui::Spacing();

    switch (g_formStage) {
        case 0: {
            const char* selectedBranch = k_branchesByMfr[g_formMfrIndex].branches[g_formBranchIndex];
            ImGui::TextColored(COL_MUTED, "%-18s", "Factory Location");
            ImGui::SameLine();
            ImGui::TextColored(COL_ACCENT, "%s", selectedBranch);
            break;
        }
        case 1:
            labelText("Warehouse", g_formWarehouse, sizeof(g_formWarehouse));
            labelText("Received By", g_formReceivedBy, sizeof(g_formReceivedBy));
            labelText("Supplier ID", g_formSupplierId, sizeof(g_formSupplierId));
            break;
        case 2:
            labelText("Inspector ID", g_formInspector, sizeof(g_formInspector));
            ImGui::TextColored(COL_MUTED, "%-18s", "Passed");
            ImGui::SameLine();
            ImGui::Checkbox("##add_passed", &g_formPassed);
            labelText("QC Notes", g_formQcNotes, sizeof(g_formQcNotes));
            break;
        case 3:
            labelText("Dealer ID", g_formDealerId, sizeof(g_formDealerId));
            labelText("Destination", g_formDestination, sizeof(g_formDestination));
            ImGui::TextColored(COL_MUTED, "%-18s", "Transport Mode");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(300);
            ImGui::Combo("##add_Transport", &g_formTransportIndex,
                         k_transportModes, k_transportModeCount);
            break;
        case 4:
            labelText("Buyer ID", g_formBuyerId, sizeof(g_formBuyerId));
            labelText("Retailer ID", g_formRetailerId, sizeof(g_formRetailerId));
            ImGui::TextColored(COL_MUTED, "%-18s", "Sale Price (MYR)");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::InputDouble("##add_price", &g_formSalePrice, 1000.0, 10000.0, "%.2f");
            labelText("Warranty Expiry", g_formWarranty, sizeof(g_formWarranty));
            break;
    }

    ImGui::Spacing();
    ImGui::Spacing();

    const cw1::Role addRole = CurrentUserRole();
    const bool isAdmin = (addRole == cw1::Role::ADMIN);
    const char* submitLabel = isAdmin ? "  Add Block (Direct)  "
                                      : "  Submit Pending Request  ";

    const bool submit = DrawPrimaryButton(submitLabel, ImVec2(220, 36));
    const bool canSubmit = (g_formVin[0] != '\0');

    if (submit && !canSubmit) {
        ImGui::SameLine();
        ImGui::TextColored(COL_RED, "  Car Plate Number is required.");
    }

    if (submit && canSubmit) {
        cw1::CarRecord record;
        record.vin = g_formVin;
        record.manufacturer = k_manufacturers[g_formMfrIndex];
        record.model = k_modelsByMfr[g_formMfrIndex].models[g_formModelIndex];
        record.color = k_colors[g_formColorIndex];
        record.productionYear = g_formYear;
        record.stage = static_cast<cw1::BlockStage>(g_formStage);
        record.manufacturerId = g_formMfrId;

        switch (g_formStage) {
            case 0:
                record.factoryLocation = k_branchesByMfr[g_formMfrIndex].branches[g_formBranchIndex];
                break;
            case 1:
                record.warehouseLocation = g_formWarehouse;
                record.receivedBy = g_formReceivedBy;
                record.supplierId = g_formSupplierId;
                break;
            case 2:
                record.inspectorId = g_formInspector;
                record.passed = g_formPassed;
                record.qcNotes = g_formQcNotes;
                break;
            case 3:
                record.dealerId = g_formDealerId;
                record.destination = g_formDestination;
                record.transportMode = k_transportModes[g_formTransportIndex];
                break;
            case 4:
                record.buyerId = g_formBuyerId;
                record.retailerId = g_formRetailerId;
                record.salePrice = g_formSalePrice;
                record.warrantyExpiry = g_formWarranty;
                break;
        }

        try {
            if (isAdmin) {
                const cw1::User& user = g_authMgr.currentUser();
                g_lastAddBlockSeconds = cw1::measureSeconds([&]() {
                    chain.addBlockWithMetadata(record, user.username, user.username,
                                               -1, "", true);
                });
                PushToast("Block #" + std::to_string(chain.totalBlocks() - 1)
                              + " added for " + record.vin
                              + " | Operation took: "
                              + cw1::formatSeconds(g_lastAddBlockSeconds) + " s",
                          COL_GREEN_BR);
            } else {
                const cw1::User& user = g_authMgr.currentUser();
                cw1::PendingBlockRequest request;
                request.requestedBy = user.username;
                request.requestedByDisplay = user.displayName;
                request.requestTimestamp = cw1::TimeUtil::nowIso8601();
                request.requestedStage = record.stage;
                request.payload = record;
                request.creatorSignature = cw1::SecurityUtil::signRequest(
                    user.username,
                    request.requestTimestamp,
                    cw1::stageToString(record.stage),
                    record.serialize(),
                    user.secretKey);

                const int requestId = g_pendingMgr.submitRequest(request);
                if (requestId >= 0) {
                    WriteAuditEvent(
                        chain,
                        cw1::AuditAction::APPROVAL_ACTION,
                        "User " + user.username
                            + " submitted pending request #"
                            + std::to_string(requestId)
                            + " for VIN " + record.vin
                            + " (" + cw1::stageToString(record.stage) + ")");
                    PushToast("Pending request #" + std::to_string(requestId)
                                  + " submitted for " + record.vin,
                              COL_ACCENT);
                } else {
                    PushToast("Failed to submit request: " + g_pendingMgr.lastError(), COL_RED);
                }
            }

            ResetAddBlockForm();
        } catch (const std::exception& ex) {
            ReportGuiException(chain, cw1::AuditAction::BLOCK_ADDED,
                               "Add block failed", ex);
        }
    }

    if (!isAdmin) {
        ImGui::Spacing();
        ImGui::TextColored(COL_YELLOW,
                           "Note: Your role requires Admin approval before blocks are committed to the chain.");
    }

    if (g_lastAddBlockSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last add-block operation took: %s s",
                           cw1::formatSeconds(g_lastAddBlockSeconds).c_str());
    }

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}
