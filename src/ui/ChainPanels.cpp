// Groups the blockchain-centric panels. These views focus on chain structure,
// auditability, immutability demonstrations, and soft-delete recovery rather
// than vehicle data entry.

#include "ui/GuiApp.hpp"

#include <algorithm>
#include <cstdio>
#include <string>

#include "security/AccessControl.hpp"
#include "utils/OperationTimer.hpp"

namespace {

void RenderTempChainControls(cw1::Blockchain& chain) {
    if (!g_tempGenerating) {
        ImGui::PushStyleColor(ImGuiCol_Button, HexColor(0x1b4d30));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x238636));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x2ea043));
        if (ImGui::Button("Start Generating Temp Blocks##tempstart", ImVec2(280.0f, 36.0f))) {
            g_tempChain = chain.getChain();
            g_tempRealCount = g_tempChain.size();
            g_tempPrevCount = g_tempChain.size();
            g_tempGenerating = true;
            g_tempAccumulator = 0.0f;
            g_tempVerifyDone = false;
            g_tempTamperIndex = -1;
            g_tempDeleteIndex = -1;
            PushToast("Auto-generation started.", COL_GREEN_BR);
        }
        ImGui::PopStyleColor(3);
    } else if (DrawDangerButton("Stop Generating##tempstop", ImVec2(220.0f, 36.0f))) {
        g_tempGenerating = false;
        PushToast("Temp block auto-generation stopped.", COL_YELLOW);
    }

    if (!g_tempChain.empty()) {
        ImGui::Spacing();
        const std::size_t generatedCount = g_tempChain.size() - g_tempRealCount;
        ImGui::TextColored(COL_TEXT,
                           "Temp chain: %zu blocks (%zu real + %zu generated)  |  Generating: %s",
                           g_tempChain.size(), g_tempRealCount, generatedCount,
                           g_tempGenerating ? "ON" : "OFF");
    }

    if (g_tempGenerating || !g_tempChain.empty()) {
        ImGui::Spacing();
        ImGui::SetNextItemWidth(240.0f);
        ImGui::SliderFloat("Generation interval (s)##tempinterval",
                           &g_tempInterval, 1.0f, 10.0f, "%.1f s");
    }

    if (g_tempChain.empty()) {
        return;
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "TAMPER TEMP CHAIN");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetNextItemWidth(140.0f);
    ImGui::InputInt("Tamper temp block index##temptamperidx", &g_tempTamperIndex);
    ImGui::SameLine();

    const bool tamperValid = g_tempTamperIndex >= 0
                          && static_cast<std::size_t>(g_tempTamperIndex) < g_tempChain.size();

    ImGui::BeginDisabled(!tamperValid);
    if (DrawDangerButton("Tamper Data##temptamperdata", ImVec2(140.0f, 34.0f))) {
        g_tempChain[static_cast<std::size_t>(g_tempTamperIndex)]
            .debugTamperPayloadForSimulation("TAMPERED-IMMUTABILITY-DEMO");
        PushToast("Temp block #" + std::to_string(g_tempTamperIndex)
                      + " payload tampered.",
                  COL_RED);
    }
    ImGui::SameLine();
    if (DrawDangerButton("Tamper Hash##temptamperhash", ImVec2(140.0f, 34.0f))) {
        g_tempChain[static_cast<std::size_t>(g_tempTamperIndex)]
            .debugOverrideCurrentHash("000000000000tampered");
        PushToast("Temp block #" + std::to_string(g_tempTamperIndex)
                      + " hash overridden.",
                  COL_RED);
    }
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::SetNextItemWidth(140.0f);
    ImGui::InputInt("Delete temp block index##tempdelidx", &g_tempDeleteIndex);
    ImGui::SameLine();

    const bool deleteValid =
        g_tempDeleteIndex >= 0
        && static_cast<std::size_t>(g_tempDeleteIndex) >= g_tempRealCount
        && static_cast<std::size_t>(g_tempDeleteIndex) < g_tempChain.size()
        && g_tempChain.size() > 1;

    ImGui::BeginDisabled(!deleteValid);
    if (DrawDangerButton("Delete Temp Block##tempdel", ImVec2(180.0f, 34.0f))) {
        const std::size_t deleteIndex = static_cast<std::size_t>(g_tempDeleteIndex);
        g_tempChain.erase(g_tempChain.begin() + static_cast<std::ptrdiff_t>(deleteIndex));
        for (std::size_t i = deleteIndex; i < g_tempChain.size(); ++i) {
            g_tempChain[i].setIndex(i);
        }
        PushToast("Temp block #" + std::to_string(g_tempDeleteIndex)
                      + " deleted (chain links NOT repaired).",
                  COL_RED);
    }
    ImGui::EndDisabled();
    if (g_tempDeleteIndex >= 0
        && static_cast<std::size_t>(g_tempDeleteIndex) < g_tempRealCount) {
        ImGui::SameLine();
        ImGui::TextColored(COL_YELLOW, "Cannot delete original chain blocks (0-%zu)",
                           g_tempRealCount > 0 ? g_tempRealCount - 1 : 0u);
    }
}

