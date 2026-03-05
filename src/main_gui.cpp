// main_gui.cpp -- ImGui/GLFW/OpenGL3 GUI for the Car Warehouse Blockchain.
// GitHub Dark theme, 3-panel layout (header | sidebar | content area).
// Views: Dashboard, Car Detail, Add Block, Global Chain, Audit Log, Integrity.

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"

// =================================================================
//  SECTION 1: Color palette (GitHub Dark)
// =================================================================

/// Convert packed RGB hex to ImVec4 RGBA float.
static ImVec4 HexColor(uint32_t hex, float a = 1.0f) {
    return ImVec4(
        static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
        static_cast<float>((hex >>  8) & 0xFF) / 255.0f,
        static_cast<float>( hex        & 0xFF) / 255.0f, a);
}

static const ImVec4 COL_BG_MAIN    = HexColor(0x0d1117);  // main window
static const ImVec4 COL_BG_PANEL   = HexColor(0x161b22);  // panel / sidebar
static const ImVec4 COL_BG_CARD    = HexColor(0x21262d);  // cards / frames
static const ImVec4 COL_BG_HOVER   = HexColor(0x30363d);  // hover / borders
static const ImVec4 COL_TEXT       = HexColor(0xe6edf3);  // primary text
static const ImVec4 COL_MUTED      = HexColor(0x8b949e);  // secondary text
static const ImVec4 COL_VERY_MUTED = HexColor(0x484f58);  // very muted
static const ImVec4 COL_ACCENT     = HexColor(0x1f6feb);  // primary blue
static const ImVec4 COL_ACCENT_HO  = HexColor(0x388bfd);  // blue hover
static const ImVec4 COL_GREEN      = HexColor(0x238636);  // success / pass
static const ImVec4 COL_GREEN_BR   = HexColor(0x2ea043);  // green hover
static const ImVec4 COL_RED        = HexColor(0xda3633);  // error / fail
static const ImVec4 COL_YELLOW     = HexColor(0xd29922);  // warning / QC
static const ImVec4 COL_PURPLE     = HexColor(0x8957e5);  // hash values
static const ImVec4 COL_ORANGE     = HexColor(0xdb6d28);  // nonce / dispatch

// =================================================================
//  SECTION 2: GitHub Dark theme
// =================================================================

static void ApplyGitHubDarkTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 8.0f;  s.FrameRounding    = 6.0f;
    s.PopupRounding     = 8.0f;  s.TabRounding      = 6.0f;
    s.ScrollbarRounding = 6.0f;  s.GrabRounding     = 4.0f;
    s.ChildRounding     = 6.0f;
    s.FramePadding      = ImVec2(10.0f, 6.0f);
    s.ItemSpacing       = ImVec2(10.0f, 8.0f);
    s.WindowPadding     = ImVec2(16.0f, 16.0f);
    s.ItemInnerSpacing  = ImVec2(8.0f,  4.0f);
    s.ScrollbarSize     = 12.0f; s.GrabMinSize      = 8.0f;
    s.WindowBorderSize  = 1.0f;  s.FrameBorderSize  = 0.0f;
    s.ChildBorderSize   = 1.0f;  s.PopupBorderSize  = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                  = COL_TEXT;
    c[ImGuiCol_TextDisabled]          = COL_MUTED;
    c[ImGuiCol_WindowBg]              = COL_BG_MAIN;
    c[ImGuiCol_ChildBg]               = COL_BG_PANEL;
    c[ImGuiCol_PopupBg]               = COL_BG_PANEL;
    c[ImGuiCol_Border]                = COL_BG_HOVER;
    c[ImGuiCol_BorderShadow]          = ImVec4(0,0,0,0);
    c[ImGuiCol_FrameBg]               = COL_BG_CARD;
    c[ImGuiCol_FrameBgHovered]        = COL_BG_HOVER;
    c[ImGuiCol_FrameBgActive]         = HexColor(0x1f6feb, 0.35f);
    c[ImGuiCol_TitleBg]               = COL_BG_PANEL;
    c[ImGuiCol_TitleBgActive]         = COL_BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed]      = COL_BG_MAIN;
    c[ImGuiCol_MenuBarBg]             = COL_BG_PANEL;
    c[ImGuiCol_ScrollbarBg]           = COL_BG_MAIN;
    c[ImGuiCol_ScrollbarGrab]         = COL_VERY_MUTED;
    c[ImGuiCol_ScrollbarGrabHovered]  = COL_MUTED;
    c[ImGuiCol_ScrollbarGrabActive]   = COL_ACCENT;
    c[ImGuiCol_CheckMark]             = COL_ACCENT;
    c[ImGuiCol_SliderGrab]            = COL_ACCENT;
    c[ImGuiCol_SliderGrabActive]      = COL_ACCENT_HO;
    c[ImGuiCol_Button]                = COL_BG_CARD;
    c[ImGuiCol_ButtonHovered]         = COL_BG_HOVER;
    c[ImGuiCol_ButtonActive]          = HexColor(0x1f6feb, 0.4f);
    c[ImGuiCol_Header]                = COL_BG_CARD;
    c[ImGuiCol_HeaderHovered]         = COL_BG_HOVER;
    c[ImGuiCol_HeaderActive]          = HexColor(0x1f6feb, 0.4f);
    c[ImGuiCol_Separator]             = COL_BG_HOVER;
    c[ImGuiCol_SeparatorHovered]      = COL_MUTED;
    c[ImGuiCol_SeparatorActive]       = COL_ACCENT;
    c[ImGuiCol_ResizeGrip]            = COL_BG_HOVER;
    c[ImGuiCol_ResizeGripHovered]     = COL_MUTED;
    c[ImGuiCol_ResizeGripActive]      = COL_ACCENT;
    c[ImGuiCol_Tab]                   = COL_BG_PANEL;
    c[ImGuiCol_TabHovered]            = COL_BG_HOVER;
    c[ImGuiCol_TabSelected]           = COL_BG_CARD;
    c[ImGuiCol_TabSelectedOverline]   = COL_ACCENT;
    c[ImGuiCol_TabDimmed]             = COL_BG_PANEL;
    c[ImGuiCol_TabDimmedSelected]     = COL_BG_CARD;
    c[ImGuiCol_PlotLines]             = COL_ACCENT;
    c[ImGuiCol_PlotLinesHovered]      = COL_ACCENT_HO;
    c[ImGuiCol_PlotHistogram]         = COL_ACCENT;
    c[ImGuiCol_PlotHistogramHovered]  = COL_ACCENT_HO;
    c[ImGuiCol_TableHeaderBg]         = COL_BG_CARD;
    c[ImGuiCol_TableBorderStrong]     = COL_BG_HOVER;
    c[ImGuiCol_TableBorderLight]      = HexColor(0x21262d);
    c[ImGuiCol_TableRowBg]            = ImVec4(0,0,0,0);
    c[ImGuiCol_TableRowBgAlt]         = HexColor(0x161b22, 0.5f);
    c[ImGuiCol_TextSelectedBg]        = HexColor(0x1f6feb, 0.3f);
    c[ImGuiCol_DragDropTarget]        = COL_ACCENT;
    c[ImGuiCol_NavHighlight]          = COL_ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingDimBg]     = HexColor(0x0d1117, 0.7f);
    c[ImGuiCol_ModalWindowDimBg]      = HexColor(0x0d1117, 0.6f);
}

