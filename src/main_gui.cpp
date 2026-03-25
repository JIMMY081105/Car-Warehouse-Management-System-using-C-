#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "blockchain/BlockFormatter.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "utils/OperationTimer.hpp"






static ImVec4 HexColor(uint32_t hex, float a = 1.0f) {
    return ImVec4(
        static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
        static_cast<float>((hex >>  8) & 0xFF) / 255.0f,
        static_cast<float>( hex        & 0xFF) / 255.0f, a);
}

static const ImVec4 COL_BG_MAIN    = HexColor(0x0d1117);  
static const ImVec4 COL_BG_PANEL   = HexColor(0x161b22);  
static const ImVec4 COL_BG_CARD    = HexColor(0x21262d);  
static const ImVec4 COL_BG_ELEV    = HexColor(0x1b222c);  
static const ImVec4 COL_BG_HOVER   = HexColor(0x30363d);  
static const ImVec4 COL_TEXT       = HexColor(0xe6edf3);  
static const ImVec4 COL_MUTED      = HexColor(0x8b949e);  
static const ImVec4 COL_VERY_MUTED = HexColor(0x484f58);  
static const ImVec4 COL_ACCENT     = HexColor(0x1f6feb);  
static const ImVec4 COL_ACCENT_HO  = HexColor(0x388bfd);  
static const ImVec4 COL_ACCENT_SOFT = HexColor(0x1f6feb, 0.20f);
static const ImVec4 COL_GREEN      = HexColor(0x238636);  
static const ImVec4 COL_GREEN_BR   = HexColor(0x2ea043);  
static const ImVec4 COL_RED        = HexColor(0xda3633);  
static const ImVec4 COL_YELLOW     = HexColor(0xd29922);  
static const ImVec4 COL_PURPLE     = HexColor(0x8957e5);  
static const ImVec4 COL_ORANGE     = HexColor(0xdb6d28);  
static const ImVec4 COL_BORDER_SOFT = HexColor(0x30363d, 0.80f);


static ImFont* g_fontBody = nullptr;
static ImFont* g_fontSection = nullptr;
static ImFont* g_fontTitle = nullptr;





static void ApplyGitHubDarkTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 10.0f;  s.FrameRounding    = 7.0f;
    s.PopupRounding     = 8.0f;   s.TabRounding      = 7.0f;
    s.ScrollbarRounding = 8.0f;   s.GrabRounding     = 6.0f;
    s.ChildRounding     = 8.0f;
    s.FramePadding      = ImVec2(10.0f, 7.0f);
    s.ItemSpacing       = ImVec2(10.0f, 10.0f);
    s.WindowPadding     = ImVec2(16.0f, 16.0f);
    s.ItemInnerSpacing  = ImVec2(8.0f,  6.0f);
    s.CellPadding       = ImVec2(10.0f, 8.0f);
    s.ScrollbarSize     = 12.0f;  s.GrabMinSize      = 8.0f;
    s.WindowBorderSize  = 1.0f;   s.FrameBorderSize  = 1.0f;
    s.ChildBorderSize   = 1.0f;   s.PopupBorderSize  = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                  = COL_TEXT;
    c[ImGuiCol_TextDisabled]          = COL_MUTED;
    c[ImGuiCol_WindowBg]              = COL_BG_MAIN;
    c[ImGuiCol_ChildBg]               = COL_BG_PANEL;
    c[ImGuiCol_PopupBg]               = COL_BG_PANEL;
    c[ImGuiCol_Border]                = COL_BORDER_SOFT;
    c[ImGuiCol_BorderShadow]          = ImVec4(0,0,0,0);
    c[ImGuiCol_FrameBg]               = COL_BG_ELEV;
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
    c[ImGuiCol_Button]                = COL_BG_ELEV;
    c[ImGuiCol_ButtonHovered]         = COL_BG_HOVER;
    c[ImGuiCol_ButtonActive]          = COL_ACCENT_SOFT;
    c[ImGuiCol_Header]                = COL_BG_CARD;
    c[ImGuiCol_HeaderHovered]         = COL_BG_HOVER;
    c[ImGuiCol_HeaderActive]          = COL_ACCENT_SOFT;
    c[ImGuiCol_Separator]             = COL_BORDER_SOFT;
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
    c[ImGuiCol_TableHeaderBg]         = COL_BG_ELEV;
    c[ImGuiCol_TableBorderStrong]     = COL_BORDER_SOFT;
    c[ImGuiCol_TableBorderLight]      = HexColor(0x30363d, 0.55f);
    c[ImGuiCol_TableRowBg]            = ImVec4(0,0,0,0);
    c[ImGuiCol_TableRowBgAlt]         = HexColor(0x161b22, 0.78f);
    c[ImGuiCol_TextSelectedBg]        = HexColor(0x1f6feb, 0.3f);
    c[ImGuiCol_DragDropTarget]        = COL_ACCENT;
    c[ImGuiCol_NavHighlight]          = COL_ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingDimBg]     = HexColor(0x0d1117, 0.7f);
    c[ImGuiCol_ModalWindowDimBg]      = HexColor(0x0d1117, 0.6f);
}





enum class View { DASHBOARD, CAR_DETAIL, ADD_BLOCK, GLOBAL_CHAIN, AUDIT_LOG, INTEGRITY, DELETE_BLOCK };

static View        g_view        = View::DASHBOARD;
static std::string g_selectedVin;
static char        g_searchBuf[256] = {};
static cw1::ValidationResult g_lastVerify = {false, "Not yet verified", {}};
static bool        g_verifyDone = false;
static double      g_lastVerifySeconds = 0.0;
static double      g_lastAddBlockSeconds = 0.0;
static double      g_lastSearchSeconds = 0.0;


static int    g_deleteBlockIndex  = -1;
static double g_lastDeleteSeconds = 0.0;


static std::string              g_cachedSearchQuery;
static std::vector<std::string> g_cachedSearchVins;



static View        g_lastLoggedView = View::DASHBOARD;
static std::string g_lastLoggedVin;


static void LogViewOnce(const cw1::Blockchain& chain, View currentView,
                        const std::string& vin, const std::string& details) {
    if (currentView != g_lastLoggedView || vin != g_lastLoggedVin) {
        chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED, details);
        g_lastLoggedView = currentView;
        g_lastLoggedVin  = vin;
    }
}


struct Toast { std::string msg; float remainingSecs; ImVec4 color; };
static std::vector<Toast> g_toasts;
static void PushToast(const std::string& msg, ImVec4 color, float dur = 3.0f) {
    g_toasts.push_back({msg, dur, color});
}


static int    g_formStage           = 0;
static char   g_formVin[64]         = {};
static int    g_formMfrIndex        = 0;
static int    g_formModelIndex      = 0;
static int    g_formBranchIndex     = 0;
static int    g_formColorIndex      = 0;
static int    g_formYear            = 2025;
static char   g_formFactory[128]    = {};
static char   g_formWarehouse[64]   = {};
static char   g_formReceivedBy[64]  = {};
static char   g_formInspector[64]   = {};
static bool   g_formPassed          = true;
static char   g_formQcNotes[256]    = {};
static char   g_formDealerId[64]    = {};
static char   g_formDestination[64] = {};
static int    g_formTransportIndex  = 0;
static char   g_formBuyerId[64]     = {};
static double g_formSalePrice       = 0.0;
static char   g_formWarranty[32]    = {};
static char   g_formMfrId[64]       = {};
static char   g_formSupplierId[64]  = {};
static char   g_formRetailerId[64]  = {};
static int    g_auditN              = 20;

// -- Immutability demo state --
static bool                    g_tempGenerating   = false;
static float                   g_tempAccumulator  = 0.0f;
static float                   g_tempInterval     = 5.0f;
static std::vector<cw1::Block> g_tempChain;
static cw1::ValidationResult   g_tempVerify       = {false, "Not verified", {}};
static bool                    g_tempVerifyDone   = false;
static double                  g_tempVerifySeconds = 0.0;
static int                     g_tempTamperIndex  = -1;
static int                     g_tempDeleteIndex  = -1;
static std::size_t             g_tempRealCount    = 0;
static std::size_t             g_tempPrevCount    = 0;   // track for new-block animation
static float                   g_tempNewBlockAnim = 1.0f; // 0..1 fade-in alpha






static void loadDemoData(cw1::Blockchain& chain) {
    { cw1::CarRecord r;
      r.vin="VIN1001"; r.manufacturer="Perodua"; r.model="Axia";
      r.color="Silver"; r.productionYear=2024;
      r.manufacturerId="MFR-2522";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Shah Alam Plant";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-A1"; r.receivedBy="Ahmad bin Ismail";
      r.supplierId="SUP-197588";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-001"; r.passed=true; r.qcNotes="All systems nominal";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::DEALER_DISPATCH;
      r.dealerId="DLR-KB-01"; r.destination="Kota Bharu"; r.transportMode="Truck";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::CUSTOMER_SALE;
      r.buyerId="CUST-10201"; r.salePrice=38000.00; r.warrantyExpiry="2029-03-01";
      r.retailerId="RTL-91428";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1002"; r.manufacturer="Toyota"; r.model="Vios";
      r.color="White"; r.productionYear=2023;
      r.manufacturerId="MFR-3011";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Toyota Bukit Raja";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-B2"; r.receivedBy="Lim Wei Jie";
      r.supplierId="SUP-203344";
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
      r.manufacturerId="MFR-4500";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Honda Pegoh, Melaka";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-C3"; r.receivedBy="Raj Kumar";
      r.supplierId="SUP-310021";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::QUALITY_CHECK;
      r.inspectorId="QC-007"; r.passed=false;
      r.qcNotes="Brake pad below spec - pending replacement";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1004"; r.manufacturer="Proton"; r.model="X50";
      r.color="Red"; r.productionYear=2025;
      r.manufacturerId="MFR-1088";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Proton Tanjung Malim";
      chain.addBlock(r);
      r.stage=cw1::BlockStage::WAREHOUSE_INTAKE;
      r.warehouseLocation="WH-D4"; r.receivedBy="Nurul Aina";
      r.supplierId="SUP-405512";
      chain.addBlock(r); }
    { cw1::CarRecord r;
      r.vin="VIN1005"; r.manufacturer="Perodua"; r.model="Myvi";
      r.color="Black"; r.productionYear=2025;
      r.manufacturerId="MFR-2523";
      r.stage=cw1::BlockStage::PRODUCTION; r.factoryLocation="Perodua Rawang";
      chain.addBlock(r); }
}





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

static int ExtractFailedBlockIndex(const std::string& message) {
    const std::string needle = "block index ";
    const std::size_t pos = message.find(needle);
    if (pos == std::string::npos) {
        return -1;
    }

    const char* start = message.c_str() + pos + needle.size();
    char* end = nullptr;
    const long value = std::strtol(start, &end, 10);
    if (end == start || value < 0) {
        return -1;
    }
    return static_cast<int>(value);
}

static void DrawSectionHeading(const char* text) {
    if (g_fontSection != nullptr) {
        ImGui::PushFont(g_fontSection);
    }
    ImGui::TextColored(COL_ACCENT, "%s", text);
    if (g_fontSection != nullptr) {
        ImGui::PopFont();
    }
}

static bool DrawPrimaryButton(const char* label, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
    ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1158c7));
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

static bool DrawDangerButton(const char* label, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
    ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x5d2227));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x7c2f35));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x94353d));
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

static void DrawMetricBadge(const char* text, ImVec4 textColor, ImVec4 bgColor) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgColor);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::Button(text);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
}





static void RenderHeader(const cw1::Blockchain& chain) {
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

    float statsWidth = 430.0f;
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - statsWidth
                    + ImGui::GetCursorPosX() - 16.0f);

    char badge[64];
    snprintf(badge, sizeof(badge), "%zu blocks", chain.totalBlocks());
    DrawMetricBadge(badge, COL_ACCENT_HO, HexColor(0x1f6feb, 0.14f));
    ImGui::SameLine();
    snprintf(badge, sizeof(badge), "%zu cars", chain.getAllVins().size());
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