void RenderTempChainVerification() {
    if (g_tempChain.empty()) {
        return;
    }

    ImGui::Spacing();
    ImGui::Spacing();
    if (DrawPrimaryButton("Verify Temp Chain##tempverify", ImVec2(220.0f, 36.0f))) {
        cw1::OperationTimer timer;
        g_tempVerify = cw1::Validation::verifyChain(g_tempChain);
        g_tempVerifySeconds = timer.elapsedSeconds();
        g_tempVerifyDone = true;
        PushToast(g_tempVerify.ok ? "Blockchain verified OK."
                                  : "Blockchain integrity FAILED.",
                  g_tempVerify.ok ? COL_GREEN_BR : COL_RED);
    }

    if (!g_tempVerifyDone) {
        return;
    }

    ImGui::Spacing();
    const ImVec4 bannerColor = g_tempVerify.ok ? HexColor(0x1b4d30) : HexColor(0x5a2429);
    const char* bannerText = g_tempVerify.ok ? "BLOCKCHAIN VERIFIED"
                                             : "BLOCKCHAIN INTEGRITY FAILURE";
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bannerColor);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##tempintbanner", ImVec2(-1, 60), true);
    ImGui::SetCursorPos(ImVec2(20.0f, 14.0f));
    ImGui::SetWindowFontScale(1.4f);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", bannerText);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::TextColored(COL_MUTED, "Verification Time:");
    ImGui::SameLine();
    ImGui::TextColored(COL_TEXT, "%s seconds",
                       cw1::formatSeconds(g_tempVerifySeconds).c_str());

    ImGui::TextColored(COL_MUTED, "Result:");
    ImGui::SameLine();
    ImGui::TextWrapped("%s", g_tempVerify.message.c_str());

    if (!g_tempVerify.ok || g_tempVerify.failedIndices.empty()) {
        return;
    }

    ImGui::Spacing();
    ImGui::TextColored(COL_RED, "Failed blocks detail:");
    ImGui::Spacing();

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
    if (ImGui::BeginTable("##tempfaildetail", 4, tableFlags, ImVec2(-1, 0))) {
        ImGui::TableSetupColumn("Block #", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Stored Hash", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Expected Hash", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Failure Type", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableHeadersRow();

        for (std::size_t failedIndex : g_tempVerify.failedIndices) {
            if (failedIndex >= g_tempChain.size()) {
                continue;
            }

            const cw1::Block& block = g_tempChain[failedIndex];
            const std::string expected = block.computeHash();
            std::string failureType;

            if (block.getCurrentHash() != expected) {
                failureType = "Hash mismatch";
            } else if (block.getSha3Hash() != block.computeSha3Hash()) {
                failureType = "SHA3-128 mismatch";
            } else if (failedIndex == 0 && block.getPreviousHash() != block.getCurrentHash()) {
                failureType = "Genesis rule";
            } else if (failedIndex > 0) {
                failureType = "Chain link mismatch";
            }

            ImGui::TableNextRow();
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                   ImGui::ColorConvertFloat4ToU32(HexColor(0x5a2429, 0.4f)));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_RED, "#%zu", failedIndex);

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", Truncate(block.getCurrentHash(), 16).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", Truncate(expected, 16).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_RED, "X  %s", failureType.c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

void RenderTempChainVisual() {
    if (g_tempChain.empty()) {
        return;
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "TEMP CHAIN BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    bool scrollToEnd = false;
    if (g_tempChain.size() != g_tempPrevCount) {
        if (g_tempChain.size() > g_tempPrevCount) {
            g_tempNewBlockAnim = 0.0f;
            scrollToEnd = true;
        }
        g_tempPrevCount = g_tempChain.size();
    }
    if (g_tempNewBlockAnim < 1.0f) {
        g_tempNewBlockAnim += ImGui::GetIO().DeltaTime * 2.0f;
        if (g_tempNewBlockAnim > 1.0f) {
            g_tempNewBlockAnim = 1.0f;
        }
    }

    constexpr float kCardWidth = 170.0f;
    constexpr float kCardHeight = 130.0f;
    constexpr float kArrowWidth = 30.0f;
    const float chainHeight = kCardHeight + 20.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##tempchainscroll", ImVec2(-1.0f, chainHeight), true,
                      ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PopStyleColor();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float startY = ImGui::GetCursorScreenPos().y + 5.0f;

    for (std::size_t index = 0; index < g_tempChain.size(); ++index) {
        const cw1::Block& block = g_tempChain[index];
        const cw1::CarRecord& record = block.getRecord();

        bool isFailed = false;
        if (g_tempVerifyDone) {
            for (std::size_t failedIndex : g_tempVerify.failedIndices) {
                if (failedIndex == index) {
                    isFailed = true;
                    break;
                }
            }
        }

        const bool isNewest = (index == g_tempChain.size() - 1);
        const float alpha = (isNewest && g_tempNewBlockAnim < 1.0f) ? g_tempNewBlockAnim : 1.0f;

        ImVec2 cardMin = ImGui::GetCursorScreenPos();
        cardMin.y = startY;
        const ImVec2 cardMax(cardMin.x + kCardWidth, cardMin.y + kCardHeight);

        const ImVec4 background = isFailed
                                      ? HexColor(0x5a2429, 0.6f * alpha)
                                      : HexColor(0x21262d, alpha);
        const ImVec4 border = isFailed
                                  ? HexColor(0xda3633, 0.8f * alpha)
                                  : HexColor(0x30363d, 0.8f * alpha);
        drawList->AddRectFilled(cardMin, cardMax,
                                ImGui::ColorConvertFloat4ToU32(background), 6.0f);
        drawList->AddRect(cardMin, cardMax,
                          ImGui::ColorConvertFloat4ToU32(border), 6.0f);

        ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 8.0f, cardMin.y + 6.0f));
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));

        ImGui::TextColored(ImVec4(COL_ACCENT.x, COL_ACCENT.y, COL_ACCENT.z, alpha),
                           "Block #%zu", block.getIndex());
        ImGui::TextColored(ImVec4(COL_PURPLE.x, COL_PURPLE.y, COL_PURPLE.z, alpha),
                           "Hash: %s", Truncate(block.getCurrentHash(), 10).c_str());
        ImGui::TextColored(ImVec4(COL_PURPLE.x, COL_PURPLE.y, COL_PURPLE.z, alpha * 0.7f),
                           "Prev: %s", Truncate(block.getPreviousHash(), 10).c_str());
        ImGui::TextColored(ImVec4(COL_TEXT.x, COL_TEXT.y, COL_TEXT.z, alpha),
                           "%s", record.vin.c_str());
        const ImVec4 stageColor = StageColor(record.stage);
        ImGui::TextColored(ImVec4(stageColor.x, stageColor.y, stageColor.z, alpha),
                           "%s", cw1::stageToString(record.stage).c_str());

        if (g_tempVerifyDone) {
            if (isFailed) {
                ImGui::TextColored(ImVec4(COL_RED.x, COL_RED.y, COL_RED.z, alpha), "FAILED");
            } else {
                ImGui::TextColored(ImVec4(COL_GREEN_BR.x, COL_GREEN_BR.y, COL_GREEN_BR.z, alpha),
                                   "OK");
            }
        }

        ImGui::PopStyleVar();
        ImGui::EndGroup();

        if (ImGui::IsMouseHoveringRect(cardMin, cardMax)) {
            ImGui::BeginTooltip();
            ImGui::TextColored(COL_ACCENT, "Block #%zu", block.getIndex());
            ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", block.getCurrentHash().c_str());
            ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", block.getSha3Hash().c_str());
            ImGui::TextColored(COL_PURPLE, "Prev:     %s", block.getPreviousHash().c_str());
            ImGui::EndTooltip();
        }

        ImGui::SetCursorScreenPos(ImVec2(cardMax.x, startY));

        if (index + 1 < g_tempChain.size()) {
            const float arrowCenterY = startY + kCardHeight * 0.5f;
            drawList->AddLine(ImVec2(cardMax.x + 4.0f, arrowCenterY),
                              ImVec2(cardMax.x + kArrowWidth - 6.0f, arrowCenterY),
                              ImGui::ColorConvertFloat4ToU32(COL_MUTED), 2.0f);
            drawList->AddTriangleFilled(
                ImVec2(cardMax.x + kArrowWidth - 6.0f, arrowCenterY - 5.0f),
                ImVec2(cardMax.x + kArrowWidth - 6.0f, arrowCenterY + 5.0f),
                ImVec2(cardMax.x + kArrowWidth, arrowCenterY),
                ImGui::ColorConvertFloat4ToU32(COL_MUTED));
            ImGui::SetCursorScreenPos(ImVec2(cardMax.x + kArrowWidth, startY));
        }

        if (isNewest && scrollToEnd) {
            ImGui::SetScrollHereX(1.0f);
        }
    }

    const float totalWidth =
        g_tempChain.size() * kCardWidth + (g_tempChain.size() - 1) * kArrowWidth;
    ImGui::SetCursorPos(ImVec2(totalWidth, 0));
    ImGui::Dummy(ImVec2(1, 1));

    ImGui::EndChild();
}