// =================================================================
//  SECTION 3: Application state
// =================================================================

enum class View { DASHBOARD, CAR_DETAIL, ADD_BLOCK, GLOBAL_CHAIN, AUDIT_LOG, INTEGRITY };

static View        g_view        = View::DASHBOARD;
static std::string g_selectedVin;
static char        g_searchBuf[256] = {};
static cw1::ValidationResult g_lastVerify = {false, "Not yet verified"};
static bool        g_verifyDone = false;

// Guard to prevent per-frame audit log spam.
// We only log a CHAIN_VIEWED event once when the user first switches to a view.
static View        g_lastLoggedView = View::DASHBOARD;
static std::string g_lastLoggedVin;

/// Log a CHAIN_VIEWED event only once per view switch (not every frame).
static void LogViewOnce(const cw1::Blockchain& chain, View currentView,
                        const std::string& vin, const std::string& details) {
    if (currentView != g_lastLoggedView || vin != g_lastLoggedVin) {
        chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED, details);
        g_lastLoggedView = currentView;
        g_lastLoggedVin  = vin;
    }
}

// Toast notification system
struct Toast { std::string msg; float remainingSecs; ImVec4 color; };
static std::vector<Toast> g_toasts;
static void PushToast(const std::string& msg, ImVec4 color, float dur = 3.0f) {
    g_toasts.push_back({msg, dur, color});
}

// Add-block form state
static int    g_formStage           = 0;
static char   g_formVin[64]         = {};
static char   g_formMfr[64]         = {};
static char   g_formModel[64]       = {};
static char   g_formColor[64]       = {};
static int    g_formYear            = 2025;
static char   g_formFactory[128]    = {};
static char   g_formWarehouse[64]   = {};
static char   g_formReceivedBy[64]  = {};
static char   g_formInspector[64]   = {};
static bool   g_formPassed          = true;
static char   g_formQcNotes[256]    = {};
static char   g_formDealerId[64]    = {};
static char   g_formDestination[64] = {};
static char   g_formTransport[64]   = {};
static char   g_formBuyerId[64]     = {};
static double g_formSalePrice       = 0.0;
static char   g_formWarranty[32]    = {};
static int    g_auditN              = 20;

// =================================================================
//  SECTION 4: Demo data (no cout, no sleep_for)
// =================================================================