static void RenderSidebar(const cw1::Blockchain& chain) {
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
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_ACCENT_SOFT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x1f6feb, 0.35f));
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        COL_BG_ELEV);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        if (ImGui::Button(item.label, ImVec2(-1.0f, 34.0f))) {
            g_view = item.view;
            g_selectedVin.clear();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
    }

    
    {
        bool active = (g_view == View::DELETE_BLOCK);
        if (active) {
            ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x5d2227));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x7c2f35));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x94353d));
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_TEXT);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x3d1519));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x5d2227));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  HexColor(0x7c2f35));
            ImGui::PushStyleColor(ImGuiCol_Text,          COL_RED);
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
    ImGui::Separator();
    ImGui::Spacing();

    
    auto allVins = chain.getAllVins();
    ImGui::TextColored(COL_MUTED, "VEHICLES (%zu)", allVins.size());
    ImGui::Spacing();

    
    
    
    std::vector<std::string> displayVins;
    const std::string query(g_searchBuf);
    if (!query.empty()) {
        if (query != g_cachedSearchQuery) {
            g_cachedSearchQuery = query;
            g_cachedSearchVins.clear();

            const double seconds = cw1::measureSeconds([&]() {
                const auto hits = chain.searchBySQL(query);
                std::vector<std::string> seen;
                for (const auto* blk : hits) {
                    const std::string& vin = blk->getRecord().vin;
                    if (std::find(seen.begin(), seen.end(), vin) == seen.end()) {
                        seen.push_back(vin);
                        g_cachedSearchVins.push_back(vin);
                    }
                }
            });
            g_lastSearchSeconds = seconds;
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
        if (history.empty()) continue;
        const auto& rec    = history.front()->getRecord();
        bool        sel    = (vin == g_selectedVin);
        cw1::BlockStage st = chain.getLatestStage(vin);

        
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








static void RenderBranchMap(float mapW, float mapH); // forward declaration

static void RenderDashboard(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    size_t totalBlocks = chain.totalBlocks();
    size_t totalCars   = chain.getAllVins().size();
    DrawSectionHeading("Dashboard");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Top section: stat cards (left) + map (right) ---
    auto StatCard = [](const char* value, const char* label, ImVec4 col, ImVec2 sz) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
        ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
        ImGui::BeginChild(label, sz, true);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
        if (g_fontSection != nullptr) ImGui::PushFont(g_fontSection);
        ImGui::TextColored(col, "%s", value);
        if (g_fontSection != nullptr) ImGui::PopFont();
        ImGui::TextColored(COL_MUTED, "%s", label);
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
    };

    char buf1[32], buf2[32], buf4[32];
    snprintf(buf1, sizeof(buf1), "%zu", totalBlocks);
    snprintf(buf2, sizeof(buf2), "%zu", totalCars);
    snprintf(buf4, sizeof(buf4), "%zu", chain.getAuditLog().size());

    float totalW = ImGui::GetContentRegionAvail().x;
    float mapWidth = totalW * 0.42f;
    if (mapWidth < 280) mapWidth = 280;
    float statsW = totalW - mapWidth - 12.0f;
    float mapH = 260.0f;
    float cardH = (mapH - 8.0f) / 2.0f;

    // Left column: 2x2 grid of stat cards
    ImGui::BeginGroup();
    {
        float halfW = (statsW - 8.0f) / 2.0f;
        StatCard(buf1, "Total Blocks",   COL_ACCENT,   ImVec2(halfW, cardH));
        ImGui::SameLine(0, 8);
        StatCard(buf2, "Unique Vehicles",COL_GREEN_BR, ImVec2(halfW, cardH));

        ImGui::Spacing();

        StatCard(g_verifyDone && g_lastVerify.ok ? "VERIFIED" : (g_verifyDone ? "TAMPERED" : "N/A"),
                 "Integrity",
                 g_verifyDone && g_lastVerify.ok ? COL_GREEN_BR : COL_RED,
                 ImVec2(halfW, cardH));
        ImGui::SameLine(0, 8);
        StatCard(buf4, "Audit Events", COL_PURPLE, ImVec2(halfW, cardH));
    }
    ImGui::EndGroup();

    // Right column: branch map
    ImGui::SameLine(0, 12);
    ImGui::BeginGroup();
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(8, 15, 30, 255));
        ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
        ImGui::BeginChild("##mapChild", ImVec2(mapWidth, mapH), true,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        float mw = ImGui::GetContentRegionAvail().x;
        float mh = ImGui::GetContentRegionAvail().y;
        RenderBranchMap(mw, mh);
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
    }
    ImGui::EndGroup();

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "RECENT BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    
    ImGuiTableFlags tflags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                             ImGuiTableFlags_RowBg |
                             ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 5.0f));
    float availH = ImGui::GetContentRegionAvail().y;
    if (ImGui::BeginTable("##recent", 6, tflags, ImVec2(-1, availH > 0 ? availH : 400))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed,   60);
        ImGui::TableSetupColumn("Hash",         ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN",          ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage",        ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Timestamp",    ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        
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
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", b.getCurrentHash().c_str());
                ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", b.getSha3Hash().c_str());
                ImGui::EndTooltip();
            }

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
    ImGui::PopStyleVar();
}