void RenderSecurityPanel(cw1::Blockchain& chain) {
    ImGui::Spacing();
    ImGui::Spacing();
    DrawSectionHeading("Security Validation");
    ImGui::Separator();
    ImGui::Spacing();

    const auto& blocks = chain.getChain();
    int verified = 0;
    int unverified = 0;
    int noSignature = 0;
    for (const auto& block : blocks) {
        if (chain.isDeleted(block.getIndex())) {
            continue;
        }
        if (block.getCreatorSignature().empty()) {
            ++noSignature;
        } else if (block.isSignatureVerified()) {
            ++verified;
        } else {
            ++unverified;
        }
    }

    ImGui::TextColored(COL_MUTED, "Block Signature Summary:");
    ImGui::Spacing();

    char buffer[64];
    constexpr float kGap = 12.0f;
    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const float rowWidth = std::min(availableWidth, 900.0f);
    const float cardWidth = (rowWidth - (kGap * 2.0f)) / 3.0f;
    const float valueHeight =
        (g_fontSection != nullptr) ? g_fontSection->FontSize : ImGui::GetFontSize();
    const float cardHeight =
        valueHeight + ImGui::GetTextLineHeightWithSpacing()
        + (ImGui::GetStyle().WindowPadding.y * 2.0f) + 16.0f;
    if (availableWidth > rowWidth) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availableWidth - rowWidth) * 0.5f);
    }

    std::snprintf(buffer, sizeof(buffer), "%d", verified);
    DrawStatCard(buffer, "Signatures Verified", COL_GREEN_BR, ImVec2(cardWidth, cardHeight));
    ImGui::SameLine(0, kGap);

    std::snprintf(buffer, sizeof(buffer), "%d", unverified);
    DrawStatCard(buffer, "Unverified", COL_YELLOW, ImVec2(cardWidth, cardHeight));
    ImGui::SameLine(0, kGap);

    std::snprintf(buffer, sizeof(buffer), "%d", noSignature);
    DrawStatCard(buffer, "No Signature", COL_MUTED, ImVec2(cardWidth, cardHeight));

    ImGui::Spacing();
    std::snprintf(buffer, sizeof(buffer), "%d", g_pendingMgr.pendingCount());
    ImGui::TextColored(COL_MUTED, "Pending Approval Requests:");
    ImGui::SameLine();
    ImGui::TextColored(COL_YELLOW, "%s", buffer);
}

}  // namespace