static void loadDemoData(cw1::Blockchain& chain) {
    { cw1::CarRecord r;
      r.vin="VIN1001"; r.manufacturer="Perodua"; r.model="Axia";
      r.color="Silver"; r.productionYear=2024;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Shah Alam Plant";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-A1"; r.receivedBy="Ahmad bin Ismail";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-001"; r.passed=true; r.qcNotes="All systems nominal";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KB-01"; r.destination="Kota Bharu"; r.transportMode="Truck";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::CUSTOMER_SALE;
      r.buyerId="CUST-10201"; r.salePrice=38000.00; r.warrantyExpiry="2029-03-01";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1002"; r.manufacturer="Toyota"; r.model="Vios";
      r.color="White"; r.productionYear=2023;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Toyota Bukit Raja";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-B2"; r.receivedBy="Lim Wei Jie";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-003"; r.passed=true; r.qcNotes="Minor paint scratch - touched up";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KL-05"; r.destination="Kuala Lumpur"; r.transportMode="Truck";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1003"; r.manufacturer="Honda"; r.model="City";
      r.color="Blue"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Honda Pegoh, Melaka";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-C3"; r.receivedBy="Raj Kumar";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-007"; r.passed=false;
      r.qcNotes="Brake pad below spec - pending replacement";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1004"; r.manufacturer="Proton"; r.model="X50";
      r.color="Red"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Proton Tanjung Malim";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-D4"; r.receivedBy="Nurul Aina";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1005"; r.manufacturer="Perodua"; r.model="Myvi";
      r.color="Black"; r.productionYear=2025;
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Perodua Rawang";
      chain.addBlock(r); }
}

// =================================================================
//  SECTION 5: Helper utilities
// =================================================================

static ImVec4 StageColor(cw1::BlockStage stage) {
    switch (stage) {
        case cw1::BlockStage::PRODUCTION:       return COL_MUTED;
        case cw1::BlockStage::WAREHOUSE_INTAKE: return COL_ACCENT;
        case cw1::BlockStage::QUALITY_CHECK:    return COL_YELLOW;
        case cw1::BlockStage::DEALER_DISPATCH:  return COL_ORANGE;
        case cw1::BlockStage::CUSTOMER_SALE:    return COL_GREEN;
        default:                                return COL_TEXT;
    }
}

static float StageProgress(cw1::BlockStage stage) {
    return (static_cast<float>(stage) + 1.0f) / 5.0f;
}

static std::string Truncate(const std::string& s, size_t maxLen) {
    return (s.size() <= maxLen) ? s : s.substr(0, maxLen) + "...";
}

static std::string FormatMYR(double v) {
    std::ostringstream ss;
    ss << "MYR " << std::fixed << std::setprecision(2) << v;
    return ss.str();
}

// =================================================================
//  SECTION 6: Header bar
// =================================================================