// Helper: render a label-value row inside the detail card
static void DetailRow(const char* label, const char* value,
                      ImVec4 valColor = COL_TEXT) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextColored(COL_MUTED, "%s", label);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextColored(valColor, "%s", value);
}

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

    // Vehicle header card
    const cw1::CarRecord& first = history.front()->getRecord();
    cw1::BlockStage latestStage = chain.getLatestStage(g_selectedVin);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
    ImGui::BeginChild("##vehHeader", ImVec2(-1, 80), true);
    if (g_fontSection) ImGui::PushFont(g_fontSection);
    ImGui::TextColored(COL_TEXT, "%s %s", first.manufacturer.c_str(), first.model.c_str());
    if (g_fontSection) ImGui::PopFont();
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
    ImGui::Separator(); ImGui::Spacing();

    for (size_t idx = 0; idx < history.size(); ++idx) {
        const cw1::Block& blk = *history[idx];
        const cw1::CarRecord& rec = blk.getRecord();
        ImVec4 stCol = StageColor(rec.stage);

        char hdrLabel[128];
        snprintf(hdrLabel, sizeof(hdrLabel), "Block #%zu  |  %s##det%zu",
                 blk.getIndex(), cw1::stageToString(rec.stage).c_str(), idx);

        // Stage color accent bar on the left of the header
        ImVec2 hdrPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            hdrPos, ImVec2(hdrPos.x + 4.0f, hdrPos.y + ImGui::GetFrameHeight()),
            ImGui::ColorConvertFloat4ToU32(stCol), 2.0f);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

        ImGui::PushStyleColor(ImGuiCol_Header,        COL_BG_CARD);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, COL_BG_HOVER);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  HexColor(0x1f6feb, 0.2f));
        bool open = ImGui::CollapsingHeader(hdrLabel,
                        idx == history.size() - 1
                            ? ImGuiTreeNodeFlags_DefaultOpen
                            : ImGuiTreeNodeFlags_None);
        ImGui::PopStyleColor(3);
        if (!open) continue;

        // Count rows to compute a fixed height for the child
        int rows = 7; // hash, sha3, prev, timestamp, nonce, vin, vehicle
        const bool isGenesis = (blk.getIndex() == 0);
        if (isGenesis) rows++;
        switch (rec.stage) {
            case cw1::BlockStage::PRODUCTION:       rows += 1; break;
            case cw1::BlockStage::WAREHOUSE_INTAKE: rows += 3; break;
            case cw1::BlockStage::QUALITY_CHECK:    rows += 3; break;
            case cw1::BlockStage::DEALER_DISPATCH:  rows += 3; break;
            case cw1::BlockStage::CUSTOMER_SALE:    rows += 4; break;
        }
        float rowH = ImGui::GetTextLineHeightWithSpacing() + 4.0f;
        float childH = rows * rowH + 72.0f; // extra for section headers and padding

        char childId[64];
        snprintf(childId, sizeof(childId), "##blkdet%zu", idx);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_CARD);
        ImGui::BeginChild(childId, ImVec2(-1, childH), true);

        // -- Block Info section --
        ImGui::TextColored(COL_ACCENT, "Block Information");
        ImGui::Spacing();
        if (ImGui::BeginTable("##binfo", 2, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            DetailRow("Current Hash", blk.getCurrentHash().c_str(), COL_TEXT);
            DetailRow("SHA3-128 Hash", blk.getSha3Hash().c_str(), COL_TEXT);
            DetailRow("Previous Hash", blk.getPreviousHash().c_str(), COL_TEXT);
            if (isGenesis) {
                bool ok = (blk.getPreviousHash() == blk.getCurrentHash());
                DetailRow("Genesis Rule", ok ? "previousHash == currentHash  YES" : "previousHash == currentHash  NO",
                          ok ? COL_GREEN_BR : COL_RED);
            }
            DetailRow("Timestamp", blk.getTimestamp().c_str(), COL_TEXT);
            {
                char nonceBuf[32];
                snprintf(nonceBuf, sizeof(nonceBuf), "%llu", (unsigned long long)blk.getNonce());
                DetailRow("Nonce", nonceBuf, COL_TEXT);
            }
            DetailRow("VIN", rec.vin.c_str(), COL_ACCENT);
            DetailRow("Mfr. ID", rec.manufacturerId.c_str(), COL_TEXT);
            {
                char vehBuf[128];
                snprintf(vehBuf, sizeof(vehBuf), "%s %s (%s, %d)",
                         rec.manufacturer.c_str(), rec.model.c_str(),
                         rec.color.c_str(), rec.productionYear);
                DetailRow("Vehicle", vehBuf, COL_TEXT);
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::TextColored(stCol, "Payload  -  %s", cw1::stageToString(rec.stage).c_str());
        ImGui::Spacing();

        if (ImGui::BeginTable("##payload", 2, ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("PLabel", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("PValue", ImGuiTableColumnFlags_WidthStretch);

            switch (rec.stage) {
                case cw1::BlockStage::PRODUCTION:
                    DetailRow("Factory Location", rec.factoryLocation.c_str());
                    break;
                case cw1::BlockStage::WAREHOUSE_INTAKE:
                    DetailRow("Warehouse Location", rec.warehouseLocation.c_str());
                    DetailRow("Received By", rec.receivedBy.c_str());
                    DetailRow("Supplier ID", rec.supplierId.c_str());
                    break;
                case cw1::BlockStage::QUALITY_CHECK:
                    DetailRow("Inspector ID", rec.inspectorId.c_str());
                    DetailRow("QC Result", rec.passed ? "PASSED" : "FAILED",
                              rec.passed ? COL_GREEN_BR : COL_RED);
                    DetailRow("QC Notes", rec.qcNotes.c_str());
                    break;
                case cw1::BlockStage::DEALER_DISPATCH:
                    DetailRow("Dealer ID", rec.dealerId.c_str());
                    DetailRow("Destination", rec.destination.c_str());
                    DetailRow("Transport Mode", rec.transportMode.c_str());
                    break;
                case cw1::BlockStage::CUSTOMER_SALE: {
                    DetailRow("Buyer ID", rec.buyerId.c_str());
                    DetailRow("Retailer ID", rec.retailerId.c_str());
                    char priceBuf[64];
                    snprintf(priceBuf, sizeof(priceBuf), "MYR %.2f", rec.salePrice);
                    DetailRow("Sale Price", priceBuf, COL_GREEN_BR);
                    DetailRow("Warranty Expiry", rec.warrantyExpiry.c_str());
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





static const char* k_stageNames[] = {
    "PRODUCTION", "WAREHOUSE_INTAKE", "QUALITY_CHECK",
    "DEALER_DISPATCH", "CUSTOMER_SALE"
};

static const char* k_manufacturers[] = {
    "Perodua", "Proton", "Toyota", "Honda", "Nissan", "Mazda",
    "Mitsubishi", "Suzuki", "Hyundai", "Kia", "BMW", "Mercedes-Benz",
    "Volkswagen", "Subaru", "Isuzu", "Ford", "Lexus", "Volvo"
};

struct ModelList { const char** models; int count; };

static const char* k_models_perodua[]  = {"Myvi","Axia","Bezza","Aruz","Ativa","Alza"};
static const char* k_models_proton[]   = {"Saga","Persona","X50","X70","X90","Iriz","Exora","S70"};
static const char* k_models_toyota[]   = {"Vios","Camry","Yaris","Corolla Cross","Hilux","Innova","Rush","Fortuner","Veloz"};
static const char* k_models_honda[]    = {"City","Civic","HR-V","CR-V","Accord","BR-V","WR-V"};
static const char* k_models_nissan[]   = {"Almera","X-Trail","Navara","Serena","Kicks"};
static const char* k_models_mazda[]    = {"Mazda 2","Mazda 3","CX-3","CX-5","CX-8","CX-30","BT-50","MX-5"};
static const char* k_models_mitsubishi[] = {"Triton","Xpander","Outlander","ASX"};
static const char* k_models_suzuki[]   = {"Swift","Vitara","Jimny","S-Cross","Ertiga","XL7"};
static const char* k_models_hyundai[]  = {"Kona","Tucson","Santa Fe","Stargazer","Ioniq 5","Ioniq 6","Palisade"};
static const char* k_models_kia[]      = {"Seltos","Sportage","Carnival","Cerato","EV6","EV9"};
static const char* k_models_bmw[]      = {"3 Series","5 Series","X1","X3","X5","iX","i4","i5"};
static const char* k_models_merc[]     = {"A-Class","C-Class","E-Class","GLA","GLC","GLE","EQA","EQB","EQE"};
static const char* k_models_vw[]       = {"Golf","Tiguan","Arteon","ID.4","T-Cross","Passat"};
static const char* k_models_subaru[]   = {"XV","Forester","Outback","WRX","BRZ"};
static const char* k_models_isuzu[]    = {"D-Max","MU-X"};
static const char* k_models_ford[]     = {"Ranger","Everest","Territory"};
static const char* k_models_lexus[]    = {"NX","RX","ES","UX","IS","LC"};
static const char* k_models_volvo[]    = {"XC40","XC60","XC90","S60","S90","C40","EX30"};

static const ModelList k_modelsByMfr[] = {
    {k_models_perodua,    6}, {k_models_proton,     8}, {k_models_toyota,    9},
    {k_models_honda,      7}, {k_models_nissan,     5}, {k_models_mazda,     8},
    {k_models_mitsubishi, 4}, {k_models_suzuki,     6}, {k_models_hyundai,   7},
    {k_models_kia,        6}, {k_models_bmw,        8}, {k_models_merc,      9},
    {k_models_vw,         6}, {k_models_subaru,     5}, {k_models_isuzu,     2},
    {k_models_ford,       3}, {k_models_lexus,      6}, {k_models_volvo,     7},
};

// ── Branch / factory locations per manufacturer (5 Malaysia + 1 origin) ──
struct BranchList { const char** branches; int count; };

static const char* k_branches_perodua[] = {
    "Rawang, Selangor, Malaysia",
    "Alor Setar, Kedah, Malaysia",
    "Kuala Terengganu, Terengganu, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Kuching, Sarawak, Malaysia"            // origin: Malaysia
};
static const char* k_branches_proton[] = {
    "Tanjung Malim, Perak, Malaysia",
    "Batu Kawan, Penang, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Pasir Gudang, Johor, Malaysia",
    "Shah Alam, Selangor, Malaysia"         // origin: Malaysia
};
static const char* k_branches_toyota[] = {
    "Glenmarie, Selangor, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Taiping, Perak, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Toyota City, Aichi, Japan"             // origin: Japan
};
static const char* k_branches_honda[] = {
    "Pegoh, Melaka, Malaysia",
    "Kota Bharu, Kelantan, Malaysia",
    "Ipoh, Perak, Malaysia",
    "Tawau, Sabah, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Minato, Tokyo, Japan"                  // origin: Japan
};
static const char* k_branches_nissan[] = {
    "Serendah, Selangor, Malaysia",
    "Langkawi, Kedah, Malaysia",
    "Kuala Terengganu, Terengganu, Malaysia",
    "Miri, Sarawak, Malaysia",
    "Pasir Gudang, Johor, Malaysia",
    "Yokohama, Kanagawa, Japan"             // origin: Japan
};
static const char* k_branches_mazda[] = {
    "Kulim, Kedah, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Sibu, Sarawak, Malaysia",
    "Sandakan, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Hiroshima, Hiroshima, Japan"           // origin: Japan
};
static const char* k_branches_mitsubishi[] = {
    "Pekan, Pahang, Malaysia",
    "Butterworth, Penang, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Okazaki, Aichi, Japan"                 // origin: Japan
};
static const char* k_branches_suzuki[] = {
    "Shah Alam, Selangor, Malaysia",
    "Sungai Petani, Kedah, Malaysia",
    "Kuala Terengganu, Terengganu, Malaysia",
    "Sibu, Sarawak, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Hamamatsu, Shizuoka, Japan"            // origin: Japan
};
static const char* k_branches_hyundai[] = {
    "Gurun, Kedah, Malaysia",
    "Taiping, Perak, Malaysia",
    "Temerloh, Pahang, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Ulsan, Gyeongsang, South Korea"        // origin: South Korea
};
static const char* k_branches_kia[] = {
    "Alor Setar, Kedah, Malaysia",
    "Ipoh, Perak, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Pasir Gudang, Johor, Malaysia",
    "Hwaseong, Gyeonggi, South Korea"       // origin: South Korea
};
static const char* k_branches_bmw[] = {
    "Kulim, Kedah, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Pekan, Pahang, Malaysia",
    "Sandakan, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Munich, Bavaria, Germany"              // origin: Germany
};
static const char* k_branches_merc[] = {
    "Sitiawan, Perak, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Kota Bharu, Kelantan, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Stuttgart, Baden-Wurttemberg, Germany"  // origin: Germany
};
static const char* k_branches_vw[] = {
    "Pekan, Pahang, Malaysia",
    "Langkawi, Kedah, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Wolfsburg, Lower Saxony, Germany"      // origin: Germany
};
static const char* k_branches_subaru[] = {
    "Glenmarie, Selangor, Malaysia",
    "Kota Bharu, Kelantan, Malaysia",
    "Miri, Sarawak, Malaysia",
    "Tawau, Sabah, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Ota, Gunma, Japan"                     // origin: Japan
};
static const char* k_branches_isuzu[] = {
    "Rawang, Selangor, Malaysia",
    "Sitiawan, Perak, Malaysia",
    "Alor Setar, Kedah, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Fujisawa, Kanagawa, Japan"             // origin: Japan
};
static const char* k_branches_ford[] = {
    "Sungai Petani, Kedah, Malaysia",
    "Ipoh, Perak, Malaysia",
    "Kuantan, Pahang, Malaysia",
    "Seremban, Negeri Sembilan, Malaysia",
    "Kota Kinabalu, Sabah, Malaysia",
    "Dearborn, Michigan, United States"     // origin: USA
};
static const char* k_branches_lexus[] = {
    "Shah Alam, Selangor, Malaysia",
    "Taiping, Perak, Malaysia",
    "Georgetown, Penang, Malaysia",
    "Tawau, Sabah, Malaysia",
    "Johor Bahru, Johor, Malaysia",
    "Tahara, Aichi, Japan"                  // origin: Japan
};
static const char* k_branches_volvo[] = {
    "Glenmarie, Selangor, Malaysia",
    "Butterworth, Penang, Malaysia",
    "Kuching, Sarawak, Malaysia",
    "Sandakan, Sabah, Malaysia",
    "Batu Pahat, Johor, Malaysia",
    "Gothenburg, Vastra Gotaland, Sweden"   // origin: Sweden
};

static const BranchList k_branchesByMfr[] = {
    {k_branches_perodua,    6}, {k_branches_proton,     6}, {k_branches_toyota,    6},
    {k_branches_honda,      6}, {k_branches_nissan,     6}, {k_branches_mazda,     6},
    {k_branches_mitsubishi, 6}, {k_branches_suzuki,     6}, {k_branches_hyundai,   6},
    {k_branches_kia,        6}, {k_branches_bmw,        6}, {k_branches_merc,      6},
    {k_branches_vw,         6}, {k_branches_subaru,     6}, {k_branches_isuzu,     6},
    {k_branches_ford,       6}, {k_branches_lexus,      6}, {k_branches_volvo,     6},
};

static const char* k_colors[] = {
    "White","Black","Silver","Grey","Red","Blue",
    "Brown","Green","Yellow","Orange","Beige","Maroon"
};

static const char* k_transportModes[] = {
    "Truck","Trailer","Car Carrier","Self-Drive"
};

// ── Interactive world map with branch pins ──────────────────────────────

struct GeoPin { float lat; float lng; const char* label; const char* country; };

static const GeoPin k_allPins[] = {
    // Malaysia — scattered across all states
    { 3.3215f, 101.5770f, "Rawang",           "Malaysia"},
    { 6.1200f, 100.3700f, "Alor Setar",       "Malaysia"},
    { 5.3117f, 103.1324f, "K. Terengganu",    "Malaysia"},
    { 2.7300f, 101.9400f, "Seremban",         "Malaysia"},
    { 1.4927f, 103.7414f, "Johor Bahru",      "Malaysia"},
    { 1.5535f, 110.3593f, "Kuching",          "Malaysia"},
    { 3.6845f, 101.5216f, "Tanjung Malim",    "Malaysia"},
    { 5.2000f, 100.4375f, "Batu Kawan",       "Malaysia"},
    { 3.8077f, 103.3260f, "Kuantan",          "Malaysia"},
    { 5.9804f, 116.0735f, "Kota Kinabalu",    "Malaysia"},
    { 1.4700f, 103.9100f, "Pasir Gudang",     "Malaysia"},
    { 3.0733f, 101.5185f, "Shah Alam",        "Malaysia"},
    { 3.1000f, 101.5833f, "Glenmarie",        "Malaysia"},
    { 5.4141f, 100.3288f, "Georgetown",       "Malaysia"},
    { 4.8500f, 100.7300f, "Taiping",          "Malaysia"},
    { 2.3167f, 102.1500f, "Pegoh",            "Malaysia"},
    { 6.1200f, 102.2400f, "Kota Bharu",       "Malaysia"},
    { 4.5975f, 101.0901f, "Ipoh",             "Malaysia"},
    { 4.2400f, 117.8900f, "Tawau",            "Malaysia"},
    { 3.3667f, 101.6000f, "Serendah",         "Malaysia"},
    { 6.3500f,  99.8000f, "Langkawi",         "Malaysia"},
    { 4.4000f, 114.0000f, "Miri",             "Malaysia"},
    { 1.8500f, 102.9300f, "Batu Pahat",       "Malaysia"},
    { 5.3833f, 100.5667f, "Kulim",            "Malaysia"},
    { 2.3000f, 111.8300f, "Sibu",             "Malaysia"},
    { 3.4833f, 103.3833f, "Pekan",            "Malaysia"},
    { 5.8400f, 118.1200f, "Sandakan",         "Malaysia"},
    { 5.6500f, 100.4900f, "Sungai Petani",    "Malaysia"},
    { 5.8167f, 100.4667f, "Gurun",            "Malaysia"},
    { 3.4500f, 102.4200f, "Temerloh",         "Malaysia"},
    { 5.4000f, 100.3700f, "Butterworth",      "Malaysia"},
    { 4.2200f, 100.7000f, "Sitiawan",         "Malaysia"},
    // Japan
    {35.0853f, 137.1577f, "Toyota City",      "Japan"},
    {35.6581f, 139.7514f, "Minato, Tokyo",    "Japan"},
    {35.4437f, 139.6380f, "Yokohama",         "Japan"},
    {34.3853f, 132.4553f, "Hiroshima",        "Japan"},
    {34.9551f, 137.1629f, "Okazaki",          "Japan"},
    {34.7108f, 137.7261f, "Hamamatsu",        "Japan"},
    {36.2959f, 139.3784f, "Ota, Gunma",       "Japan"},
    {35.3390f, 139.4870f, "Fujisawa",         "Japan"},
    {34.6693f, 137.0334f, "Tahara",           "Japan"},
    // South Korea
    {35.5384f, 129.3114f, "Ulsan",            "South Korea"},
    {37.1997f, 126.8313f, "Hwaseong",         "South Korea"},
    // Germany
    {48.1351f,  11.5820f, "Munich",           "Germany"},
    {48.7758f,   9.1829f, "Stuttgart",        "Germany"},
    {52.4227f,  10.7865f, "Wolfsburg",        "Germany"},
    // USA
    {42.3223f, -83.1763f, "Dearborn",         "United States"},
    // Sweden
    {57.7089f,  11.9746f, "Gothenburg",       "Sweden"},
};
static const int k_allPinsCount = sizeof(k_allPins) / sizeof(k_allPins[0]);

static float g_globeRotLon  = 110.0f;   // center longitude (starts at Malaysia)
static float g_globeRotLat  = 5.0f;     // center latitude
static float g_globeRadius  = 0.30f;    // radius fraction of min(w,h)/2

// Simplified coastline outlines
static const float k_peninsular[][2] = {
    {6.5f,100.0f},{6.2f,100.2f},{5.8f,100.3f},{5.5f,100.2f},{5.3f,100.3f},
    {4.9f,100.4f},{4.2f,100.6f},{3.7f,101.0f},{3.1f,101.3f},{2.5f,101.7f},
    {1.8f,102.3f},{1.4f,103.5f},{1.3f,103.8f},{1.4f,104.0f},{1.6f,104.2f},
    {2.0f,103.4f},{2.5f,103.4f},{3.1f,103.4f},{3.8f,103.4f},{4.2f,103.4f},
    {4.9f,103.4f},{5.5f,103.1f},{5.9f,102.4f},{6.2f,102.1f},{6.5f,101.8f},
    {6.7f,101.2f},{6.5f,100.0f},
};
static const float k_borneo[][2] = {
    {1.0f,109.6f},{1.3f,110.0f},{1.5f,110.3f},{1.6f,110.9f},{2.1f,111.3f},
    {2.5f,111.5f},{2.8f,111.9f},{3.2f,113.0f},{4.0f,114.1f},{4.3f,114.6f},
    {4.9f,115.0f},{5.3f,115.2f},{5.7f,115.6f},{6.0f,116.1f},{6.3f,116.8f},
    {6.5f,117.5f},{6.8f,117.6f},{7.0f,117.5f},{7.1f,117.0f},{6.8f,116.7f},
    {6.4f,116.5f},{5.8f,118.0f},{5.5f,118.3f},{5.0f,118.6f},{4.5f,118.1f},
    {4.0f,117.6f},{3.5f,117.3f},{3.0f,116.1f},{2.5f,115.5f},{2.0f,115.0f},
    {1.5f,114.5f},{1.2f,113.5f},{1.0f,112.5f},{0.9f,111.5f},{0.8f,110.5f},
    {1.0f,109.6f},
};
static const float k_japan[][2] = {
    {31.0f,131.0f},{33.0f,131.5f},{33.5f,132.0f},{34.0f,132.5f},{34.3f,133.0f},
    {34.5f,134.0f},{34.7f,135.0f},{35.0f,135.5f},{35.5f,136.0f},{36.0f,136.5f},
    {36.5f,137.0f},{37.0f,137.0f},{37.5f,138.5f},{38.0f,139.0f},{39.0f,140.0f},
    {40.0f,140.0f},{41.0f,140.5f},{41.5f,141.0f},{41.0f,141.5f},{40.0f,140.5f},
    {39.0f,140.5f},{38.0f,140.0f},{37.0f,139.5f},{36.5f,140.0f},{36.0f,140.5f},
    {35.5f,140.5f},{35.0f,140.0f},{34.5f,137.5f},{34.0f,136.5f},{33.5f,135.5f},
    {33.0f,134.0f},{32.5f,132.5f},{31.5f,131.5f},{31.0f,131.0f},
};
static const float k_korea[][2] = {
    {34.5f,126.5f},{35.0f,126.0f},{35.5f,126.5f},{36.0f,126.5f},{36.5f,126.5f},
    {37.0f,126.5f},{37.5f,126.5f},{38.0f,126.7f},{38.5f,127.0f},{38.5f,128.0f},
    {38.0f,128.5f},{37.5f,129.0f},{37.0f,129.4f},{36.5f,129.5f},{36.0f,129.5f},
    {35.5f,129.3f},{35.0f,129.0f},{34.5f,128.5f},{34.5f,127.5f},{34.5f,126.5f},
};
static const float k_europe[][2] = {
    {36.0f,-10.0f},{37.0f,-8.0f},{39.0f,-9.0f},{42.0f,-9.0f},{43.5f,-8.0f},
    {43.0f,-1.0f},{46.0f,-1.0f},{48.5f,-5.0f},{49.0f,-1.0f},{51.0f,1.0f},
    {53.5f,0.0f},{55.0f,8.0f},{54.0f,10.0f},{55.5f,12.5f},{56.0f,13.0f},
    {57.5f,12.0f},{58.5f,11.5f},{59.0f,11.0f},{62.0f,5.0f},{65.0f,12.0f},
    {69.0f,16.0f},{70.0f,20.0f},{69.0f,28.0f},{65.0f,30.0f},{60.0f,30.0f},
    {55.0f,28.0f},{50.0f,24.0f},{48.0f,17.0f},{46.0f,16.0f},{45.0f,14.0f},
    {44.0f,12.0f},{42.0f,12.0f},{39.0f,16.0f},{38.0f,16.0f},{37.0f,15.5f},
    {36.5f,12.5f},{37.5f,10.0f},{37.0f,1.0f},{36.5f,-2.0f},{36.0f,-6.0f},
    {36.0f,-10.0f},
};
static const float k_usa[][2] = {
    {25.0f,-80.0f},{30.0f,-82.0f},{30.0f,-90.0f},{29.0f,-95.0f},{26.0f,-97.0f},
    {32.0f,-106.0f},{32.0f,-117.0f},{34.0f,-120.0f},{37.0f,-122.0f},{40.0f,-124.0f},
    {46.0f,-124.0f},{48.5f,-123.0f},{49.0f,-95.0f},{49.0f,-83.0f},{46.0f,-83.0f},
    {42.0f,-83.0f},{42.0f,-71.0f},{40.0f,-74.0f},{37.0f,-76.0f},{35.0f,-76.0f},
    {33.0f,-79.0f},{30.0f,-81.0f},{25.0f,-80.0f},
};

// Africa
static const float k_africa[][2] = {
    {37.0f,-1.0f},{36.5f,3.0f},{37.0f,10.0f},{33.0f,12.0f},{32.0f,15.0f},
    {31.0f,25.0f},{31.5f,32.0f},{29.0f,33.0f},{22.0f,36.5f},{15.0f,40.0f},
    {12.0f,43.0f},{11.5f,51.0f},{5.0f,49.0f},{2.0f,45.0f},{-1.0f,42.0f},
    {-5.0f,40.0f},{-10.0f,40.5f},{-15.0f,40.5f},{-25.0f,35.0f},{-34.0f,26.0f},
    {-34.5f,18.0f},{-31.0f,17.0f},{-22.0f,14.0f},{-17.0f,11.0f},{-12.0f,13.0f},
    {-5.0f,12.0f},{0.0f,9.5f},{4.0f,7.0f},{6.0f,3.0f},{5.0f,-2.0f},
    {4.5f,-7.5f},{7.5f,-13.0f},{10.0f,-15.0f},{15.0f,-17.0f},{20.0f,-17.0f},
    {25.0f,-15.0f},{28.0f,-13.0f},{33.0f,-8.0f},{35.5f,-5.5f},{37.0f,-1.0f},
};
// South America
static const float k_south_america[][2] = {
    {12.0f,-72.0f},{10.5f,-75.0f},{8.0f,-77.0f},{4.0f,-77.5f},{1.0f,-80.0f},
    {-4.0f,-81.0f},{-6.0f,-81.0f},{-14.0f,-76.0f},{-18.0f,-70.0f},{-24.0f,-70.5f},
    {-33.0f,-72.0f},{-46.0f,-75.0f},{-52.0f,-70.0f},{-55.0f,-68.0f},{-54.0f,-65.0f},
    {-48.0f,-65.5f},{-42.0f,-63.0f},{-38.0f,-57.0f},{-34.5f,-54.0f},{-29.0f,-49.0f},
    {-23.0f,-43.0f},{-13.0f,-38.5f},{-5.0f,-35.0f},{-2.0f,-44.0f},{0.5f,-50.0f},
    {6.0f,-57.0f},{8.5f,-60.0f},{10.5f,-67.0f},{12.0f,-72.0f},
};
// Australia
static const float k_australia[][2] = {
    {-12.0f,136.0f},{-12.5f,130.5f},{-14.5f,129.0f},{-18.0f,122.0f},
    {-22.0f,114.0f},{-26.0f,113.0f},{-30.0f,115.0f},{-35.0f,117.5f},
    {-35.5f,137.0f},{-37.0f,140.0f},{-39.0f,146.0f},{-37.5f,150.0f},
    {-33.5f,152.0f},{-28.0f,153.5f},{-24.0f,152.0f},{-19.0f,147.0f},
    {-14.5f,143.5f},{-11.0f,136.5f},{-12.0f,136.0f},
};
// India
static const float k_india[][2] = {
    {35.0f,74.0f},{30.0f,78.0f},{27.0f,88.0f},{22.0f,89.0f},{21.0f,87.0f},
    {15.0f,80.0f},{8.0f,77.5f},{8.5f,76.0f},{13.0f,74.5f},{20.0f,73.0f},
    {24.0f,68.5f},{25.5f,62.0f},{29.0f,65.0f},{33.0f,72.0f},{35.0f,74.0f},
};
// China / mainland East Asia
static const float k_china[][2] = {
    {40.0f,73.5f},{45.0f,82.0f},{50.0f,87.5f},{53.5f,109.0f},{50.0f,117.0f},
    {48.0f,120.0f},{48.0f,135.0f},{43.0f,130.0f},{39.0f,122.0f},{35.0f,119.5f},
    {31.0f,122.0f},{25.0f,120.0f},{22.0f,114.0f},{21.5f,110.0f},{22.0f,106.0f},
    {21.5f,99.0f},{27.0f,97.0f},{35.0f,79.0f},{40.0f,73.5f},
};
// SE Asia mainland (Thailand, Vietnam, Myanmar, Cambodia, Laos)
static const float k_se_asia[][2] = {
    {22.0f,101.0f},{20.0f,100.0f},{17.0f,97.5f},{10.0f,98.5f},{7.0f,99.5f},
    {1.3f,103.8f},{1.4f,104.0f},{8.0f,105.0f},{10.5f,107.0f},{12.0f,109.0f},
    {16.5f,108.0f},{21.0f,106.0f},{23.5f,104.0f},{22.0f,101.0f},
};
// Arabian Peninsula / Middle East
static const float k_arabia[][2] = {
    {30.0f,35.0f},{25.0f,37.0f},{16.0f,43.0f},{13.0f,48.0f},{22.0f,59.5f},
    {26.0f,56.0f},{26.5f,50.0f},{29.0f,48.0f},{33.5f,36.0f},{30.0f,35.0f},
};
// Greenland
static const float k_greenland[][2] = {
    {60.0f,-45.0f},{65.0f,-54.0f},{72.0f,-56.0f},{78.0f,-68.0f},{82.0f,-55.0f},
    {83.0f,-40.0f},{78.0f,-18.0f},{70.0f,-22.0f},{62.0f,-42.0f},{60.0f,-45.0f},
};
// UK
static const float k_uk[][2] = {
    {50.0f,-5.5f},{51.5f,1.0f},{53.0f,0.0f},{55.5f,-2.0f},{58.5f,-3.0f},
    {58.5f,-5.0f},{56.0f,-5.5f},{54.0f,-4.5f},{51.5f,-5.0f},{50.0f,-5.5f},
};
// Central America / Mexico
static const float k_mexico[][2] = {
    {32.5f,-117.0f},{24.0f,-110.0f},{19.0f,-105.0f},{15.5f,-92.0f},{18.0f,-88.0f},
    {21.5f,-87.0f},{26.0f,-97.0f},{29.0f,-95.0f},{30.0f,-90.0f},{32.5f,-117.0f},
};
// Sumatra
static const float k_sumatra[][2] = {
    {5.5f,95.0f},{3.5f,98.5f},{0.0f,100.5f},{-3.0f,103.5f},{-5.8f,105.5f},
    {-3.5f,105.5f},{1.0f,102.0f},{5.0f,97.5f},{5.5f,95.0f},
};
// Java
static const float k_java[][2] = {
    {-6.0f,105.5f},{-7.0f,109.0f},{-8.0f,114.0f},{-8.5f,114.5f},
    {-7.5f,111.0f},{-6.5f,107.0f},{-6.0f,105.5f},
};
// Philippines
static const float k_philippines[][2] = {
    {18.5f,120.5f},{14.5f,120.0f},{13.0f,121.5f},{9.0f,126.0f},{6.5f,125.0f},
    {9.5f,124.0f},{12.5f,124.0f},{14.5f,121.0f},{18.5f,120.5f},
};
// New Zealand North Island
static const float k_nz_n[][2] = {
    {-34.5f,173.0f},{-37.5f,176.0f},{-41.5f,175.0f},{-38.5f,174.0f},{-34.5f,173.0f},
};
// New Zealand South Island
static const float k_nz_s[][2] = {
    {-41.0f,173.5f},{-44.0f,170.0f},{-46.5f,168.5f},{-44.0f,172.5f},{-41.0f,174.5f},{-41.0f,173.5f},
};
// Iceland
static const float k_iceland[][2] = {
    {63.5f,-20.0f},{64.0f,-14.0f},{66.0f,-14.0f},{66.5f,-18.0f},{65.5f,-24.0f},{63.5f,-20.0f},
};
// Madagascar
static const float k_madagascar[][2] = {
    {-12.0f,49.0f},{-15.5f,50.5f},{-22.0f,48.0f},{-25.5f,45.0f},{-23.0f,44.0f},
    {-16.0f,46.0f},{-12.0f,49.0f},
};
// Sri Lanka
static const float k_sri_lanka[][2] = {
    {9.5f,80.0f},{7.5f,81.5f},{6.0f,80.5f},{7.0f,79.5f},{9.5f,80.0f},
};
// Taiwan
static const float k_taiwan[][2] = {
    {25.0f,121.0f},{23.5f,121.5f},{22.0f,120.5f},{23.0f,120.0f},{25.0f,121.0f},
};
// Papua New Guinea
static const float k_png[][2] = {
    {-2.5f,141.0f},{-5.0f,141.5f},{-8.0f,143.0f},{-10.0f,147.0f},{-8.0f,148.5f},
    {-6.0f,147.5f},{-5.5f,150.0f},{-4.0f,152.5f},{-2.5f,150.0f},{-1.0f,141.0f},{-2.5f,141.0f},
};

#define OUTLINE_ENTRY(arr) {arr, sizeof(arr)/sizeof(arr[0])}
struct Outline { const float (*pts)[2]; int count; };
static const Outline k_outlines[] = {
    OUTLINE_ENTRY(k_peninsular), OUTLINE_ENTRY(k_borneo),
    OUTLINE_ENTRY(k_japan),      OUTLINE_ENTRY(k_korea),
    OUTLINE_ENTRY(k_europe),     OUTLINE_ENTRY(k_usa),
    OUTLINE_ENTRY(k_africa),     OUTLINE_ENTRY(k_south_america),
    OUTLINE_ENTRY(k_australia),  OUTLINE_ENTRY(k_india),
    OUTLINE_ENTRY(k_china),      OUTLINE_ENTRY(k_se_asia),
    OUTLINE_ENTRY(k_arabia),     OUTLINE_ENTRY(k_greenland),
    OUTLINE_ENTRY(k_uk),         OUTLINE_ENTRY(k_mexico),
    OUTLINE_ENTRY(k_sumatra),    OUTLINE_ENTRY(k_java),
    OUTLINE_ENTRY(k_philippines),OUTLINE_ENTRY(k_nz_n),
    OUTLINE_ENTRY(k_nz_s),      OUTLINE_ENTRY(k_iceland),
    OUTLINE_ENTRY(k_madagascar), OUTLINE_ENTRY(k_sri_lanka),
    OUTLINE_ENTRY(k_taiwan),     OUTLINE_ENTRY(k_png),
};
static const int k_outlineCount = sizeof(k_outlines)/sizeof(k_outlines[0]);

// Country center labels for hover
struct CountryCenter { float lat; float lon; const char* name; };
static const CountryCenter k_countries[] = {
    // Asia
    { 4.5f,  101.5f, "Malaysia"},       { 2.5f,  111.0f, "Malaysia"},
    {35.5f, 138.0f, "Japan"},
    {36.0f,  128.0f, "South Korea"},    {35.0f,  105.0f, "China"},
    {21.0f,   78.0f, "India"},          {15.5f,  101.0f, "Thailand"},
    {16.0f,  108.0f, "Vietnam"},        {12.5f,  105.0f, "Cambodia"},
    {19.0f,   97.0f, "Myanmar"},        { 1.3f,  103.8f, "Singapore"},
    {-2.0f,  118.0f, "Indonesia"},      {13.0f,  122.0f, "Philippines"},
    {23.7f,   90.4f, "Bangladesh"},     {28.4f,   84.1f, "Nepal"},
    {33.0f,   65.0f, "Afghanistan"},    {30.0f,   70.0f, "Pakistan"},
    { 7.0f,   80.0f, "Sri Lanka"},      {23.5f,  121.0f, "Taiwan"},
    {47.0f,  105.0f, "Mongolia"},       {37.0f,  127.5f, "North Korea"},
    {20.0f,  105.0f, "Laos"},
    // Middle East
    {24.0f,   45.0f, "Saudi Arabia"},   {32.0f,   54.0f, "Iran"},
    {33.0f,   44.0f, "Iraq"},           {26.0f,   50.5f, "Bahrain"},
    {25.3f,   51.2f, "Qatar"},          {24.0f,   54.0f, "UAE"},
    {21.0f,   57.0f, "Oman"},           {15.5f,   48.0f, "Yemen"},
    {35.0f,   38.0f, "Syria"},          {34.0f,   36.0f, "Lebanon"},
    {31.5f,   35.0f, "Israel"},         {31.0f,   36.0f, "Jordan"},
    {39.0f,   35.0f, "Turkey"},         {41.0f,   45.0f, "Georgia"},
    // Europe
    {48.8f,    2.3f, "France"},         {51.0f,   10.0f, "Germany"},
    {42.5f,   12.5f, "Italy"},          {40.0f,   -4.0f, "Spain"},
    {39.5f,   -8.0f, "Portugal"},       {54.0f,   -2.0f, "United Kingdom"},
    {53.0f,   -8.0f, "Ireland"},        {52.0f,    5.0f, "Netherlands"},
    {50.5f,    4.0f, "Belgium"},        {47.0f,    8.0f, "Switzerland"},
    {47.5f,   14.0f, "Austria"},        {49.8f,   15.5f, "Czech Republic"},
    {52.0f,   20.0f, "Poland"},         {47.5f,   19.0f, "Hungary"},
    {44.0f,   21.0f, "Serbia"},         {42.7f,   25.5f, "Bulgaria"},
    {44.5f,   26.0f, "Romania"},        {39.0f,   22.0f, "Greece"},
    {56.0f,   10.0f, "Denmark"},        {62.0f,   15.0f, "Sweden"},
    {64.0f,   26.0f, "Finland"},        {62.0f,   10.0f, "Norway"},
    {65.0f,  -18.0f, "Iceland"},        {46.0f,   25.0f, "Romania"},
    {48.7f,   19.7f, "Slovakia"},       {42.0f,   20.0f, "Kosovo"},
    {56.8f,   24.0f, "Latvia"},         {55.0f,   24.0f, "Lithuania"},
    {59.0f,   25.0f, "Estonia"},        {50.0f,   32.0f, "Ukraine"},
    // Africa
    { 0.0f,   38.0f, "Kenya"},          {-6.3f,   35.0f, "Tanzania"},
    {-1.3f,   36.8f, "Uganda"},         { 9.0f,    8.0f, "Nigeria"},
    {30.0f,    3.0f, "Algeria"},         {32.0f,   -5.0f, "Morocco"},
    {28.0f,   30.0f, "Egypt"},          {15.5f,   32.5f, "Sudan"},
    { 9.0f,   42.0f, "Ethiopia"},       {-14.0f,  34.0f, "Malawi"},
    {-29.0f,  24.0f, "South Africa"},   {-15.5f,  28.3f, "Zambia"},
    {-20.0f,  30.0f, "Zimbabwe"},       { 6.5f,   -1.5f, "Ghana"},
    {14.5f,  -14.5f, "Senegal"},        {-19.0f,  46.0f, "Madagascar"},
    {34.0f,    9.0f, "Tunisia"},        {-4.0f,   22.0f, "DR Congo"},
    { 7.5f,    1.5f, "Togo"},           {12.0f,  -15.0f, "Guinea"},
    // Americas
    {56.0f, -106.0f, "Canada"},         {39.0f,  -98.0f, "United States"},
    {23.0f, -102.0f, "Mexico"},         {-10.0f, -55.0f, "Brazil"},
    {-35.0f, -65.0f, "Argentina"},      {-33.5f, -71.0f, "Chile"},
    { 4.5f,  -74.0f, "Colombia"},       {-12.0f, -76.0f, "Peru"},
    {-17.0f, -65.0f, "Bolivia"},        {10.5f,  -66.9f, "Venezuela"},
    {-1.0f,  -78.0f, "Ecuador"},        {-33.0f, -56.0f, "Uruguay"},
    {-23.0f, -58.0f, "Paraguay"},       {18.5f,  -72.0f, "Haiti"},
    {19.0f,  -70.7f, "Dominican Republic"},
    {23.0f,  -82.0f, "Cuba"},           { 9.7f,  -84.0f, "Costa Rica"},
    {14.0f,  -87.0f, "Honduras"},       {13.2f,  -59.5f, "Barbados"},
    // Oceania
    {-25.0f, 134.0f, "Australia"},      {-42.0f, 172.0f, "New Zealand"},
    {-6.0f,  147.0f, "Papua New Guinea"},
    // Russia & Central Asia
    {60.0f,   100.0f, "Russia"},
    {41.0f,    65.0f, "Uzbekistan"},    {48.0f,    67.0f, "Kazakhstan"},
};
static const int k_countryCount = sizeof(k_countries)/sizeof(k_countries[0]);

static void RenderBranchMap(float mapW, float mapH) {
    const float PI = 3.14159265f;
    const float DEG2RAD = PI / 180.0f;

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 mapMin = origin;
    ImVec2 mapMax = ImVec2(origin.x + mapW, origin.y + mapH);
    ImDrawList* dl = ImGui::GetWindowDrawList();

    float cx = origin.x + mapW * 0.5f;
    float cy = origin.y + mapH * 0.5f;
    float radius = std::min(mapW, mapH) * g_globeRadius;

    // Dark space background
    dl->AddRectFilled(mapMin, mapMax, IM_COL32(2, 5, 15, 255));
    dl->PushClipRect(mapMin, mapMax, true);

    // Globe ocean sphere
    dl->AddCircleFilled(ImVec2(cx, cy), radius, IM_COL32(12, 25, 55, 255), 64);
    // Subtle highlight for 3D look
    dl->AddCircleFilled(ImVec2(cx - radius*0.18f, cy - radius*0.18f),
                        radius*0.55f, IM_COL32(20, 45, 90, 60), 48);

    // Atmosphere glow
    dl->AddCircle(ImVec2(cx, cy), radius + 2, IM_COL32(60, 130, 220, 35), 64, 3.0f);
    dl->AddCircle(ImVec2(cx, cy), radius + 6, IM_COL32(40, 90, 180, 15), 64, 2.0f);

    // Rotation parameters
    float cLatR = g_globeRotLat * DEG2RAD;
    float cosClat = std::cos(cLatR), sinClat = std::sin(cLatR);

    // Orthographic projection: geo (lat,lon) -> screen (x,y) + depth z
    auto GeoToGlobe = [&](float lat, float lon, float& outZ) -> ImVec2 {
        float latR = lat * DEG2RAD;
        float dLon = (lon - g_globeRotLon) * DEG2RAD;
        float cosLat = std::cos(latR), sinLat = std::sin(latR);
        float cosDlon = std::cos(dLon), sinDlon = std::sin(dLon);
        float x0 = cosLat * sinDlon;
        float y0 = cosClat * sinLat - sinClat * cosLat * cosDlon;
        float z0 = sinClat * sinLat + cosClat * cosLat * cosDlon;
        outZ = z0;
        return ImVec2(cx + x0 * radius, cy - y0 * radius);
    };

    // Grid lines (latitude circles)
    for (int lat = -60; lat <= 80; lat += 30) {
        ImVec2 prev; float prevZ = -1;
        for (int lon = -180; lon <= 180; lon += 3) {
            float z;
            ImVec2 p = GeoToGlobe((float)lat, (float)lon, z);
            if (prevZ > 0 && z > 0)
                dl->AddLine(prev, p, IM_COL32(30, 55, 80, 45), 0.5f);
            prev = p; prevZ = z;
        }
    }
    // Grid lines (longitude meridians)
    for (int lon = -180; lon < 180; lon += 30) {
        ImVec2 prev; float prevZ = -1;
        for (int lat = -90; lat <= 90; lat += 3) {
            float z;
            ImVec2 p = GeoToGlobe((float)lat, (float)lon, z);
            if (prevZ > 0 && z > 0)
                dl->AddLine(prev, p, IM_COL32(30, 55, 80, 45), 0.5f);
            prev = p; prevZ = z;
        }
    }

    // Land masses — filled polygons + outlines
    for (int oi = 0; oi < k_outlineCount; ++oi) {
        const Outline& ol = k_outlines[oi];
        // Collect visible projected points for fill
        std::vector<ImVec2> visPts;
        for (int j = 0; j < ol.count; ++j) {
            float z;
            ImVec2 p = GeoToGlobe(ol.pts[j][0], ol.pts[j][1], z);
            if (z > 0.0f) visPts.push_back(p);
        }
        if (visPts.size() >= 3)
            dl->AddConvexPolyFilled(visPts.data(), (int)visPts.size(),
                                    IM_COL32(25, 60, 35, 200));
        // Outline edges (only when both endpoints visible)
        for (int j = 0; j < ol.count - 1; ++j) {
            float z1, z2;
            ImVec2 p1 = GeoToGlobe(ol.pts[j][0], ol.pts[j][1], z1);
            ImVec2 p2 = GeoToGlobe(ol.pts[j+1][0], ol.pts[j+1][1], z2);
            if (z1 > 0 && z2 > 0)
                dl->AddLine(p1, p2, IM_COL32(50, 110, 70, 200), 1.0f);
        }
    }

    // Pins
    const char* hoveredLabel = nullptr;
    const char* hoveredCountry = nullptr;
    float hoveredLat = 0, hoveredLng = 0;
    char hoveredMfrs[512] = {};
    ImVec2 mousePos = ImGui::GetIO().MousePos;

    for (int i = 0; i < k_allPinsCount; ++i) {
        const GeoPin& pin = k_allPins[i];
        float z;
        ImVec2 sp = GeoToGlobe(pin.lat, pin.lng, z);
        if (z <= 0.05f) continue;  // behind the globe

        bool isMY = (strcmp(pin.country, "Malaysia") == 0);
        ImU32 pinCol  = isMY ? IM_COL32(0,200,120,255) : IM_COL32(255,100,60,255);
        ImU32 glowCol = isMY ? IM_COL32(0,200,120,50)  : IM_COL32(255,100,60,50);
        float pinR = 4.0f * (0.5f + 0.5f * z);  // perspective size
        dl->AddCircleFilled(sp, pinR + 3.0f, glowCol);
        dl->AddCircleFilled(sp, pinR, pinCol);
        dl->AddCircleFilled(sp, 1.5f, IM_COL32(255,255,255,220));

        // Label text (only when facing forward and globe is large enough)
        if (z > 0.3f && radius > 80)
            dl->AddText(ImVec2(sp.x + 7, sp.y - 6),
                        IM_COL32(200,210,220,(int)(180*z)), pin.label);

        float dx = mousePos.x - sp.x, dy = mousePos.y - sp.y;
        if (dx*dx + dy*dy < (pinR+6)*(pinR+6)) {
            hoveredLabel = pin.label;
            hoveredCountry = pin.country;
            hoveredLat = pin.lat; hoveredLng = pin.lng;
            hoveredMfrs[0] = '\0';
            int mfrCount = (int)(sizeof(k_manufacturers)/sizeof(k_manufacturers[0]));
            for (int mi = 0; mi < mfrCount; ++mi) {
                const BranchList& bl = k_branchesByMfr[mi];
                for (int bi = 0; bi < bl.count; ++bi) {
                    if (strstr(bl.branches[bi], pin.label)) {
                        if (hoveredMfrs[0] != '\0') strcat(hoveredMfrs, ", ");
                        strcat(hoveredMfrs, k_manufacturers[mi]);
                        break;
                    }
                }
            }
        }
    }

    // Country name hover — unproject mouse to lat/lon on sphere
    const char* hoveredCountryName = nullptr;
    {
        float mx = (mousePos.x - cx) / radius;
        float my = -(mousePos.y - cy) / radius;
        float r2 = mx * mx + my * my;
        if (r2 < 1.0f) {
            float mz = std::sqrt(1.0f - r2);
            // Inverse rotation to get lat/lon
            float sinLat = sinClat * mz + cosClat * my;
            if (sinLat > 1.0f) sinLat = 1.0f;
            if (sinLat < -1.0f) sinLat = -1.0f;
            float lat = std::asin(sinLat) / DEG2RAD;
            float cosLatM = cosClat * mz - sinClat * my;
            float lon = g_globeRotLon + std::atan2(mx, cosLatM) / DEG2RAD;
            // Find nearest country center
            float bestDist = 1e9f;
            for (int ci = 0; ci < k_countryCount; ++ci) {
                float dlat = k_countries[ci].lat - lat;
                float dlon = k_countries[ci].lon - lon;
                if (dlon > 180.0f) dlon -= 360.0f;
                if (dlon < -180.0f) dlon += 360.0f;
                float d = dlat*dlat + dlon*dlon;
                if (d < bestDist) { bestDist = d; hoveredCountryName = k_countries[ci].name; }
            }
            // Only show if reasonably close (threshold depends on zoom)
            float thresh = 400.0f / (g_globeRadius * g_globeRadius + 1.0f);
            if (thresh < 20.0f) thresh = 20.0f;
            if (bestDist > thresh) hoveredCountryName = nullptr;
        }
    }

    dl->PopClipRect();

    // Mouse interaction
    ImGui::SetCursorScreenPos(mapMin);
    ImGui::InvisibleButton("##globe", ImVec2(mapW, mapH));
    bool hov = ImGui::IsItemHovered();

    if (hov && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 d = ImGui::GetIO().MouseDelta;
        float sensitivity = 0.195f / g_globeRadius;
        if (sensitivity < 0.0065f) sensitivity = 0.0065f;
        g_globeRotLon -= d.x * sensitivity;
        g_globeRotLat += d.y * sensitivity;
        if (g_globeRotLat >  85.0f) g_globeRotLat =  85.0f;
        if (g_globeRotLat < -85.0f) g_globeRotLat = -85.0f;
    }
    if (hov) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            g_globeRadius *= (wheel > 0) ? 1.16f : 0.87f;
            if (g_globeRadius < 0.20f) g_globeRadius = 0.20f;
            if (g_globeRadius > 150.0f) g_globeRadius = 150.0f;
        }
    }

    // Tooltip for pins
    if (hoveredLabel && hov) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1,1,1,1), "%s", hoveredLabel);
        ImGui::TextColored(ImVec4(0.6f,0.7f,0.8f,1), "%s  (%.2f, %.2f)",
                           hoveredCountry, hoveredLat, hoveredLng);
        if (hoveredMfrs[0] != '\0') {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.4f,0.9f,0.6f,1), "Brands: %s", hoveredMfrs);
        }
        ImGui::EndTooltip();
    }
    // Tooltip for country name (only when not hovering a pin)
    else if (hoveredCountryName && hov && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(1,1,1,1), "%s", hoveredCountryName);
        ImGui::EndTooltip();
    }

    // --- Scale bar ---
    {
        // Earth radius ~ 3959 miles. 1 radian on sphere = 3959 miles.
        // On screen: 1 radian = radius pixels.
        // So miles_per_pixel = 3959 / radius.
        float milesPerPx = 3959.0f / radius;
        float targetBarPx = 80.0f;
        float rawMiles = targetBarPx * milesPerPx;
        // Pick nearest nice round number
        static const float nice[] = {1,2,5,10,20,50,100,200,500,1000,2000,5000,10000};
        float bestVal = nice[0];
        float bestDiff = 1e9f;
        for (int i = 0; i < 13; ++i) {
            float diff = std::abs(nice[i] - rawMiles);
            if (diff < bestDiff) { bestDiff = diff; bestVal = nice[i]; }
        }
        float barPx = bestVal / milesPerPx;
        if (barPx < 20.0f) barPx = 20.0f;
        if (barPx > 160.0f) barPx = 160.0f;

        char scaleBuf[64];
        if (bestVal >= 1.0f)
            snprintf(scaleBuf, sizeof(scaleBuf), "%.0f miles", bestVal);
        else
            snprintf(scaleBuf, sizeof(scaleBuf), "%.1f miles", bestVal);

        float sx = mapMax.x - barPx - 16.0f;
        float sy = mapMax.y - 18.0f;
        // Bar background
        dl->AddRectFilled(ImVec2(sx - 4, sy - 14), ImVec2(sx + barPx + 8, sy + 8),
                          IM_COL32(10, 20, 35, 200), 3.0f);
        // Bar line
        dl->AddLine(ImVec2(sx, sy), ImVec2(sx + barPx, sy), IM_COL32(200,210,220,200), 2.0f);
        // End caps
        dl->AddLine(ImVec2(sx, sy - 4), ImVec2(sx, sy + 4), IM_COL32(200,210,220,200), 1.5f);
        dl->AddLine(ImVec2(sx + barPx, sy - 4), ImVec2(sx + barPx, sy + 4),
                    IM_COL32(200,210,220,200), 1.5f);
        // Label
        dl->AddText(ImVec2(sx + 2, sy - 14), IM_COL32(200,210,220,220), scaleBuf);
    }

    // Legend (top-left)
    ImVec2 lp = ImVec2(mapMin.x + 6, mapMin.y + 6);
    dl->AddRectFilled(lp, ImVec2(lp.x + 195, lp.y + 28), IM_COL32(10,20,35,200), 4.0f);
    dl->AddCircleFilled(ImVec2(lp.x+12, lp.y+9), 4, IM_COL32(0,200,120,255));
    dl->AddText(ImVec2(lp.x+20, lp.y+2), IM_COL32(180,190,200,220), "Malaysia");
    dl->AddCircleFilled(ImVec2(lp.x+100, lp.y+9), 4, IM_COL32(255,100,60,255));
    dl->AddText(ImVec2(lp.x+108, lp.y+2), IM_COL32(180,190,200,220), "Origin HQ");
    dl->AddText(ImVec2(lp.x+6, lp.y+16), IM_COL32(120,130,140,180), "Drag to rotate, scroll to zoom");
}