void RenderGlobalChain(cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    const cw1::LinkedBlockNode* head = chain.getLinkedHead();

    DrawSectionHeading("Global Chain");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu block(s))", blocks.size());
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "BLOCKCHAIN VISUALIZATION");
    ImGui::Spacing();

    {
        constexpr float kCardWidth = 180.0f;
        constexpr float kCardHeight = 150.0f;
        constexpr float kArrowWidth = 36.0f;
        const float chainHeight = kCardHeight + 24.0f;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##global_horiz_chain", ImVec2(-1.0f, chainHeight), true,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PopStyleColor();

        if (blocks.empty()) {
            ImGui::TextColored(COL_MUTED, "No blocks in the chain.");
        } else {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            const float startY = ImGui::GetCursorScreenPos().y + 5.0f;

            for (std::size_t i = 0; i < blocks.size(); ++i) {
                const cw1::Block& block = blocks[i];
                if (chain.isDeleted(block.getIndex())) {
                    continue;
                }
                const cw1::CarRecord& record = block.getRecord();

                ImVec2 cardMin = ImGui::GetCursorScreenPos();
                cardMin.y = startY;
                const ImVec2 cardMax(cardMin.x + kCardWidth, cardMin.y + kCardHeight);

                drawList->AddRectFilled(cardMin, cardMax,
                                        ImGui::ColorConvertFloat4ToU32(COL_BG_ELEV), 6.0f);
                drawList->AddRect(cardMin, cardMax,
                                  ImGui::ColorConvertFloat4ToU32(COL_BORDER_SOFT), 6.0f);

                ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 8.0f, cardMin.y + 6.0f));
                ImGui::BeginGroup();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));

                ImGui::TextColored(COL_ACCENT, "Block #%zu", block.getIndex());
                ImGui::TextColored(COL_PURPLE, "Hash: %s",
                                   Truncate(block.getCurrentHash(), 10).c_str());
                ImGui::TextColored(COL_PURPLE, "Prev: %s",
                                   Truncate(block.getPreviousHash(), 10).c_str());
                ImGui::TextColored(COL_TEXT, "%s", record.vin.c_str());
                ImGui::TextColored(COL_MUTED, "%s %s",
                                   record.manufacturer.c_str(), record.model.c_str());
                ImGui::TextColored(StageColor(record.stage), "%s",
                                   cw1::stageToString(record.stage).c_str());
                ImGui::TextColored(COL_VERY_MUTED, "%s", block.getTimestamp().c_str());

                ImGui::PopStyleVar();
                ImGui::EndGroup();

                if (ImGui::IsMouseHoveringRect(cardMin, cardMax)) {
                    ImGui::BeginTooltip();
                    ImGui::TextColored(COL_ACCENT, "Block #%zu", block.getIndex());
                    ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", block.getCurrentHash().c_str());
                    ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", block.getSha3Hash().c_str());
                    ImGui::TextColored(COL_PURPLE, "Prev:     %s", block.getPreviousHash().c_str());
                    ImGui::Separator();
                    ImGui::TextColored(COL_TEXT, "VIN: %s", record.vin.c_str());
                    ImGui::TextColored(COL_TEXT, "%s %s (%s, %d)",
                                       record.manufacturer.c_str(),
                                       record.model.c_str(),
                                       record.color.c_str(),
                                       record.productionYear);
                    ImGui::TextColored(StageColor(record.stage), "Stage: %s",
                                       cw1::stageToString(record.stage).c_str());
                    if (!block.getCreatedBy().empty()) {
                        ImGui::TextColored(COL_MUTED, "Created by: %s",
                                           block.getCreatedBy().c_str());
                    }
                    ImGui::EndTooltip();
                }

                if (ImGui::IsMouseHoveringRect(cardMin, cardMax) && ImGui::IsMouseClicked(0)) {
                    g_selectedVin = record.vin;
                    g_view = View::CAR_DETAIL;
                }

                ImGui::SetCursorScreenPos(ImVec2(cardMax.x, startY));

                if (i + 1 < blocks.size()) {
                    const float arrowCenterY = startY + kCardHeight * 0.5f;
                    drawList->AddLine(ImVec2(cardMax.x + 4.0f, arrowCenterY),
                                      ImVec2(cardMax.x + kArrowWidth - 8.0f, arrowCenterY),
                                      ImGui::ColorConvertFloat4ToU32(COL_MUTED), 2.0f);
                    drawList->AddTriangleFilled(
                        ImVec2(cardMax.x + kArrowWidth - 8.0f, arrowCenterY - 5.0f),
                        ImVec2(cardMax.x + kArrowWidth - 8.0f, arrowCenterY + 5.0f),
                        ImVec2(cardMax.x + kArrowWidth - 2.0f, arrowCenterY),
                        ImGui::ColorConvertFloat4ToU32(COL_MUTED));
                    ImGui::SetCursorScreenPos(ImVec2(cardMax.x + kArrowWidth, startY));
                }
            }

            std::size_t visibleCount = 0;
            for (const auto& block : blocks) {
                if (!chain.isDeleted(block.getIndex())) {
                    ++visibleCount;
                }
            }
            const float totalWidth =
                visibleCount * kCardWidth + (visibleCount > 0 ? (visibleCount - 1) * kArrowWidth : 0);
            ImGui::SetCursorPos(ImVec2(totalWidth, 0));
            ImGui::Dummy(ImVec2(1, 1));
        }

        ImGui::EndChild();
    }
    ImGui::Spacing();

    const ImGuiTableFlags linkedFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##linked_nodes", 5, linkedFlags, ImVec2(-1, 170.0f))) {
        ImGui::TableSetupColumn("Node*", ImGuiTableColumnFlags_WidthStretch, 2.2f);
        ImGui::TableSetupColumn("Block #", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("VIN", ImGuiTableColumnFlags_WidthStretch, 1.6f);
        ImGui::TableSetupColumn("Prev Link", ImGuiTableColumnFlags_WidthStretch, 1.8f);
        ImGui::TableSetupColumn("Next*", ImGuiTableColumnFlags_WidthStretch, 2.2f);
        ImGui::TableHeadersRow();

        const cw1::LinkedBlockNode* node = head;
        while (node != nullptr) {
            const cw1::Block& block = *node->block;
            if (chain.isDeleted(block.getIndex())) {
                node = node->next;
                continue;
            }
            const std::string nodePtr = PointerToString(node);
            const std::string nextPtr =
                (node->next != nullptr) ? PointerToString(node->next) : "nullptr";

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", nodePtr.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%zu", block.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ACCENT, "%s", block.getRecord().vin.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s",
                               Truncate(block.getPreviousHash(), 12).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", nextPtr.c_str());

            node = node->next;
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "TABULAR CHAIN DATA (AUTHORITATIVE VECTOR)");
    ImGui::Spacing();

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##global", 8, tableFlags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Hash", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Prev Hash", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Nonce", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < blocks.size(); ++i) {
            const cw1::Block& block = blocks[i];
            const cw1::CarRecord& record = block.getRecord();

            if (chain.isDeleted(block.getIndex())) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%zu", block.getIndex());

            ImGui::TableNextColumn();
            DrawHashWithTooltip(block);

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s",
                               Truncate(block.getPreviousHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", block.getPreviousHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            char selectableId[80];
            std::snprintf(selectableId, sizeof(selectableId), "%s##gc%zu",
                          record.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selectableId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = record.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", record.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(record.stage), "%s",
                               cw1::stageToString(record.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ORANGE, "%llu",
                               static_cast<unsigned long long>(block.getNonce()));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", block.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    LogViewOnce(chain, View::GLOBAL_CHAIN, "", "Global chain viewed");
}

void RenderAuditLog(cw1::Blockchain& chain) {
    DrawSectionHeading("Audit Log");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu total event(s))", chain.getAuditLog().size());
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Show last");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##auditn", &g_auditN);
    if (g_auditN < 1) {
        g_auditN = 1;
    }
    if (g_auditN > 500) {
        g_auditN = 500;
    }
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, "entries");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const cw1::RecentEntryArray entries(chain.getAuditLog(),
                                        static_cast<std::size_t>(g_auditN));

    const auto actionColor = [](cw1::AuditAction action) -> ImVec4 {
        switch (action) {
            case cw1::AuditAction::BLOCK_ADDED: return COL_GREEN_BR;
            case cw1::AuditAction::INTEGRITY_CHECK: return COL_YELLOW;
            case cw1::AuditAction::SEARCH_PERFORMED: return COL_ACCENT;
            case cw1::AuditAction::CHAIN_VIEWED: return COL_PURPLE;
            case cw1::AuditAction::TAMPER_SIMULATED: return COL_RED;
            case cw1::AuditAction::PERSISTENCE_IO: return COL_ORANGE;
            case cw1::AuditAction::BLOCK_DELETED: return COL_RED;
            case cw1::AuditAction::USER_LOGIN: return COL_ACCENT;
            case cw1::AuditAction::USER_LOGOUT: return COL_MUTED;
            case cw1::AuditAction::APPROVAL_ACTION: return COL_YELLOW;
            case cw1::AuditAction::ACCESS_DENIED: return COL_RED;
            default: return COL_TEXT;
        }
    };

    const auto actionName = [](cw1::AuditAction action) -> const char* {
        switch (action) {
            case cw1::AuditAction::BLOCK_ADDED: return "BLOCK_ADDED";
            case cw1::AuditAction::INTEGRITY_CHECK: return "INTEGRITY_CHECK";
            case cw1::AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
            case cw1::AuditAction::CHAIN_VIEWED: return "CHAIN_VIEWED";
            case cw1::AuditAction::TAMPER_SIMULATED: return "TAMPER_SIMULATED";
            case cw1::AuditAction::PERSISTENCE_IO: return "PERSISTENCE_IO";
            case cw1::AuditAction::BLOCK_DELETED: return "BLOCK_DELETED";
            case cw1::AuditAction::USER_LOGIN: return "USER_LOGIN";
            case cw1::AuditAction::USER_LOGOUT: return "USER_LOGOUT";
            case cw1::AuditAction::APPROVAL_ACTION: return "APPROVAL_ACTION";
            case cw1::AuditAction::ACCESS_DENIED: return "ACCESS_DENIED";
            default: return "UNKNOWN";
        }
    };

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##auditlog", 3, tableFlags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch, 4.0f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < entries.size(); ++i) {
            const cw1::AuditEntry* entry = entries[i];
            if (entry == nullptr) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", entry->timestamp.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(actionColor(entry->action), "%s", actionName(entry->action));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_TEXT, "%s", entry->details.c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