static void RenderHeader(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 12.0f));
    ImGui::BeginChild("##header", ImVec2(-1.0f, 54.0f), true);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    ImGui::TextColored(COL_ACCENT, "%s", "* ");
    ImGui::SameLine();
    ImGui::Text("Car Warehouse Blockchain");

    float statsWidth = 340.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - statsWidth
                    + ImGui::GetCursorPosX() - 16.0f);

    char badge[64];
    snprintf(badge, sizeof(badge), "  %zu blocks  ", chain.totalBlocks());
    ImGui::TextColored(COL_ACCENT, "%s", badge);
    ImGui::SameLine();
    snprintf(badge, sizeof(badge), "  %zu cars  ", chain.getAllVins().size());
    ImGui::TextColored(COL_GREEN, "%s", badge);
    ImGui::SameLine();
    if (g_verifyDone) {
        if (g_lastVerify.ok)
            ImGui::TextColored(COL_GREEN, "  Verified  ");
        else
            ImGui::TextColored(COL_RED, "  Invalid  ");
    } else {
        ImGui::TextColored(COL_MUTED, "  Checking...  ");
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// =================================================================
//  SECTION 7: Left sidebar
// =================================================================

static void RenderSidebar(const cw1::Blockchain& chain) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##sidebar", ImVec2(280.0f, -1.0f), true);

    // Search box
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##search", "Search VIN, brand, model...",
                             g_searchBuf, sizeof(g_searchBuf));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Navigation buttons
    struct NavItem { const char* label; View view; };
    static const NavItem navItems[] = {
        { "  Dashboard",        View::DASHBOARD    },
        { "  + Add New Block",  View::ADD_BLOCK     },
        { "  Global Chain",     View::GLOBAL_CHAIN  },
        { "  Audit Log",        View::AUDIT_LOG     },
        { "  Verify Integrity", View::INTEGRITY     },
    };

    for (const auto& item : navItems) {
        bool active = (g_view == item.view);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(1,1,1,1));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        }
        if (ImGui::Button(item.label, ImVec2(-1.0f, 36.0f))) {
            g_view = item.view;
            g_selectedVin.clear();
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Car list header
    auto allVins = chain.getAllVins();
    ImGui::TextColored(COL_MUTED, "VEHICLES (%zu)", allVins.size());
    ImGui::Spacing();

    // Build filtered list
    std::vector<std::string> displayVins;
    if (g_searchBuf[0]) {
        const auto hits = chain.searchGeneral(std::string(g_searchBuf));
        std::vector<std::string> seen;
        for (const auto* blk : hits) {
            const auto& v = blk->getRecord().vin;
            if (std::find(seen.begin(), seen.end(), v) == seen.end()) {
                seen.push_back(v);
                displayVins.push_back(v);
            }
        }
    } else {
        displayVins = allVins;
    }

    for (const auto& vin : displayVins) {
        const auto history = chain.getCarHistory(vin);
        if (history.empty()) continue;
        const auto& rec    = history.front()->getRecord();
        bool        sel    = (vin == g_selectedVin);
        cw1::BlockStage st = chain.getLatestStage(vin);

        // Blue left border for selected card
        if (sel) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                p, ImVec2(p.x + 3.0f, p.y + 72.0f),
                ImGui::ColorConvertFloat4ToU32(COL_ACCENT));
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_Header,        sel ? COL_BG_HOVER : COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.25f));

        char cardId[32]; snprintf(cardId, sizeof(cardId), "##car_%s", vin.c_str());
        bool clicked = ImGui::Selectable(cardId, sel, 0, ImVec2(-1.0f, 68.0f));
        ImGui::PopStyleColor(3);

        if (clicked) { g_selectedVin = vin; g_view = View::CAR_DETAIL; }

        float  topY = ImGui::GetCursorPos().y - 68.0f;
        float  offX = sel ? 22.0f : 16.0f;

        ImGui::SetCursorPos(ImVec2(offX, topY + 6.0f));
        ImGui::TextColored(sel ? COL_ACCENT : COL_TEXT, "%s", vin.c_str());

        ImGui::SetCursorPos(ImVec2(offX, topY + 24.0f));
        ImGui::TextColored(COL_MUTED, "%s %s - %s",
            rec.manufacturer.c_str(), rec.model.c_str(), rec.color.c_str());

        ImGui::SetCursorPos(ImVec2(offX, topY + 42.0f));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, StageColor(st));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,       COL_BG_MAIN);
        ImGui::ProgressBar(StageProgress(st), ImVec2(150.0f, 8.0f), "");
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, " %s", cw1::stageToString(st).c_str());

        ImGui::SetCursorPosY(ImGui::GetCursorPosY());
        ImGui::Spacing();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}


// =================================================================

// =================================================================
//  SECTION 8: Dashboard view
// =================================================================