static void RenderAddBlock(cw1::Blockchain& chain) {
    DrawSectionHeading("Add Block to Chain");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, COL_BG_HOVER);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);

    ImGui::TextColored(COL_MUTED, "COMMON FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    ImGui::SetNextItemWidth(240);
    ImGui::Combo("Stage##addstage", &g_formStage, k_stageNames, 5);
    ImGui::Spacing();

    
    auto LabelText = [](const char* lbl, char* buf, size_t sz) {
        ImGui::TextColored(COL_MUTED, "%-18s", lbl);
        ImGui::SameLine();
        char id[64]; snprintf(id, sizeof(id), "##add_%s", lbl);
        ImGui::SetNextItemWidth(300);
        ImGui::InputText(id, buf, sz);
    };

    LabelText("Car Plate Number", g_formVin,   sizeof(g_formVin));

    ImGui::SameLine(0, 12);
    if (g_formVin[0] != '\0') {
        // Build the full VIN with prefix for lookup
        std::string fullVin = g_formVin;
        if (fullVin.substr(0, 3) != "VIN") fullVin = "VIN" + fullVin;
        if (chain.hasVin(fullVin))
            ImGui::TextColored(COL_GREEN_BR, "[+ Existing: %s]", fullVin.c_str());
        else
            ImGui::TextColored(COL_ACCENT,   "[* New: %s]", fullVin.c_str());
    }

    ImGui::TextColored(COL_MUTED, "%-18s", "Manufacturer");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    if (ImGui::Combo("##add_Manufacturer", &g_formMfrIndex, k_manufacturers, IM_ARRAYSIZE(k_manufacturers))) {
        g_formModelIndex = 0;
        g_formBranchIndex = 0;
    }

    ImGui::TextColored(COL_MUTED, "%-18s", "Model");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    const ModelList& ml = k_modelsByMfr[g_formMfrIndex];
    ImGui::Combo("##add_Model", &g_formModelIndex, ml.models, ml.count);

    ImGui::TextColored(COL_MUTED, "%-18s", "Branch");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(380);
    const BranchList& bl = k_branchesByMfr[g_formMfrIndex];
    ImGui::Combo("##add_Branch", &g_formBranchIndex, bl.branches, bl.count);

    ImGui::TextColored(COL_MUTED, "%-18s", "Color");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::Combo("##add_Color", &g_formColorIndex, k_colors, IM_ARRAYSIZE(k_colors));

    ImGui::TextColored(COL_MUTED, "%-18s", "Production Year");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##add_year", &g_formYear);
    if (g_formYear < 1900) g_formYear = 1900;

    if (g_formYear > 2026) g_formYear = 2026;
    ImGui::Spacing(); ImGui::Spacing();
    LabelText("Manufacturer ID", g_formMfrId, sizeof(g_formMfrId));
    ImGui::TextColored(COL_MUTED, "STAGE-SPECIFIC FIELDS");
    ImGui::Separator(); ImGui::Spacing();

    switch (g_formStage) {
    case 0: {
        const char* selBranch = k_branchesByMfr[g_formMfrIndex].branches[g_formBranchIndex];
        ImGui::TextColored(COL_MUTED, "%-18s", "Factory Location");
        ImGui::SameLine();
        ImGui::TextColored(COL_ACCENT, "%s", selBranch);
        break;
    }
    case 1: 
        LabelText("Warehouse",  g_formWarehouse,   sizeof(g_formWarehouse));
        LabelText("Received By",g_formReceivedBy,  sizeof(g_formReceivedBy));
        LabelText("Supplier ID", g_formSupplierId, sizeof(g_formSupplierId));
        break;
    case 2: 
        LabelText("Inspector ID", g_formInspector, sizeof(g_formInspector));
        ImGui::TextColored(COL_MUTED, "%-18s", "Passed");
        ImGui::SameLine();
        ImGui::Checkbox("##add_passed", &g_formPassed);
        LabelText("QC Notes", g_formQcNotes, sizeof(g_formQcNotes));
        break;
    case 3:
        LabelText("Dealer ID",    g_formDealerId,   sizeof(g_formDealerId));
        LabelText("Destination",  g_formDestination,sizeof(g_formDestination));
        ImGui::TextColored(COL_MUTED, "%-18s", "Transport Mode");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300);
        ImGui::Combo("##add_Transport", &g_formTransportIndex, k_transportModes, IM_ARRAYSIZE(k_transportModes));
        break;
    case 4: 
        LabelText("Buyer ID",     g_formBuyerId, sizeof(g_formBuyerId));
        LabelText("Retailer ID",  g_formRetailerId, sizeof(g_formRetailerId));
        ImGui::TextColored(COL_MUTED, "%-18s", "Sale Price (MYR)");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputDouble("##add_price", &g_formSalePrice, 1000.0, 10000.0, "%.2f");
        LabelText("Warranty Expiry", g_formWarranty, sizeof(g_formWarranty));
        break;
    }

    ImGui::Spacing(); ImGui::Spacing();

    bool submit = DrawPrimaryButton("  Add Block  ", ImVec2(150, 36));

    bool canSubmit = (g_formVin[0] != '\0');

    if (submit && !canSubmit) {
        ImGui::SameLine();
        ImGui::TextColored(COL_RED, "  Car Plate Number is required.");
    }

    if (submit && canSubmit) {
        cw1::CarRecord r;
        // Auto-prepend "VIN" if the user didn't type it
        {
            std::string raw = g_formVin;
            if (raw.substr(0, 3) != "VIN") raw = "VIN" + raw;
            r.vin = raw;
        }
        r.manufacturer   = k_manufacturers[g_formMfrIndex];
        r.model          = k_modelsByMfr[g_formMfrIndex].models[g_formModelIndex];
        r.color          = k_colors[g_formColorIndex];
        r.productionYear = g_formYear;
        r.stage          = static_cast<cw1::BlockStage>(g_formStage);
        r.manufacturerId = g_formMfrId;

        switch (g_formStage) {
        case 0:
            r.factoryLocation  = k_branchesByMfr[g_formMfrIndex].branches[g_formBranchIndex];
            break;
        case 1:
            r.warehouseLocation = g_formWarehouse;
            r.receivedBy        = g_formReceivedBy;
            r.supplierId        = g_formSupplierId;
            break;
        case 2:
            r.inspectorId = g_formInspector;
            r.passed      = g_formPassed;
            r.qcNotes     = g_formQcNotes;
            break;
        case 3:
            r.dealerId      = g_formDealerId;
            r.destination   = g_formDestination;
            r.transportMode = k_transportModes[g_formTransportIndex];
            break;
        case 4:
            r.buyerId        = g_formBuyerId;
            r.retailerId     = g_formRetailerId;
            r.salePrice      = g_formSalePrice;
            r.warrantyExpiry = g_formWarranty;
            break;
        }

        const double seconds = cw1::measureSeconds([&]() {
            chain.addBlock(r);
        });
        g_lastAddBlockSeconds = seconds;
        PushToast("Block #" + std::to_string(chain.totalBlocks() - 1) +
                  " added for " + r.vin +
                  " | Operation took: " + cw1::formatSeconds(seconds) + " s",
                  COL_GREEN_BR);


        g_formVin[0]    = '\0';
        g_formMfrIndex  = 0;  g_formModelIndex = 0;  g_formBranchIndex = 0;  g_formColorIndex = 0;
        g_formYear      = 2025;
        g_formFactory[0]= '\0'; g_formWarehouse[0]    = '\0';
        g_formReceivedBy[0]= '\0'; g_formInspector[0] = '\0';
        g_formPassed    = true;  g_formQcNotes[0]     = '\0';
        g_formDealerId[0]= '\0'; g_formDestination[0] = '\0';
        g_formTransportIndex = 0; g_formBuyerId[0]    = '\0';
        g_formSalePrice = 0.0;   g_formWarranty[0]    = '\0';
        g_formMfrId[0] = '\0'; g_formSupplierId[0] = '\0'; g_formRetailerId[0] = '\0';
    }

    if (g_lastAddBlockSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last add-block operation took: %s s",
                           cw1::formatSeconds(g_lastAddBlockSeconds).c_str());
    }

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}





