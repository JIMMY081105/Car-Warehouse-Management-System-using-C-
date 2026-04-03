// Hosts shared layout widgets that appear regardless of the active content
// panel: the top header, the left navigation/sidebar, and toast overlays.

#include "ui/GuiApp.hpp"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

#include "security/AccessControl.hpp"
#include "utils/OperationTimer.hpp"

void RenderHeader(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f));
    ImGui::BeginChild("##header", ImVec2(-1.0f, 66.0f), true);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
    ImGui::TextColored(COL_ACCENT, "%s", ">> ");
    ImGui::SameLine();
    if (g_fontTitle != nullptr) {
        ImGui::PushFont(g_fontTitle);
    }
    ImGui::Text("Car Warehouse Blockchain");
    if (g_fontTitle != nullptr) {
        ImGui::PopFont();
    }

    const float statsWidth = 620.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - statsWidth
                    + ImGui::GetCursorPosX() - 16.0f);

    if (g_authMgr.isLoggedIn()) {
        const cw1::User& user = g_authMgr.currentUser();
        char userBadge[128];
        std::snprintf(userBadge, sizeof(userBadge), "%s [%s]",
                      user.displayName.c_str(),
                      cw1::roleToString(user.role).c_str());
        DrawMetricBadge(userBadge, COL_TEXT, HexColor(0x30363d, 0.60f));
        ImGui::SameLine();
    }

    char badge[64];
    std::snprintf(badge, sizeof(badge), "%zu blocks", chain.totalBlocks());
    DrawMetricBadge(badge, COL_ACCENT_HO, HexColor(0x1f6feb, 0.14f));
    ImGui::SameLine();

    std::snprintf(badge, sizeof(badge), "%zu cars", chain.getAllVins().size());
    DrawMetricBadge(badge, COL_GREEN_BR, HexColor(0x238636, 0.16f));
    ImGui::SameLine();

    if (g_verifyDone) {
        if (g_lastVerify.ok) {
            DrawMetricBadge("Verified", COL_GREEN_BR, HexColor(0x238636, 0.16f));
        } else {
            DrawMetricBadge("Invalid", COL_RED, HexColor(0xda3633, 0.18f));
        }
    } else {
        DrawMetricBadge("Checking...", COL_MUTED, HexColor(0x30363d, 0.60f));
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void RenderSidebar(cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##sidebar", ImVec2(-1.0f, -1.0f), true);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##search", "Search VIN, brand, model...",
                             g_searchBuf, sizeof(g_searchBuf));
    ImGui::PopStyleColor(4);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const int pendingCount = g_pendingMgr.pendingCount();
    const cw1::Role currentRole = CurrentUserRole();

    struct NavItem {
        const char* label;
        View view;
    };

    static const NavItem kNavItems[] = {
        {"  Dashboard", View::DASHBOARD},
        {"  + Add New Block", View::ADD_BLOCK},
        {"  Pending Approvals", View::PENDING_APPROVALS},
        {"  Global Chain", View::GLOBAL_CHAIN},
        {"  Audit Log", View::AUDIT_LOG},
        {"  Verify Integrity", View::INTEGRITY},
        {"  AI Assistant", View::AI_ASSISTANT},
    };

    for (const auto& item : kNavItems) {
        if (item.view == View::ADD_BLOCK && cw1::AccessControl::isReadOnly(currentRole)) {
            continue;
        }

        const bool active = (g_view == item.view);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button, COL_ACCENT_SOFT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x1f6feb, 0.35f));
            ImGui::PushStyleColor(ImGuiCol_Text, COL_TEXT);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, COL_BG_ELEV);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_TEXT);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

        if (item.view == View::PENDING_APPROVALS && pendingCount > 0) {
            char label[64];
            std::snprintf(label, sizeof(label), "  Pending Approvals (%d)", pendingCount);
            if (ImGui::Button(label, ImVec2(-1.0f, 34.0f))) {
                g_view = item.view;
                g_selectedVin.clear();
            }
        } else if (ImGui::Button(item.label, ImVec2(-1.0f, 34.0f))) {
            g_view = item.view;
            g_selectedVin.clear();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
    }

    if (cw1::AccessControl::canDelete(currentRole)) {
        const bool active = (g_view == View::DELETE_BLOCK);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button, HexColor(0x5d2227));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x7c2f35));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x94353d));
            ImGui::PushStyleColor(ImGuiCol_Text, COL_TEXT);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, HexColor(0x3d1519));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x5d2227));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x7c2f35));
            ImGui::PushStyleColor(ImGuiCol_Text, COL_RED);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        if (ImGui::Button("  Delete Block", ImVec2(-1.0f, 34.0f))) {
            g_view = View::DELETE_BLOCK;
            g_selectedVin.clear();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
    }

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button, HexColor(0x1a3a2a));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x238636));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x2ea043));
    ImGui::PushStyleColor(ImGuiCol_Text, COL_GREEN_BR);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    if (ImGui::Button("  Export PDF Report", ImVec2(-1.0f, 34.0f))) {
        std::time_t now = std::time(nullptr);
        char fileName[64];
        std::strftime(fileName, sizeof(fileName), "report_%Y%m%d_%H%M%S.pdf",
                      std::localtime(&now));
        const std::string directory = g_projectRoot + "/reports";
        const std::string path = directory + "/" + fileName;
        if (ExportBlockchainPDF(chain, g_fuelMgr, path)) {
            char toast[128];
            std::snprintf(toast, sizeof(toast), "PDF exported: reports/%s", fileName);
            PushToast(toast, COL_GREEN_BR);
            WriteAuditEvent(chain, cw1::AuditAction::PERSISTENCE_IO,
                            "PDF report exported to " + path);
        } else {
            PushToast("PDF export failed!", COL_RED);
        }
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (g_authMgr.isLoggedIn()) {
        ImGui::TextColored(COL_MUTED, "Logged in as:");
        ImGui::TextColored(COL_ACCENT, "%s", g_authMgr.currentUser().displayName.c_str());
        ImGui::TextColored(COL_VERY_MUTED, "(%s)",
                           cw1::roleToString(g_authMgr.currentUser().role).c_str());
        ImGui::Spacing();
        if (DrawDangerButton("  Logout", ImVec2(-1.0f, 30.0f))) {
            WriteAuditEvent(chain, cw1::AuditAction::USER_LOGOUT,
                            "User " + g_authMgr.currentUser().username + " logged out");
            g_authMgr.logout();
            g_showLoginScreen = true;
            g_view = View::DASHBOARD;
            g_loginUser[0] = '\0';
            g_loginPass[0] = '\0';
            g_loginError.clear();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const auto allVins = chain.getAllVins();
    ImGui::TextColored(COL_MUTED, "VEHICLES (%zu)", allVins.size());
    ImGui::Spacing();

    std::vector<std::string> displayVins;
    const std::string query(g_searchBuf);
    if (!query.empty()) {
        if (query != g_cachedSearchQuery) {
            g_cachedSearchQuery = query;
            g_cachedSearchVins.clear();

            try {
                g_lastSearchSeconds = cw1::measureSeconds([&]() {
                    const auto hits = chain.searchBySQL(query);
                    std::vector<std::string> seen;
                    for (const auto* block : hits) {
                        const std::string& vin = block->getRecord().vin;
                        if (std::find(seen.begin(), seen.end(), vin) == seen.end()) {
                            seen.push_back(vin);
                            g_cachedSearchVins.push_back(vin);
                        }
                    }
                });
            } catch (const std::exception& ex) {
                g_lastSearchSeconds = 0.0;
                g_cachedSearchVins.clear();
                ReportGuiException(chain, cw1::AuditAction::SEARCH_PERFORMED,
                                   "Search failed", ex);
            }
        }
        displayVins = g_cachedSearchVins;
    } else {
        g_cachedSearchQuery.clear();
        g_cachedSearchVins.clear();
        displayVins = allVins;
    }

    if (!query.empty()) {
        ImGui::TextColored(COL_VERY_MUTED, "Search operation took: %s s",
                           cw1::formatSeconds(g_lastSearchSeconds).c_str());
        ImGui::Spacing();
    }

    for (const auto& vin : displayVins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) {
            continue;
        }

        const auto& record = history.front()->getRecord();
        const bool selected = (vin == g_selectedVin);
        const cw1::BlockStage stage = chain.getLatestStage(vin);

        if (selected) {
            const ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                pos,
                ImVec2(pos.x + 3.0f, pos.y + 72.0f),
                ImGui::ColorConvertFloat4ToU32(COL_ACCENT));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Header, selected ? COL_BG_HOVER : COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, HexColor(0x1f6feb, 0.25f));

        char cardId[32];
        std::snprintf(cardId, sizeof(cardId), "##car_%s", vin.c_str());
        const bool clicked = ImGui::Selectable(cardId, selected, 0, ImVec2(-1.0f, 68.0f));
        ImGui::PopStyleColor(3);

        if (clicked) {
            g_selectedVin = vin;
            g_view = View::CAR_DETAIL;
        }

        const float topY = ImGui::GetCursorPos().y - 68.0f;
        const float offsetX = selected ? 22.0f : 16.0f;

        ImGui::SetCursorPos(ImVec2(offsetX, topY + 6.0f));
        ImGui::TextColored(selected ? COL_ACCENT : COL_TEXT, "%s", vin.c_str());

        ImGui::SetCursorPos(ImVec2(offsetX, topY + 24.0f));
        ImGui::TextColored(COL_MUTED, "%s %s - %s",
                           record.manufacturer.c_str(),
                           record.model.c_str(),
                           record.color.c_str());

        ImGui::SetCursorPos(ImVec2(offsetX, topY + 42.0f));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, StageColor(stage));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_MAIN);
        ImGui::ProgressBar(StageProgress(stage), ImVec2(150.0f, 8.0f), "");
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, " %s", cw1::stageToString(stage).c_str());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY());
        ImGui::Spacing();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void RenderToasts(float deltaTime) {
    g_toasts.erase(
        std::remove_if(g_toasts.begin(), g_toasts.end(),
                       [](const Toast& toast) { return toast.remainingSecs <= 0.0f; }),
        g_toasts.end());

    constexpr float kToastWidth = 340.0f;
    constexpr float kToastHeight = 44.0f;
    constexpr float kMargin = 20.0f;
    constexpr float kTopY = 70.0f;

    ImGuiIO& io = ImGui::GetIO();

    for (std::size_t i = 0; i < g_toasts.size(); ++i) {
        Toast& toast = g_toasts[i];
        float alpha = (toast.remainingSecs < 0.5f) ? (toast.remainingSecs / 0.5f) : 1.0f;
        if (alpha < 0.0f) {
            alpha = 0.0f;
        }

        const float posX = io.DisplaySize.x - kToastWidth - kMargin;
        const float posY = kTopY + static_cast<float>(i) * (kToastHeight + 8.0f);

        char windowName[32];
        std::snprintf(windowName, sizeof(windowName), "##toast%zu", i);

        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(kToastWidth, kToastHeight));
        ImGui::SetNextWindowBgAlpha(0.92f * alpha);

        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, toast.color);
        ImGui::PushStyleColor(ImGuiCol_Border, toast.color);
        if (ImGui::Begin(windowName, nullptr, flags)) {
            ImGui::SetCursorPos(ImVec2(12.0f, 12.0f));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha), "%s", toast.msg.c_str());
        }
        ImGui::End();
        ImGui::PopStyleColor(2);

        toast.remainingSecs -= deltaTime;
    }
}