static void RenderDashboard(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    size_t totalBlocks = chain.totalBlocks();
    size_t totalCars   = chain.getAllVins().size();

    // ── Stat cards ────────────────────────────────────────────────
    if (ImGui::BeginTable("##stats", 4,
            ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoPadOuterX,
            ImVec2(-1, 90))) {
        ImGui::TableNextRow();

        auto StatCard = [](const char* value, const char* label, ImVec4 col) {
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
            ImGui::BeginChild(label, ImVec2(-1, 80), true);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
            ImGui::TextColored(col, "%s", value);
            ImGui::TextColored(COL_MUTED, "%s", label);
            ImGui::EndChild();
            ImGui::PopStyleColor();
        };

        char buf1[32], buf2[32], buf4[32];
        snprintf(buf1, sizeof(buf1), "%zu", totalBlocks);
        snprintf(buf2, sizeof(buf2), "%zu", totalCars);
        snprintf(buf4, sizeof(buf4), "%zu", chain.getAuditLog().size());

        StatCard(buf1, "Total Blocks",    COL_ACCENT);
        StatCard(buf2, "Unique Vehicles", COL_GREEN_BR);
        StatCard(g_verifyDone && g_lastVerify.ok ? "VERIFIED" : (g_verifyDone ? "TAMPERED" : "N/A"),
                 "Integrity",
                 g_verifyDone && g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
        StatCard(buf4, "Audit Events",    COL_PURPLE);

        ImGui::EndTable();
    }

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "RECENT BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    // ── Recent blocks table ────────────────────────────────────────
    ImGuiTableFlags tflags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                             ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##recent", 6, tflags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        // Most-recent first
        size_t count = blocks.size();
        for (size_t ri = 0; ri < count; ++ri) {
            size_t i = count - 1 - ri;
            const cw1::Block& b = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", b.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), 12).c_str());

            ImGui::TableNextColumn();
            char selId[80];
            snprintf(selId, sizeof(selId), "%s##rv%zu", rec.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = rec.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", rec.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(rec.stage), "%s",
                               cw1::stageToString(rec.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", b.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
}

// =================================================================
//  SECTION 9: Car Detail view
// =================================================================

static void RenderCarDetail(const cw1::Blockchain& chain) {
    if (ImGui::Button("< Back")) {
        g_view = View::DASHBOARD;
        return;
    }
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " / ");
    ImGui::SameLine();
    ImGui::TextColored(COL_ACCENT, "%s", g_selectedVin.c_str());

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    if (g_selectedVin.empty() || !chain.hasVin(g_selectedVin)) {
        ImGui::TextColored(COL_RED, "VIN not found.");
        return;
    }

    auto history = chain.getCarHistory(g_selectedVin);
    if (history.empty()) {
        ImGui::TextColored(COL_MUTED, "No blocks for this VIN.");
        return;
    }

    // Car header from first block
    const cw1::CarRecord& first = history.front()->getRecord();
    ImGui::TextColored(COL_TEXT, "%s %s  (%d)   %s",
        first.manufacturer.c_str(), first.model.c_str(),
        first.productionYear, first.color.c_str());
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "LIFECYCLE TIMELINE  (%zu block(s))", history.size());
    ImGui::Separator(); ImGui::Spacing();

    for (size_t idx = 0; idx < history.size(); ++idx) {
        const cw1::Block& blk = *history[idx];
        const cw1::CarRecord& rec = blk.getRecord();

        char hdrLabel[128];
        snprintf(hdrLabel, sizeof(hdrLabel), "Block #%zu  |  %s##det%zu",
                 blk.getIndex(), cw1::stageToString(rec.stage).c_str(), idx);

        ImGui::PushStyleColor(ImGuiCol_Header,        COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.2f));
        bool open = ImGui::CollapsingHeader(hdrLabel,
                        idx == history.size() - 1
                            ? ImGuiTreeNodeFlags_DefaultOpen
                            : ImGuiTreeNodeFlags_None);
        ImGui::PopStyleColor(3);
        if (!open) continue;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        // Use hdrLabel as unique child ID
        ImGui::BeginChild(hdrLabel, ImVec2(-1, 0), true);

        // Stage badge + timestamp
        ImGui::TextColored(StageColor(rec.stage), "[%s]",
                           cw1::stageToString(rec.stage).c_str());
        ImGui::SameLine();
        ImGui::TextColored(COL_MUTED, "  %s", blk.getTimestamp().c_str());

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // Helper: label + value on one line
        auto Field = [](const char* lbl, const char* val, ImVec4 col) {
            ImGui::TextColored(COL_MUTED, "%-20s", lbl);
            ImGui::SameLine();
            ImGui::TextColored(col, "%s", val);
        };

        Field("Hash:",          blk.getCurrentHash().c_str(),  COL_PURPLE);
        Field("Prev Hash:",     blk.getPreviousHash().c_str(), COL_PURPLE);

        char nonceBuf[32];
        snprintf(nonceBuf, sizeof(nonceBuf), "%llu",
                 static_cast<unsigned long long>(blk.getNonce()));
        Field("Nonce:",         nonceBuf, COL_ORANGE);

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Record Fields");
        ImGui::Separator(); ImGui::Spacing();

        Field("VIN:",           rec.vin.c_str(),          COL_TEXT);
        Field("Manufacturer:",  rec.manufacturer.c_str(), COL_TEXT);
        Field("Model:",         rec.model.c_str(),        COL_TEXT);
        Field("Color:",         rec.color.c_str(),        COL_TEXT);

        char yearBuf[16];
        snprintf(yearBuf, sizeof(yearBuf), "%d", rec.productionYear);
        Field("Production Year:", yearBuf, COL_TEXT);
        Field("Stage:",           cw1::stageToString(rec.stage).c_str(),
                                  StageColor(rec.stage));

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Stage Details");
        ImGui::Separator(); ImGui::Spacing();

        switch (rec.stage) {
        case cw1::BlockStage::PRODUCTION:
            Field("Factory:",   rec.factoryLocation.c_str(), COL_TEXT);
            break;
        case cw1::BlockStage::WAREHOUSE_INTAKE:
            Field("Warehouse:", rec.warehouseLocation.c_str(), COL_TEXT);
            Field("Received By:",rec.receivedBy.c_str(),       COL_TEXT);
            break;
        case cw1::BlockStage::QUALITY_CHECK:
            Field("Inspector:", rec.inspectorId.c_str(), COL_TEXT);
            Field("Passed:",    rec.passed ? "Yes" : "No",
                                rec.passed ? COL_GREEN_BR : COL_RED);
            Field("QC Notes:",  rec.qcNotes.c_str(), COL_TEXT);
            break;
        case cw1::BlockStage::DEALER_DISPATCH:
            Field("Dealer ID:", rec.dealerId.c_str(),      COL_TEXT);
            Field("Destination:",rec.destination.c_str(),  COL_TEXT);
            Field("Transport:", rec.transportMode.c_str(), COL_ORANGE);
            break;
        case cw1::BlockStage::CUSTOMER_SALE: {
            Field("Buyer ID:",  rec.buyerId.c_str(), COL_TEXT);
            char priceBuf[48];
            snprintf(priceBuf, sizeof(priceBuf), "%s", FormatMYR(rec.salePrice).c_str());
            Field("Sale Price:", priceBuf, COL_GREEN_BR);
            Field("Warranty:",  rec.warrantyExpiry.c_str(), COL_TEXT);
            break;
        }
        }

        ImGui::Spacing();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }
    LogViewOnce(chain, View::CAR_DETAIL, g_selectedVin,
                "Car detail viewed: " + g_selectedVin);
}

// =================================================================
//  SECTION 10: Add Block form
// =================================================================

static const char* k_stageNames[] = {
    "PRODUCTION", "WAREHOUSE_INTAKE", "QUALITY_CHECK",
    "DEALER_DISPATCH", "CUSTOMER_SALE"
};

static void RenderAddBlock(cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Add Block to Chain");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "COMMON FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::SetNextItemWidth(240);
    ImGui::Combo("Stage##addstage", &g_formStage, k_stageNames, 5);
    ImGui::Spacing();

    // Helper: label + InputText on one row
    auto LabelText = [](const char* lbl, char* buf, size_t sz) {
        ImGui::TextColored(COL_MUTED, "%-18s", lbl);
        ImGui::SameLine();
        char id[64]; snprintf(id, sizeof(id), "##add_%s", lbl);
        ImGui::SetNextItemWidth(300);
        ImGui::InputText(id, buf, sz);
    };

    LabelText("VIN",          g_formVin,   sizeof(g_formVin));
    // VIN status badge
    ImGui::SameLine(0, 12);
    if (g_formVin[0] != '\0') {
        if (chain.hasVin(std::string(g_formVin)))
            ImGui::TextColored(COL_GREEN_BR, "[+ Existing VIN]");
        else
            ImGui::TextColored(COL_ACCENT,   "[* New VIN]");
    }

    LabelText("Manufacturer", g_formMfr,   sizeof(g_formMfr));
    LabelText("Model",        g_formModel, sizeof(g_formModel));
    LabelText("Color",        g_formColor, sizeof(g_formColor));

    ImGui::TextColored(COL_MUTED, "%-18s", "Production Year");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##add_year", &g_formYear);

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "STAGE-SPECIFIC FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    switch (g_formStage) {
    case 0: // PRODUCTION
        LabelText("Factory Location", g_formFactory, sizeof(g_formFactory));
        break;
    case 1: // WAREHOUSE_INTAKE
        LabelText("Warehouse",  g_formWarehouse,   sizeof(g_formWarehouse));
        LabelText("Received By",g_formReceivedBy,  sizeof(g_formReceivedBy));
        break;
    case 2: // QUALITY_CHECK
        LabelText("Inspector ID", g_formInspector, sizeof(g_formInspector));
        ImGui::TextColored(COL_MUTED, "%-18s", "Passed");
        ImGui::SameLine();
        ImGui::Checkbox("##add_passed", &g_formPassed);
        LabelText("QC Notes", g_formQcNotes, sizeof(g_formQcNotes));
        break;
    case 3: // DEALER_DISPATCH
        LabelText("Dealer ID",    g_formDealerId,   sizeof(g_formDealerId));
        LabelText("Destination",  g_formDestination,sizeof(g_formDestination));
        LabelText("Transport",    g_formTransport,  sizeof(g_formTransport));
        break;
    case 4: // CUSTOMER_SALE
        LabelText("Buyer ID",     g_formBuyerId, sizeof(g_formBuyerId));
        ImGui::TextColored(COL_MUTED, "%-18s", "Sale Price (MYR)");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputDouble("##add_price", &g_formSalePrice, 1000.0, 10000.0, "%.2f");
        LabelText("Warranty Expiry", g_formWarranty, sizeof(g_formWarranty));
        break;
    }

    ImGui::Spacing(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    bool submit = ImGui::Button("  Add Block  ", ImVec2(150, 36));
    ImGui::PopStyleColor(3);

    if (submit) {
        cw1::CarRecord r;
        r.vin            = g_formVin;
        r.manufacturer   = g_formMfr;
        r.model          = g_formModel;
        r.color          = g_formColor;
        r.productionYear = g_formYear;
        r.stage          = static_cast<cw1::BlockStage>(g_formStage);

        switch (g_formStage) {
        case 0:
            r.factoryLocation  = g_formFactory;
            break;
        case 1:
            r.warehouseLocation = g_formWarehouse;
            r.receivedBy        = g_formReceivedBy;
            break;
        case 2:
            r.inspectorId = g_formInspector;
            r.passed      = g_formPassed;
            r.qcNotes     = g_formQcNotes;
            break;
        case 3:
            r.dealerId      = g_formDealerId;
            r.destination   = g_formDestination;
            r.transportMode = g_formTransport;
            break;
        case 4:
            r.buyerId        = g_formBuyerId;
            r.salePrice      = g_formSalePrice;
            r.warrantyExpiry = g_formWarranty;
            break;
        }

        chain.addBlock(r);
        PushToast("Block #" + std::to_string(chain.totalBlocks() - 1) +
                  " added for " + std::string(g_formVin),
                  COL_GREEN_BR);

        // Clear all form buffers
        g_formVin[0]    = '\0'; g_formMfr[0]         = '\0';
        g_formModel[0]  = '\0'; g_formColor[0]        = '\0';
        g_formYear      = 2025;
        g_formFactory[0]= '\0'; g_formWarehouse[0]    = '\0';
        g_formReceivedBy[0]= '\0'; g_formInspector[0] = '\0';
        g_formPassed    = true;  g_formQcNotes[0]     = '\0';
        g_formDealerId[0]= '\0'; g_formDestination[0] = '\0';
        g_formTransport[0]= '\0'; g_formBuyerId[0]    = '\0';
        g_formSalePrice = 0.0;   g_formWarranty[0]    = '\0';
    }
}

// =================================================================
//  SECTION 11: Global Chain view
// =================================================================

static void RenderGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();

    ImGui::TextColored(COL_ACCENT, "Global Chain");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu block(s))", blocks.size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGuiTableFlags tf = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##global", 8, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Prev Hash",    ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Nonce",        ImGuiTableColumnFlags_WidthFixed,   90);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < blocks.size(); ++i) {
            const cw1::Block& b   = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", b.getIndex());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", b.getCurrentHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getPreviousHash(), 12).c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "%s", b.getPreviousHash().c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            char selId[80];
            snprintf(selId, sizeof(selId), "%s##gc%zu", rec.vin.c_str(), i);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = rec.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", rec.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(rec.stage), "%s",
                               cw1::stageToString(rec.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ORANGE, "%llu",
                               static_cast<unsigned long long>(b.getNonce()));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", b.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
    LogViewOnce(chain, View::GLOBAL_CHAIN, "",
                "Global chain viewed");
}