static void RenderGlobalChain(const cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();

    DrawSectionHeading("Global Chain");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%zu block(s))", blocks.size());
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "VISUAL LINKAGE");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##global_visual_link", ImVec2(-1.0f, 300.0f), true);
    ImGui::PopStyleColor();

    if (blocks.empty()) {
        ImGui::TextColored(COL_MUTED, "No blocks in the chain.");
    } else {
        for (size_t i = 0; i < blocks.size(); ++i) {
            const cw1::Block& b = blocks[i];
            const cw1::CarRecord& rec = b.getRecord();
            const ImVec2 cardMin = ImGui::GetCursorScreenPos();
            const float cardWidth = ImGui::GetContentRegionAvail().x;
            const float lineH = ImGui::GetTextLineHeightWithSpacing();
            const float cardHeight = (lineH * 5.0f) + 20.0f;
            const ImVec2 cardMax(cardMin.x + cardWidth, cardMin.y + cardHeight);
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRectFilled(cardMin, cardMax,
                ImGui::ColorConvertFloat4ToU32(COL_BG_ELEV), 8.0f);
            dl->AddRect(cardMin, cardMax,
                ImGui::ColorConvertFloat4ToU32(COL_BORDER_SOFT), 8.0f);

            ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 12.0f, cardMin.y + 9.0f));
            ImGui::BeginGroup();
            ImGui::TextColored(COL_ACCENT, "Block #%zu", b.getIndex());
            ImGui::TextColored(COL_PURPLE, "Hash: %s", Truncate(b.getCurrentHash(), 8).c_str());
            ImGui::TextColored(COL_PURPLE, "Prev: %s", Truncate(b.getPreviousHash(), 8).c_str());
            ImGui::TextColored(StageColor(rec.stage), "Stage: %s", cw1::stageToString(rec.stage).c_str());
            ImGui::TextColored(COL_ACCENT, "VIN: %s", rec.vin.c_str());
            ImGui::EndGroup();

            ImGui::SetCursorScreenPos(ImVec2(cardMin.x, cardMax.y + 8.0f));

            if (i + 1 < blocks.size()) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14.0f);
                ImGui::TextColored(COL_MUTED, "v");
            }
        }
    }

    ImGui::EndChild();
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "TABULAR CHAIN DATA");
    ImGui::Spacing();

    ImGuiTableFlags tf = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                         ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
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
                ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", b.getCurrentHash().c_str());
                ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", b.getSha3Hash().c_str());
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
    ImGui::PopStyleVar();
    LogViewOnce(chain, View::GLOBAL_CHAIN, "",
                "Global chain viewed");
}