void RenderIntegrity(cw1::Blockchain& chain) {
    DrawSectionHeading("Blockchain Characteristics: Immutability");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const bool canTamper = cw1::AccessControl::canTamper(CurrentUserRole());

    if (g_tempChain.empty() && !g_tempGenerating) {
        g_tempChain = chain.getChain();
        g_tempRealCount = g_tempChain.size();
        g_tempPrevCount = g_tempChain.size();
    }

    if (canTamper) {
        RenderTempChainControls(chain);
    } else {
        ImGui::TextColored(COL_YELLOW,
                           "Tamper simulation controls are restricted to Admin users.");
        ImGui::Spacing();
    }

    RenderTempChainVerification();
    RenderTempChainVisual();

    if (!g_tempChain.empty() && canTamper) {
        ImGui::Spacing();
        if (ImGui::Button("Clear Temp Chain##tempclear", ImVec2(180.0f, 34.0f))) {
            g_tempChain.clear();
            g_tempGenerating = false;
            g_tempVerifyDone = false;
            g_tempAccumulator = 0.0f;
            g_tempRealCount = 0;
            g_tempPrevCount = 0;
            g_tempNewBlockAnim = 1.0f;
            g_tempTamperIndex = -1;
            g_tempDeleteIndex = -1;
            PushToast("Temp chain cleared.", COL_MUTED);
        }
    }

    RenderSecurityPanel(chain);
}