// =================================================================
//  SECTION 12: Audit Log view
// =================================================================

static void RenderAuditLog(const cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Audit Log");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu total event(s))", chain.getAuditLog().size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Show last");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##auditn", &g_auditN);
    if (g_auditN < 1)  g_auditN = 1;
    if (g_auditN > 500) g_auditN = 500;
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, "entries");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // Retrieve via Array of Pointers pattern
    std::size_t outCount = 0;
    const cw1::AuditEntry** entries =
        chain.getAuditLog().getRecentEntries(
            static_cast<std::size_t>(g_auditN), outCount);

    auto ActionColor = [](cw1::AuditAction act) -> ImVec4 {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return COL_GREEN_BR;
        case cw1::AuditAction::INTEGRITY_CHECK:  return COL_YELLOW;
        case cw1::AuditAction::SEARCH_PERFORMED: return COL_ACCENT;
        case cw1::AuditAction::CHAIN_VIEWED:     return COL_PURPLE;
        default:                                  return COL_TEXT;
        }
    };
    auto ActionName = [](cw1::AuditAction act) -> const char* {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case cw1::AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case cw1::AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case cw1::AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        default:                                  return "UNKNOWN";
        }
    };

    ImGuiTableFlags tf = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##auditlog", 3, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Action",    ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Details",   ImGuiTableColumnFlags_WidthStretch, 4.0f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < outCount; ++i) {
            const cw1::AuditEntry* e = entries[i];
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", e->timestamp.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(ActionColor(e->action), "%s", ActionName(e->action));

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_TEXT, "%s", e->details.c_str());
        }
        ImGui::EndTable();
    }

    // Caller must delete[] the raw array (Array of Pointers pattern)
    delete[] entries;
}