static void RenderAuditLog(const cw1::Blockchain& chain) {
    DrawSectionHeading("Audit Log");
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

    
    cw1::RecentEntryArray entries(chain.getAuditLog(),
                                  static_cast<std::size_t>(g_auditN));

    auto ActionColor = [](cw1::AuditAction act) -> ImVec4 {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return COL_GREEN_BR;
        case cw1::AuditAction::INTEGRITY_CHECK:  return COL_YELLOW;
        case cw1::AuditAction::SEARCH_PERFORMED: return COL_ACCENT;
        case cw1::AuditAction::CHAIN_VIEWED:     return COL_PURPLE;
        case cw1::AuditAction::TAMPER_SIMULATED: return COL_RED;
        case cw1::AuditAction::PERSISTENCE_IO:   return COL_ORANGE;
        case cw1::AuditAction::BLOCK_DELETED:    return COL_RED;
        default:                                  return COL_TEXT;
        }
    };
    auto ActionName = [](cw1::AuditAction act) -> const char* {
        switch (act) {
        case cw1::AuditAction::BLOCK_ADDED:      return "BLOCK_ADDED";
        case cw1::AuditAction::INTEGRITY_CHECK:  return "INTEGRITY_CHECK";
        case cw1::AuditAction::SEARCH_PERFORMED: return "SEARCH_PERFORMED";
        case cw1::AuditAction::CHAIN_VIEWED:     return "CHAIN_VIEWED";
        case cw1::AuditAction::TAMPER_SIMULATED: return "TAMPER_SIMULATED";
        case cw1::AuditAction::PERSISTENCE_IO:   return "PERSISTENCE_IO";
        case cw1::AuditAction::BLOCK_DELETED:    return "BLOCK_DELETED";
        default:                                  return "UNKNOWN";
        }
    };

    ImGuiTableFlags tf = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                         ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_ScrollY  | ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    if (ImGui::BeginTable("##auditlog", 3, tf, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Action",    ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Details",   ImGuiTableColumnFlags_WidthStretch, 4.0f);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < entries.size(); ++i) {
            const cw1::AuditEntry* e = entries[i];
            if (e == nullptr) {
                continue;
            }
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
    ImGui::PopStyleVar();
}