void RenderDeleteBlock(cw1::Blockchain& chain) {
    DrawSectionHeading("Delete Block");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (!cw1::AccessControl::canDelete(CurrentUserRole())) {
        ImGui::TextColored(COL_RED, "Access Denied: Your role does not permit block deletion.");
        const std::string denyMessage = "Unauthorized delete page access by "
                                      + (g_authMgr.isLoggedIn()
                                             ? g_authMgr.currentUser().username
                                             : std::string("unknown"));
        chain.getAuditLog().log(cw1::AuditAction::ACCESS_DENIED, denyMessage);
        return;
    }

    ImGui::TextColored(COL_MUTED, "Chain size: %zu block(s)", chain.totalBlocks());
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Block index to delete:");
    ImGui::SetNextItemWidth(160.0f);
    ImGui::InputInt("##delete_block_index", &g_deleteBlockIndex);
    ImGui::Spacing();

    if (DrawDangerButton("Soft Delete (Tombstone)##soft_del", ImVec2(220.0f, 36.0f))) {
        if (g_deleteBlockIndex < 0
            || static_cast<std::size_t>(g_deleteBlockIndex) >= chain.totalBlocks()) {
            PushToast("Invalid block index.", COL_RED);
        } else {
            try {
                std::string message;
                cw1::OperationTimer timer;
                const bool ok = chain.softDeleteBlock(
                    static_cast<std::size_t>(g_deleteBlockIndex), message);
                g_lastDeleteSeconds = timer.elapsedSeconds();

                PushToast(ok ? message : ("Soft delete failed: " + message),
                          ok ? COL_GREEN_BR : COL_RED);
                RefreshChainVerification(chain);
            } catch (const std::exception& ex) {
                ReportGuiException(chain, cw1::AuditAction::BLOCK_DELETED,
                                   "Soft delete failed", ex);
            }
        }
    }

    if (g_lastDeleteSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last delete operation took: %s s",
                           cw1::formatSeconds(g_lastDeleteSeconds).c_str());
    }

    const auto deletedIndices = chain.getDeletedIndices();
    if (deletedIndices.empty()) {
        return;
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextColored(COL_YELLOW, "Soft-Deleted Blocks (original data preserved)");
    ImGui::Spacing();

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
    if (ImGui::BeginTable("##deleted_blocks_table", 5, tableFlags, ImVec2(-1, 0))) {
        ImGui::TableSetupColumn("Block #", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("VIN", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Model", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableHeadersRow();

        for (std::size_t index : deletedIndices) {
            const cw1::CarRecord* original = chain.getDeletedOriginal(index);
            if (original == nullptr) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(COL_RED, "#%zu", index);

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", original->vin.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", original->manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", original->model.c_str());

            ImGui::TableNextColumn();
            const std::string label = "Restore##restore_" + std::to_string(index);
            if (DrawPrimaryButton(label.c_str(), ImVec2(90.0f, 28.0f))) {
                try {
                    std::string message;
                    const bool ok = chain.restoreBlock(index, message);
                    PushToast(ok ? message : ("Restore failed: " + message),
                              ok ? COL_GREEN_BR : COL_RED);
                    RefreshChainVerification(chain);
                } catch (const std::exception& ex) {
                    ReportGuiException(chain, cw1::AuditAction::BLOCK_DELETED,
                                       "Restore failed", ex);
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}