// =================================================================
//  SECTION 13: Integrity check view
// =================================================================

static void RenderIntegrity(cw1::Blockchain& chain) {
    ImGui::TextColored(COL_ACCENT, "Blockchain Integrity");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    if (ImGui::Button("  Run Verification  ", ImVec2(200, 36))) {
        g_lastVerify = chain.verifyIntegrity();
        g_verifyDone = true;
        PushToast(g_lastVerify.ok ? "Chain VERIFIED OK" : "Chain TAMPERED!",
                  g_lastVerify.ok ? COL_GREEN_BR : COL_RED);
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing(); ImGui::Spacing();

    if (!g_verifyDone) {
        ImGui::TextColored(COL_MUTED, "Press Run Verification to check the chain.");
        return;
    }

    // Large PASS / FAIL banner
    ImVec4 bannerCol  = g_lastVerify.ok ? COL_GREEN : COL_RED;
    const char* bannerTxt = g_lastVerify.ok ? "VERIFIED" : "TAMPERED";

    ImGui::PushStyleColor(ImGuiCol_ChildBg, bannerCol);
    ImGui::BeginChild("##intbanner", ImVec2(-1, 80), true);
    ImGui::SetCursorPos(ImVec2(20, 18));
    ImGui::SetWindowFontScale(2.0f);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", bannerTxt);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "Result: ");
    ImGui::SameLine();
    ImGui::TextWrapped("%s", g_lastVerify.message.c_str());

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    size_t total = chain.totalBlocks();
    size_t cars  = chain.getAllVins().size();

    ImGui::TextColored(COL_MUTED, "Total blocks:    ");
    ImGui::SameLine(); ImGui::TextColored(COL_TEXT, "%zu", total);

    ImGui::TextColored(COL_MUTED, "Unique vehicles: ");
    ImGui::SameLine(); ImGui::TextColored(COL_TEXT, "%zu", cars);

    if (total > 0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "Block range: 0 .. %zu", total - 1);
    }
}