static void RenderIntegrity(cw1::Blockchain& chain) {
    DrawSectionHeading("Blockchain Characteristics: Immutability");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // Initialize temp chain from real chain on first visit (so chain is always visible)
    if (g_tempChain.empty() && !g_tempGenerating) {
        g_tempChain = chain.getChain();
        g_tempRealCount = g_tempChain.size();
        g_tempPrevCount = g_tempChain.size();
    }

    // 1. Start/Stop Toggle Button
    if (!g_tempGenerating) {
        ImGui::PushStyleColor(ImGuiCol_Button,        HexColor(0x1b4d30));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  HexColor(0x238636));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,   HexColor(0x2ea043));
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
    } else {
        if (DrawDangerButton("Stop Generating##tempstop", ImVec2(220.0f, 36.0f))) {
            g_tempGenerating = false;
            PushToast("Temp block auto-generation stopped.", COL_YELLOW);
        }
    }

    // 2. Status display
    if (!g_tempChain.empty()) {
        ImGui::Spacing();
        std::size_t genCount = g_tempChain.size() - g_tempRealCount;
        ImGui::TextColored(COL_TEXT, "Temp chain: %zu blocks (%zu real + %zu generated)  |  Generating: %s",
                           g_tempChain.size(), g_tempRealCount, genCount,
                           g_tempGenerating ? "ON" : "OFF");
    }

    // 3. Interval slider
    if (g_tempGenerating || !g_tempChain.empty()) {
        ImGui::Spacing();
        ImGui::SetNextItemWidth(240.0f);
        ImGui::SliderFloat("Generation interval (s)##tempinterval", &g_tempInterval, 1.0f, 10.0f, "%.1f s");
    }

    // 4. Tamper controls for temp chain
    if (!g_tempChain.empty()) {
        ImGui::Spacing(); ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "TAMPER TEMP CHAIN");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::SetNextItemWidth(140.0f);
        ImGui::InputInt("Tamper temp block index##temptamperidx", &g_tempTamperIndex);
        ImGui::SameLine();

        bool tamperValid = (g_tempTamperIndex >= 0 &&
                            static_cast<std::size_t>(g_tempTamperIndex) < g_tempChain.size());

        ImGui::BeginDisabled(!tamperValid);
        if (DrawDangerButton("Tamper Data##temptamperdata", ImVec2(140.0f, 34.0f))) {
            g_tempChain[static_cast<std::size_t>(g_tempTamperIndex)]
                .debugTamperPayloadForSimulation("TAMPERED-IMMUTABILITY-DEMO");
            PushToast("Temp block #" + std::to_string(g_tempTamperIndex) + " payload tampered.", COL_RED);
        }
        ImGui::SameLine();
        if (DrawDangerButton("Tamper Hash##temptamperhash", ImVec2(140.0f, 34.0f))) {
            g_tempChain[static_cast<std::size_t>(g_tempTamperIndex)]
                .debugOverrideCurrentHash("000000000000tampered");
            PushToast("Temp block #" + std::to_string(g_tempTamperIndex) + " hash overridden.", COL_RED);
        }
        ImGui::EndDisabled();

        // 5. Delete temp block control
        ImGui::Spacing();
        ImGui::SetNextItemWidth(140.0f);
        ImGui::InputInt("Delete temp block index##tempdelidx", &g_tempDeleteIndex);
        ImGui::SameLine();

        bool delValid = (g_tempDeleteIndex >= 0 &&
                         static_cast<std::size_t>(g_tempDeleteIndex) < g_tempChain.size() &&
                         g_tempChain.size() > 1);

        ImGui::BeginDisabled(!delValid);
        if (DrawDangerButton("Delete Temp Block##tempdel", ImVec2(180.0f, 34.0f))) {
            std::size_t di = static_cast<std::size_t>(g_tempDeleteIndex);
            g_tempChain.erase(g_tempChain.begin() + static_cast<std::ptrdiff_t>(di));
            // Re-index without fixing hashes -- deliberately breaks the chain.
            for (std::size_t ri = di; ri < g_tempChain.size(); ++ri) {
                g_tempChain[ri].setIndex(ri);
            }
            if (g_tempRealCount > g_tempChain.size()) {
                g_tempRealCount = g_tempChain.size();
            }
            PushToast("Temp block #" + std::to_string(g_tempDeleteIndex) +
                      " deleted (chain links NOT repaired).", COL_RED);
        }
        ImGui::EndDisabled();

        // 6. Verify temp chain button
        ImGui::Spacing(); ImGui::Spacing();
        if (DrawPrimaryButton("Verify Temp Chain##tempverify", ImVec2(220.0f, 36.0f))) {
            cw1::OperationTimer timer;
            g_tempVerify = cw1::Validation::verifyChain(g_tempChain);
            g_tempVerifySeconds = timer.elapsedSeconds();
            g_tempVerifyDone = true;
            PushToast(g_tempVerify.ok ? "Blockchain verified OK."
                                      : "Blockchain integrity FAILED.",
                      g_tempVerify.ok ? COL_GREEN_BR : COL_RED);
        }

        // 7. Verification result display
        if (g_tempVerifyDone) {
            ImGui::Spacing();
            const ImVec4 tBannerCol = g_tempVerify.ok ? HexColor(0x1b4d30) : HexColor(0x5a2429);
            const char* tBannerTxt = g_tempVerify.ok ? "BLOCKCHAIN VERIFIED" : "BLOCKCHAIN INTEGRITY FAILURE";
            ImGui::PushStyleColor(ImGuiCol_ChildBg, tBannerCol);
            ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
            ImGui::BeginChild("##tempintbanner", ImVec2(-1, 60), true);
            ImGui::SetCursorPos(ImVec2(20.0f, 14.0f));
            ImGui::SetWindowFontScale(1.4f);
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", tBannerTxt);
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

            if (!g_tempVerify.ok && !g_tempVerify.failedIndices.empty()) {
                ImGui::Spacing();
                ImGui::TextColored(COL_RED, "Failed blocks detail:");
                ImGui::Spacing();

                ImGuiTableFlags detFlags = ImGuiTableFlags_BordersInnerV |
                                           ImGuiTableFlags_BordersInnerH |
                                           ImGuiTableFlags_RowBg |
                                           ImGuiTableFlags_SizingStretchProp;
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
                if (ImGui::BeginTable("##tempfaildetail", 4, detFlags, ImVec2(-1, 0))) {
                    ImGui::TableSetupColumn("Block #",       ImGuiTableColumnFlags_WidthFixed, 70);
                    ImGui::TableSetupColumn("Stored Hash",   ImGuiTableColumnFlags_WidthStretch, 2.0f);
                    ImGui::TableSetupColumn("Expected Hash", ImGuiTableColumnFlags_WidthStretch, 2.0f);
                    ImGui::TableSetupColumn("Failure Type",  ImGuiTableColumnFlags_WidthStretch, 1.5f);
                    ImGui::TableHeadersRow();

                    for (std::size_t fi : g_tempVerify.failedIndices) {
                        if (fi >= g_tempChain.size()) continue;
                        const cw1::Block& fb = g_tempChain[fi];
                        std::string expected = fb.computeHash();
                        std::string failType;

                        if (fb.getCurrentHash() != expected) {
                            failType = "Hash mismatch";
                        } else if (fb.getSha3Hash() != fb.computeSha3Hash()) {
                            failType = "SHA3-128 mismatch";
                        } else if (fi == 0 && fb.getPreviousHash() != fb.getCurrentHash()) {
                            failType = "Genesis rule";
                        } else if (fi > 0) {
                            failType = "Chain link mismatch";
                        }

                        ImGui::TableNextRow();
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                                               ImGui::ColorConvertFloat4ToU32(HexColor(0x5a2429, 0.4f)));

                        ImGui::TableNextColumn();
                        ImGui::TextColored(COL_RED, "#%zu", fi);
                        ImGui::TableNextColumn();
                        ImGui::TextColored(COL_MUTED, "%s", Truncate(fb.getCurrentHash(), 16).c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextColored(COL_MUTED, "%s", Truncate(expected, 16).c_str());
                        ImGui::TableNextColumn();
                        ImGui::TextColored(COL_RED, "X  %s", failType.c_str());
                    }
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();
            }
        }

        // 8. Temp chain -- horizontal scrolling chain view
        ImGui::Spacing(); ImGui::Spacing();
        ImGui::TextColored(COL_MUTED, "TEMP CHAIN BLOCKS");
        ImGui::Separator();
        ImGui::Spacing();

        // Detect new blocks for fade-in animation
        bool scrollToEnd = false;
        if (g_tempChain.size() != g_tempPrevCount) {
            if (g_tempChain.size() > g_tempPrevCount) {
                g_tempNewBlockAnim = 0.0f; // start fade-in
                scrollToEnd = true;
            }
            g_tempPrevCount = g_tempChain.size();
        }
        // Advance animation (0 -> 1 over ~0.5s)
        if (g_tempNewBlockAnim < 1.0f) {
            g_tempNewBlockAnim += ImGui::GetIO().DeltaTime * 2.0f;
            if (g_tempNewBlockAnim > 1.0f) g_tempNewBlockAnim = 1.0f;
        }

        const float cardW = 170.0f;
        const float cardH = 130.0f;
        const float arrowW = 30.0f;
        const float chainHeight = cardH + 20.0f;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##tempchainscroll", ImVec2(-1.0f, chainHeight), true,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PopStyleColor();

        if (g_tempChain.empty()) {
            ImGui::TextColored(COL_MUTED, "No blocks in temp chain.");
        } else {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            const float startY = ImGui::GetCursorScreenPos().y + 5.0f;

            for (std::size_t ti = 0; ti < g_tempChain.size(); ++ti) {
                const cw1::Block& tb = g_tempChain[ti];
                const cw1::CarRecord& trec = tb.getRecord();

                bool isFailed = false;
                if (g_tempVerifyDone) {
                    for (std::size_t fi : g_tempVerify.failedIndices) {
                        if (fi == ti) { isFailed = true; break; }
                    }
                }

                // Fade-in for the newest block
                bool isNewest = (ti == g_tempChain.size() - 1);
                float alpha = (isNewest && g_tempNewBlockAnim < 1.0f) ? g_tempNewBlockAnim : 1.0f;

                ImVec2 cardMin = ImGui::GetCursorScreenPos();
                cardMin.y = startY;
                ImVec2 cardMax(cardMin.x + cardW, cardMin.y + cardH);

                // Card background
                ImVec4 bgCol = isFailed ? HexColor(0x5a2429, 0.6f * alpha) : HexColor(0x21262d, alpha);
                ImVec4 borderCol = isFailed ? HexColor(0xda3633, 0.8f * alpha) : HexColor(0x30363d, 0.8f * alpha);
                dl->AddRectFilled(cardMin, cardMax,
                    ImGui::ColorConvertFloat4ToU32(bgCol), 6.0f);
                dl->AddRect(cardMin, cardMax,
                    ImGui::ColorConvertFloat4ToU32(borderCol), 6.0f);

                // Card content
                ImGui::SetCursorScreenPos(ImVec2(cardMin.x + 8.0f, cardMin.y + 6.0f));
                ImGui::BeginGroup();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));

                ImGui::TextColored(ImVec4(COL_ACCENT.x, COL_ACCENT.y, COL_ACCENT.z, alpha),
                                   "Block #%zu", tb.getIndex());
                ImGui::TextColored(ImVec4(COL_PURPLE.x, COL_PURPLE.y, COL_PURPLE.z, alpha),
                                   "Hash: %s", Truncate(tb.getCurrentHash(), 10).c_str());
                ImGui::TextColored(ImVec4(COL_PURPLE.x, COL_PURPLE.y, COL_PURPLE.z, alpha * 0.7f),
                                   "Prev: %s", Truncate(tb.getPreviousHash(), 10).c_str());
                ImGui::TextColored(ImVec4(COL_TEXT.x, COL_TEXT.y, COL_TEXT.z, alpha),
                                   "%s", trec.vin.c_str());
                ImGui::TextColored(ImVec4(StageColor(trec.stage).x, StageColor(trec.stage).y,
                                          StageColor(trec.stage).z, alpha),
                                   "%s", cw1::stageToString(trec.stage).c_str());

                // Status line
                if (g_tempVerifyDone) {
                    if (isFailed)
                        ImGui::TextColored(ImVec4(COL_RED.x, COL_RED.y, COL_RED.z, alpha), "FAILED");
                    else
                        ImGui::TextColored(ImVec4(COL_GREEN_BR.x, COL_GREEN_BR.y, COL_GREEN_BR.z, alpha), "OK");
                }

                ImGui::PopStyleVar();
                ImGui::EndGroup();

                // Tooltip on hover with full hashes
                if (ImGui::IsMouseHoveringRect(cardMin, cardMax)) {
                    ImGui::BeginTooltip();
                    ImGui::TextColored(COL_ACCENT, "Block #%zu", tb.getIndex());
                    ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", tb.getCurrentHash().c_str());
                    ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", tb.getSha3Hash().c_str());
                    ImGui::TextColored(COL_PURPLE, "Prev:     %s", tb.getPreviousHash().c_str());
                    ImGui::EndTooltip();
                }

                // Move cursor past the card
                ImGui::SetCursorScreenPos(ImVec2(cardMax.x, startY));

                // Draw arrow to next block
                if (ti + 1 < g_tempChain.size()) {
                    float arrowCenterY = startY + cardH * 0.5f;
                    // Arrow line
                    dl->AddLine(ImVec2(cardMax.x + 4.0f, arrowCenterY),
                                ImVec2(cardMax.x + arrowW - 6.0f, arrowCenterY),
                                ImGui::ColorConvertFloat4ToU32(COL_MUTED), 2.0f);
                    // Arrow head
                    dl->AddTriangleFilled(
                        ImVec2(cardMax.x + arrowW - 6.0f, arrowCenterY - 5.0f),
                        ImVec2(cardMax.x + arrowW - 6.0f, arrowCenterY + 5.0f),
                        ImVec2(cardMax.x + arrowW, arrowCenterY),
                        ImGui::ColorConvertFloat4ToU32(COL_MUTED));
                    ImGui::SetCursorScreenPos(ImVec2(cardMax.x + arrowW, startY));
                }

                // Auto-scroll to newest block
                if (isNewest && scrollToEnd) {
                    ImGui::SetScrollHereX(1.0f);
                }
            }

            // Reserve total width so scrollbar covers the full chain
            float totalW = g_tempChain.size() * cardW + (g_tempChain.size() - 1) * arrowW;
            ImGui::SetCursorPos(ImVec2(totalW, 0));
            ImGui::Dummy(ImVec2(1, 1));
        }

        ImGui::EndChild();
    }

    // 9. Reset button
    if (!g_tempChain.empty()) {
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
}