// =================================================================
//  SECTION 14: Toast renderer
// =================================================================

static void RenderToasts(float deltaTime) {
    // Remove expired toasts first
    g_toasts.erase(
        std::remove_if(g_toasts.begin(), g_toasts.end(),
                       [](const Toast& t) { return t.remainingSecs <= 0.0f; }),
        g_toasts.end());

    const float toastW = 340.0f;
    const float toastH = 44.0f;
    const float margin = 20.0f;
    const float topY   = 70.0f;

    ImGuiIO& io = ImGui::GetIO();

    for (size_t i = 0; i < g_toasts.size(); ++i) {
        Toast& t = g_toasts[i];

        // Alpha fade-out in last 0.5 s
        float alpha = (t.remainingSecs < 0.5f) ? (t.remainingSecs / 0.5f) : 1.0f;
        if (alpha < 0.0f) alpha = 0.0f;

        float posX = io.DisplaySize.x - toastW - margin;
        float posY = topY + static_cast<float>(i) * (toastH + 8.0f);

        char wname[32];
        snprintf(wname, sizeof(wname), "##toast%zu", i);

        ImGui::SetNextWindowPos(ImVec2(posX, posY));
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH));
        ImGui::SetNextWindowBgAlpha(0.92f * alpha);

        ImGuiWindowFlags wf =
            ImGuiWindowFlags_NoDecoration       |
            ImGuiWindowFlags_NoMove             |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNav              |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, t.color);
        ImGui::PushStyleColor(ImGuiCol_Border,   t.color);
        if (ImGui::Begin(wname, nullptr, wf)) {
            ImGui::SetCursorPos(ImVec2(12.0f, 12.0f));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha),
                               "%s", t.msg.c_str());
        }
        ImGui::End();
        ImGui::PopStyleColor(2);

        t.remainingSecs -= deltaTime;
    }
}

// =================================================================
//  SECTION 15: main()
// =================================================================

int main() {
    // ── 1. GLFW ───────────────────────────────────────────────────
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1400, 900, "Car Warehouse Blockchain System", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // ── 2. ImGui ──────────────────────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ApplyGitHubDarkTheme();

    // ── 3. Font (fallback to default if file absent) ──────────────
    {
        const char* fontPath = "fonts/JetBrainsMono-Regular.ttf";
        FILE* fp = fopen(fontPath, "rb");
        if (fp) {
            fclose(fp);
            io.Fonts->AddFontFromFileTTF(fontPath, 16.0f);
        } else {
            io.Fonts->AddFontDefault();
        }
    }

    // ── 4. Blockchain + demo data ─────────────────────────────────
    cw1::Blockchain chain;
    loadDemoData(chain);
    g_lastVerify = chain.verifyIntegrity();
    g_verifyDone = true;

    // ── 5. Main loop ──────────────────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full-screen root window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_MAIN);
        ImGui::Begin("##root", nullptr,
            ImGuiWindowFlags_NoDecoration          |
            ImGuiWindowFlags_NoMove                |
            ImGuiWindowFlags_NoResize              |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar           |
            ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleColor();

        RenderHeader(chain);
        ImGui::Spacing();

        // Sidebar
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##sbwrap", ImVec2(280, -1), false);
        ImGui::PopStyleColor();
        RenderSidebar(chain);
        ImGui::EndChild();

        ImGui::SameLine();

        // Content area
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_MAIN);
        ImGui::BeginChild("##content", ImVec2(-1, -1), false,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PopStyleColor();
        ImGui::Spacing(); ImGui::Spacing();

        switch (g_view) {
        case View::DASHBOARD:    RenderDashboard(chain);    break;
        case View::CAR_DETAIL:   RenderCarDetail(chain);    break;
        case View::ADD_BLOCK:    RenderAddBlock(chain);     break;
        case View::GLOBAL_CHAIN: RenderGlobalChain(chain);  break;
        case View::AUDIT_LOG:    RenderAuditLog(chain);     break;
        case View::INTEGRITY:    RenderIntegrity(chain);    break;
        }

        ImGui::EndChild();
        ImGui::End();

        // Toast overlay (rendered after main window)
        RenderToasts(io.DeltaTime);

        // ── Draw ──────────────────────────────────────────────────
        ImGui::Render();
        int dispW, dispH;
        glfwGetFramebufferSize(window, &dispW, &dispH);
        glViewport(0, 0, dispW, dispH);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);  // #0d1117
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // ── 6. Cleanup ────────────────────────────────────────────────
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