static void RenderDeleteBlock(cw1::Blockchain& chain) {
    DrawSectionHeading("Delete Block");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Chain size: %zu block(s)", chain.totalBlocks());
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "Block index to delete:");
    ImGui::SetNextItemWidth(160.0f);
    ImGui::InputInt("##delete_block_index", &g_deleteBlockIndex);
    ImGui::Spacing();

    if (DrawDangerButton("Soft Delete (Tombstone)##soft_del", ImVec2(220.0f, 36.0f))) {
        if (g_deleteBlockIndex < 0 ||
            static_cast<std::size_t>(g_deleteBlockIndex) >= chain.totalBlocks()) {
            PushToast("Invalid block index.", COL_RED);
        } else {
            std::string msg;
            cw1::OperationTimer timer;
            const bool ok = chain.softDeleteBlock(
                static_cast<std::size_t>(g_deleteBlockIndex), msg);
            g_lastDeleteSeconds = timer.elapsedSeconds();

            PushToast(ok ? msg : ("Soft delete failed: " + msg),
                      ok ? COL_GREEN_BR : COL_RED);

            cw1::OperationTimer vt;
            g_lastVerify = chain.verifyIntegrity();
            g_lastVerifySeconds = vt.elapsedSeconds();
            g_verifyDone = true;
        }
    }

    if (g_lastDeleteSeconds > 0.0) {
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED, "Last delete operation took: %s s",
                           cw1::formatSeconds(g_lastDeleteSeconds).c_str());
    }
}





static void RenderToasts(float deltaTime) {
    
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





int main() {
    
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

    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ApplyGitHubDarkTheme();

    
    {
        auto TryLoadFont = [&](const char* path, float sizePx) -> ImFont* {
            FILE* fp = fopen(path, "rb");
            if (fp == nullptr) {
                return nullptr;
            }
            fclose(fp);
            return io.Fonts->AddFontFromFileTTF(path, sizePx);
        };

#ifdef _WIN32
        g_fontBody = TryLoadFont("C:/Windows/Fonts/segoeui.ttf", 16.0f);
        g_fontSection = TryLoadFont("C:/Windows/Fonts/segoeuib.ttf", 22.0f);
        g_fontTitle = TryLoadFont("C:/Windows/Fonts/segoeuib.ttf", 26.0f);
#endif

        if (g_fontBody == nullptr) {
            g_fontBody = TryLoadFont("fonts/SegoeUI.ttf", 16.0f);
        }
        if (g_fontSection == nullptr) {
            g_fontSection = TryLoadFont("fonts/SegoeUI-Bold.ttf", 22.0f);
        }
        if (g_fontTitle == nullptr) {
            g_fontTitle = TryLoadFont("fonts/SegoeUI-Bold.ttf", 26.0f);
        }

        if (g_fontBody == nullptr) {
            g_fontBody = TryLoadFont("fonts/JetBrainsMono-Regular.ttf", 16.0f);
        }
        if (g_fontSection == nullptr) {
            g_fontSection = TryLoadFont("fonts/JetBrainsMono-Regular.ttf", 22.0f);
        }
        if (g_fontTitle == nullptr) {
            g_fontTitle = TryLoadFont("fonts/JetBrainsMono-Regular.ttf", 26.0f);
        }

        if (g_fontBody == nullptr) {
            g_fontBody = io.Fonts->AddFontDefault();
        }
        if (g_fontSection == nullptr) {
            g_fontSection = g_fontBody;
        }
        if (g_fontTitle == nullptr) {
            g_fontTitle = g_fontSection;
        }
        io.FontDefault = g_fontBody;
    }


    cw1::Blockchain chain;
    chain.openDatabase("database/cw1_blockchain.db");
    bool loadedFromDB = false;
    if (chain.isDatabaseOpen()) {
        loadedFromDB = chain.loadFromDB() && chain.totalBlocks() > 0;
    }
    if (!loadedFromDB) {
        loadDemoData(chain);
        if (chain.isDatabaseOpen()) {
            chain.saveToDB();
        }
    }
    g_lastVerifySeconds = cw1::measureSeconds([&]() {
        g_lastVerify = chain.verifyIntegrity();
    });
    g_verifyDone = true;

    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // -- Temp block auto-generation --
        if (g_tempGenerating && !g_tempChain.empty()) {
            g_tempAccumulator += io.DeltaTime;
            if (g_tempAccumulator >= g_tempInterval) {
                g_tempAccumulator -= g_tempInterval;

                // Use std::rand for random car generation (spec requirement).
                static bool seededGui = false;
                if (!seededGui) {
                    std::srand(static_cast<unsigned int>(std::time(nullptr)));
                    seededGui = true;
                }
                constexpr int mfrCount = 18;
                constexpr int colorCount = 12;
                const int mfrIdx = std::rand() % mfrCount;
                const ModelList& ml = k_modelsByMfr[mfrIdx];
                const int modelIdx = std::rand() % ml.count;
                const int colorIdx = std::rand() % colorCount;

                cw1::CarRecord r;
                r.vin = "TEMP-" + std::to_string(g_tempChain.size());
                r.manufacturer = k_manufacturers[mfrIdx];
                r.model = ml.models[modelIdx];
                r.color = k_colors[colorIdx];
                r.productionYear = 2024 + (std::rand() % 3);
                r.manufacturerId = "MFR-TEMP";
                r.stage = static_cast<cw1::BlockStage>(std::rand() % 5);

                switch (r.stage) {
                case cw1::BlockStage::PRODUCTION:
                    r.factoryLocation = "Demo Factory";
                    break;
                case cw1::BlockStage::WAREHOUSE_INTAKE:
                    r.warehouseLocation = "WH-TEMP";
                    r.receivedBy = "Demo Receiver";
                    r.supplierId = "SUP-TEMP";
                    break;
                case cw1::BlockStage::QUALITY_CHECK:
                    r.inspectorId = "QC-TEMP";
                    r.passed = (std::rand() % 2 == 0);
                    r.qcNotes = "Auto-generated QC";
                    break;
                case cw1::BlockStage::DEALER_DISPATCH:
                    r.dealerId = "DLR-TEMP";
                    r.destination = "Demo City";
                    r.transportMode = k_transportModes[std::rand() % 4];
                    break;
                case cw1::BlockStage::CUSTOMER_SALE:
                    r.buyerId = "CUST-TEMP";
                    r.retailerId = "RTL-TEMP";
                    r.salePrice = 30000.0 + static_cast<double>(std::rand() % 70000);
                    r.warrantyExpiry = "2030-01-01";
                    break;
                }

                const std::string prevHash = g_tempChain.back().getCurrentHash();
                g_tempChain.emplace_back(g_tempChain.size(), prevHash, r);
                PushToast("Temp block #" + std::to_string(g_tempChain.size() - 1) +
                          " generated (not saved to DB)", COL_ACCENT);
            }
        }


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

        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
        ImGui::BeginChild("##sbwrap", ImVec2(320, -1), false);
        ImGui::PopStyleColor();
        RenderSidebar(chain);
        ImGui::EndChild();

        ImGui::SameLine();

        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_MAIN);
        ImGui::BeginChild("##content", ImVec2(-1, -1), false,
                          ImGuiWindowFlags_NoScrollbar);
        // Inner scrollable region — avoids scrollbar flicker from size oscillation
        ImGui::BeginChild("##contentInner", ImVec2(-1, -1), false);
        ImGui::PopStyleColor();
        ImGui::Spacing(); ImGui::Spacing();

        switch (g_view) {
        case View::DASHBOARD:    RenderDashboard(chain);    break;
        case View::CAR_DETAIL:   RenderCarDetail(chain);    break;
        case View::ADD_BLOCK:    RenderAddBlock(chain);     break;
        case View::GLOBAL_CHAIN: RenderGlobalChain(chain);  break;
        case View::AUDIT_LOG:    RenderAuditLog(chain);     break;
        case View::INTEGRITY:    RenderIntegrity(chain);    break;
        case View::DELETE_BLOCK: RenderDeleteBlock(chain);  break;
        }

        ImGui::EndChild(); // ##contentInner
        ImGui::EndChild(); // ##content
        ImGui::End();


        RenderToasts(io.DeltaTime);

        
        ImGui::Render();
        int dispW, dispH;
        glfwGetFramebufferSize(window, &dispW, &dispH);
        glViewport(0, 0, dispW, dispH);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);  
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
