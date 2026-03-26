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
#include "utils/BranchMap.hpp"
#include "utils/FuelPriceManager.hpp"
#include "utils/OperationTimer.hpp"
#include "utils/VehicleData.hpp"






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


static void LogViewOnce(cw1::Blockchain& chain, View currentView,
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

// -- Fuel Intelligence state --
static cw1::FuelPriceManager g_fuelMgr;
static bool g_fuelInitialized = false;


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








// RenderBranchMap is now in utils/BranchMap.hpp

// ---------------------------------------------------------------------------
// Dashboard sub-components
// ---------------------------------------------------------------------------

static void DrawStatCard(const char* value, const char* label, ImVec4 col, ImVec2 sz) {
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
}

static void DrawHashWithTooltip(const cw1::Block& b, size_t maxLen = 12) {
    ImGui::TextColored(COL_PURPLE, "%s", Truncate(b.getCurrentHash(), maxLen).c_str());
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", b.getCurrentHash().c_str());
        ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", b.getSha3Hash().c_str());
        ImGui::EndTooltip();
    }
}

// Returns the left-group height so the map column can match it.
static float RenderDashboardStats(cw1::Blockchain& chain,
                                  float statsW, float cardH) {
    char buf1[32], buf2[32], buf4[32];
    snprintf(buf1, sizeof(buf1), "%zu", chain.totalBlocks());
    snprintf(buf2, sizeof(buf2), "%zu", chain.getAllVins().size());
    snprintf(buf4, sizeof(buf4), "%zu", chain.getAuditLog().size());

    float leftGroupTop = ImGui::GetCursorPosY();
    ImGui::BeginGroup();
    {
        float halfW = (statsW - 8.0f) / 2.0f;
        DrawStatCard(buf1, "Total Blocks",    COL_ACCENT,   ImVec2(halfW, cardH));
        ImGui::SameLine(0, 8);
        DrawStatCard(buf2, "Unique Vehicles", COL_GREEN_BR, ImVec2(halfW, cardH));

        ImGui::Spacing();

        DrawStatCard(g_verifyDone && g_lastVerify.ok ? "VERIFIED" : (g_verifyDone ? "TAMPERED" : "N/A"),
                     "Integrity",
                     g_verifyDone && g_lastVerify.ok ? COL_GREEN_BR : COL_RED,
                     ImVec2(halfW, cardH));
        ImGui::SameLine(0, 8);
        DrawStatCard(buf4, "Audit Events", COL_PURPLE, ImVec2(halfW, cardH));
    }
    ImGui::EndGroup();
    return ImGui::GetCursorPosY() - leftGroupTop;
}

static void RenderDashboardMap(float mapWidth, float mapHeight) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(8, 15, 30, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##mapChild", ImVec2(mapWidth, mapHeight), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    float mw = ImGui::GetContentRegionAvail().x;
    float mh = ImGui::GetContentRegionAvail().y;
    RenderBranchMap(mw, mh);
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

static void RenderFuelChart(const std::vector<cw1::FuelWeeklyRecord>& hist,
                            float chartW, float chartH) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##fuelChart", ImVec2(chartW, chartH), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float gw = ImGui::GetContentRegionAvail().x;
    float gh = ImGui::GetContentRegionAvail().y - 16.0f;

    float yMin = 1e9f, yMax = -1e9f;
    for (const auto& r : hist) {
        float vals[] = {(float)r.ron95, (float)r.ron97,
                        (float)r.dieselPeninsular, (float)r.dieselEast};
        for (float v : vals) { if (v < yMin) yMin = v; if (v > yMax) yMax = v; }
    }
    float yPad = (yMax - yMin) * 0.15f;
    if (yPad < 0.05f) yPad = 0.05f;
    yMin -= yPad; yMax += yPad;
    if (yMin < 0.0f) yMin = 0.0f;

    ImVec2 org = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    float yLabelW = 44.0f;

    dl->AddRectFilled(ImVec2(org.x + yLabelW, org.y),
                      ImVec2(org.x + gw, org.y + gh),
                      IM_COL32(13, 17, 23, 255), 4.0f);

    for (int g = 0; g <= 3; ++g) {
        float frac = (float)g / 3.0f;
        float y = org.y + gh - frac * gh;
        dl->AddLine(ImVec2(org.x + yLabelW, y), ImVec2(org.x + gw, y),
                    IM_COL32(48, 54, 61, 100), 1.0f);
        char lbl[16]; snprintf(lbl, sizeof(lbl), "%.2f", yMin + frac * (yMax - yMin));
        dl->AddText(ImVec2(org.x, y - 7), IM_COL32(139, 148, 158, 180), lbl);
    }

    size_t n = hist.size();
    if (n >= 2) {
        ImU32 cols[] = {IM_COL32(46,160,67,255), IM_COL32(31,111,235,255),
                        IM_COL32(210,153,34,255), IM_COL32(219,109,40,255)};
        auto mY = [&](double v) { return org.y + gh - ((float)v - yMin) / (yMax - yMin) * gh; };
        auto mX = [&](size_t i) { return org.x + yLabelW + 6.0f + ((float)i / (float)(n-1)) * (gw - yLabelW - 12.0f); };

        struct SI { ImU32 c; double cw1::FuelWeeklyRecord::*f; };
        SI ss[] = {{cols[0], &cw1::FuelWeeklyRecord::ron95},
                   {cols[1], &cw1::FuelWeeklyRecord::ron97},
                   {cols[2], &cw1::FuelWeeklyRecord::dieselPeninsular},
                   {cols[3], &cw1::FuelWeeklyRecord::dieselEast}};

        for (const auto& s : ss) {
            for (size_t i = 1; i < n; ++i)
                dl->AddLine(ImVec2(mX(i-1), mY(hist[i-1].*s.f)),
                            ImVec2(mX(i),   mY(hist[i].*s.f)), s.c, 2.0f);
            for (size_t i = 0; i < n; ++i)
                dl->AddCircleFilled(ImVec2(mX(i), mY(hist[i].*s.f)), 2.5f, s.c);
        }

        auto dateLbl = [&](size_t idx) {
            const char* dt = hist[idx].effectiveDate.c_str();
            const char* sd = (std::strlen(dt) >= 10) ? dt + 5 : dt;
            dl->AddText(ImVec2(mX(idx) - 12, org.y + gh + 2),
                        IM_COL32(139,148,158,180), sd);
        };
        dateLbl(0); if (n > 2) dateLbl(n/2); dateLbl(n-1);

        // Hover tooltip
        ImVec2 mpos = ImGui::GetMousePos();
        if (mpos.x >= mX(0) && mpos.x <= mX(n-1) &&
            mpos.y >= org.y && mpos.y <= org.y + gh) {
            size_t nearest = 0;
            float bestDist = 1e9f;
            for (size_t i = 0; i < n; ++i) {
                float d = std::fabs(mpos.x - mX(i));
                if (d < bestDist) { bestDist = d; nearest = i; }
            }

            float nx = mX(nearest);
            dl->AddLine(ImVec2(nx, org.y), ImVec2(nx, org.y + gh),
                        IM_COL32(200, 200, 200, 80), 1.0f);
            for (const auto& s : ss)
                dl->AddCircleFilled(ImVec2(nx, mY(hist[nearest].*s.f)), 5.0f, s.c);

            ImGui::BeginTooltip();
            ImGui::TextColored(COL_ACCENT, "%s", hist[nearest].effectiveDate.c_str());
            ImGui::Separator();
            const char* names[] = {"RON95","RON97","Diesel(P)","Diesel(E)"};
            for (int si = 0; si < 4; ++si) {
                double val = hist[nearest].*(ss[si].f);
                ImVec4 sc = ImGui::ColorConvertU32ToFloat4(ss[si].c);
                ImGui::TextColored(sc, "%-12s RM %.2f", names[si], val);
            }
            ImGui::EndTooltip();
        }
    }

    // Legend (top-right corner)
    {
        float lx = org.x + gw - 120.0f, ly = org.y + 4.0f;
        const char* labels[] = {"RON95","RON97","Diesel(P)","Diesel(E)"};
        ImU32 lc[] = {IM_COL32(46,160,67,255), IM_COL32(31,111,235,255),
                      IM_COL32(210,153,34,255), IM_COL32(219,109,40,255)};
        for (int i = 0; i < 4; ++i) {
            dl->AddRectFilled(ImVec2(lx, ly + i*14.0f),
                              ImVec2(lx+8, ly + i*14.0f+8), lc[i], 2.0f);
            dl->AddText(ImVec2(lx+12, ly + i*14.0f - 1),
                        IM_COL32(200,207,213,200), labels[i]);
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

static void RenderFuelPriceTable(float rightW, float chartH) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##fuelPrices", ImVec2(rightW, chartH), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    constexpr double kDist = 10.0, kEff = 3.0;

    struct FuelRow {
        const char* name; ImVec4 col; cw1::FuelSeries series;
    };
    FuelRow rows[] = {
        {"RON95",                COL_GREEN_BR, cw1::FuelSeries::RON95},
        {"RON97",                COL_ACCENT,   cw1::FuelSeries::RON97},
        {"Diesel (Peninsular)",  COL_YELLOW,   cw1::FuelSeries::DIESEL_PENINSULAR},
        {"Diesel (East Malaysia)", COL_ORANGE, cw1::FuelSeries::DIESEL_EAST},
    };

    ImGuiTableFlags tf = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##fuelTbl", 4, tf)) {
        ImGui::TableSetupColumn("Fuel",     ImGuiTableColumnFlags_WidthStretch, 3.0f);
        ImGui::TableSetupColumn("Price/L",  ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Trend",    ImGuiTableColumnFlags_WidthStretch, 1.8f);
        ImGui::TableSetupColumn("RM/10 miles",ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableHeadersRow();

        for (const auto& fr : rows) {
            cw1::FuelForecast fc = g_fuelMgr.forecast(fr.series);
            double cost = cw1::FuelPriceManager::EstimateDeliveryCost(fc.currentPrice, kDist, kEff);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(fr.col, "%s", fr.name);

            ImGui::TableNextColumn();
            char buf[32];
            snprintf(buf, sizeof(buf), "RM %.2f", fc.currentPrice);
            ImGui::Text("%s", buf);

            ImGui::TableNextColumn();
            if (fc.stable) {
                ImGui::TextColored(COL_MUTED, "--");
            } else {
                ImVec4 tc = fc.increasing ? COL_RED : COL_GREEN_BR;
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImDrawList* tdl = ImGui::GetWindowDrawList();
                float sz = 8.0f, cy = pos.y + ImGui::GetTextLineHeight() * 0.5f;
                ImU32 acol = ImGui::ColorConvertFloat4ToU32(tc);
                if (fc.increasing) {
                    tdl->AddTriangleFilled(
                        ImVec2(pos.x + sz * 0.5f, cy - sz * 0.5f),
                        ImVec2(pos.x,             cy + sz * 0.5f),
                        ImVec2(pos.x + sz,        cy + sz * 0.5f), acol);
                } else {
                    tdl->AddTriangleFilled(
                        ImVec2(pos.x,             cy - sz * 0.5f),
                        ImVec2(pos.x + sz,        cy - sz * 0.5f),
                        ImVec2(pos.x + sz * 0.5f, cy + sz * 0.5f), acol);
                }
                ImGui::Dummy(ImVec2(sz + 4.0f, 0));
                ImGui::SameLine();
                snprintf(buf, sizeof(buf), "%+.2f", fc.delta);
                ImGui::TextColored(tc, "%s", buf);
            }

            ImGui::TableNextColumn();
            snprintf(buf, sizeof(buf), "RM %.2f", cost);
            ImGui::TextColored(COL_TEXT, "%s", buf);
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextColored(COL_VERY_MUTED, "Truck: %.0f mi/L | Weekly update (Thu)", kEff);
    ImGui::TextColored(COL_VERY_MUTED, "%s", g_fuelMgr.statusText().c_str());
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 110.0f);
    if (ImGui::SmallButton("Refresh")) {
        g_fuelMgr.loadRecentHistory(16);
        PushToast("Fuel data refreshed", COL_GREEN_BR);
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

static void RenderRecentBlocksTable(const std::vector<cw1::Block>& blocks) {
    ImGui::TextColored(COL_MUTED, "RECENT BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    ImGuiTableFlags tflags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH |
                             ImGuiTableFlags_RowBg |
                             ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 5.0f));
    if (ImGui::BeginTable("##recent", 6, tflags)) {
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
            DrawHashWithTooltip(b);

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

// ---------------------------------------------------------------------------
// Dashboard (coordinator)
// ---------------------------------------------------------------------------

static void RenderDashboard(cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    DrawSectionHeading("Dashboard");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Layout dimensions
    float totalW = ImGui::GetContentRegionAvail().x;
    float mapWidth = totalW * 0.42f;
    if (mapWidth < 280) mapWidth = 280;
    float statsW = totalW - mapWidth - 12.0f;
    float mapH = 260.0f;
    float cardH = (mapH - 8.0f) / 2.0f;

    // Top row: stat cards (left) + map (right)
    float leftGroupH = RenderDashboardStats(chain, statsW, cardH);
    ImGui::SameLine(0, 12);
    ImGui::BeginGroup();
    RenderDashboardMap(mapWidth, leftGroupH);
    ImGui::EndGroup();

    // Fuel Intelligence Panel
    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "FUEL PRICE INTELLIGENCE");
    ImGui::SameLine(totalW - mapWidth);
    ImGui::TextColored(COL_MUTED, "FUEL PRICES & DELIVERY COST / PER 10 MILES");
    ImGui::Separator();
    ImGui::Spacing();

    if (g_fuelInitialized && !g_fuelMgr.history().empty()) {
        const auto& hist = g_fuelMgr.history();
        float fuelTotalW = ImGui::GetContentRegionAvail().x;
        float chartW = fuelTotalW - mapWidth - 12.0f;
        float chartH = 260.0f;

        ImGui::BeginGroup();
        RenderFuelChart(hist, chartW, chartH);
        ImGui::EndGroup();

        ImGui::SameLine(0, 12);
        ImGui::BeginGroup();
        RenderFuelPriceTable(mapWidth, chartH);
        ImGui::EndGroup();
    }

    ImGui::Spacing(); ImGui::Spacing();
    RenderRecentBlocksTable(blocks);
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

static void RenderCarDetail(cw1::Blockchain& chain) {
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





// Vehicle data (models, colors, transport modes, stage names) now in
// utils/VehicleData.hpp / VehicleData.cpp.
// Manufacturer & branch data in utils/BranchMap.hpp / BranchMap.cpp.

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
    if (ImGui::Combo("##add_Manufacturer", &g_formMfrIndex, k_manufacturers, k_manufacturerCount)) {
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
    ImGui::Combo("##add_Color", &g_formColorIndex, k_colors, k_colorCount);

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
        ImGui::Combo("##add_Transport", &g_formTransportIndex, k_transportModes, k_transportModeCount);
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





static void RenderGlobalChain(cw1::Blockchain& chain) {
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
            DrawHashWithTooltip(b);

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





static void RenderAuditLog(cw1::Blockchain& chain) {
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





// ---------------------------------------------------------------------------
// Integrity sub-components
// ---------------------------------------------------------------------------

static void RenderTempChainControls(cw1::Blockchain& chain) {
    // Start/Stop Toggle
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

    // Status display
    if (!g_tempChain.empty()) {
        ImGui::Spacing();
        std::size_t genCount = g_tempChain.size() - g_tempRealCount;
        ImGui::TextColored(COL_TEXT, "Temp chain: %zu blocks (%zu real + %zu generated)  |  Generating: %s",
                           g_tempChain.size(), g_tempRealCount, genCount,
                           g_tempGenerating ? "ON" : "OFF");
    }

    // Interval slider
    if (g_tempGenerating || !g_tempChain.empty()) {
        ImGui::Spacing();
        ImGui::SetNextItemWidth(240.0f);
        ImGui::SliderFloat("Generation interval (s)##tempinterval", &g_tempInterval, 1.0f, 10.0f, "%.1f s");
    }

    // Tamper controls
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

        // Delete temp block
        ImGui::Spacing();
        ImGui::SetNextItemWidth(140.0f);
        ImGui::InputInt("Delete temp block index##tempdelidx", &g_tempDeleteIndex);
        ImGui::SameLine();

        // Only allow deleting generated temp blocks, not original chain blocks.
        bool delValid = (g_tempDeleteIndex >= 0 &&
                         static_cast<std::size_t>(g_tempDeleteIndex) >= g_tempRealCount &&
                         static_cast<std::size_t>(g_tempDeleteIndex) < g_tempChain.size() &&
                         g_tempChain.size() > 1);

        ImGui::BeginDisabled(!delValid);
        if (DrawDangerButton("Delete Temp Block##tempdel", ImVec2(180.0f, 34.0f))) {
            std::size_t di = static_cast<std::size_t>(g_tempDeleteIndex);
            g_tempChain.erase(g_tempChain.begin() + static_cast<std::ptrdiff_t>(di));
            for (std::size_t ri = di; ri < g_tempChain.size(); ++ri) {
                g_tempChain[ri].setIndex(ri);
            }
            PushToast("Temp block #" + std::to_string(g_tempDeleteIndex) +
                      " deleted (chain links NOT repaired).", COL_RED);
        }
        ImGui::EndDisabled();
        if (g_tempDeleteIndex >= 0 &&
            static_cast<std::size_t>(g_tempDeleteIndex) < g_tempRealCount) {
            ImGui::SameLine();
            ImGui::TextColored(COL_YELLOW, "Cannot delete original chain blocks (0-%zu)",
                               g_tempRealCount > 0 ? g_tempRealCount - 1 : 0u);
        }
    }
}

static void RenderTempChainVerification() {
    if (g_tempChain.empty()) return;

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

    if (!g_tempVerifyDone) return;

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

static void RenderTempChainVisual() {
    if (g_tempChain.empty()) return;

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "TEMP CHAIN BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    // Detect new blocks for fade-in animation
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

        bool isNewest = (ti == g_tempChain.size() - 1);
        float alpha = (isNewest && g_tempNewBlockAnim < 1.0f) ? g_tempNewBlockAnim : 1.0f;

        ImVec2 cardMin = ImGui::GetCursorScreenPos();
        cardMin.y = startY;
        ImVec2 cardMax(cardMin.x + cardW, cardMin.y + cardH);

        ImVec4 bgCol = isFailed ? HexColor(0x5a2429, 0.6f * alpha) : HexColor(0x21262d, alpha);
        ImVec4 borderCol = isFailed ? HexColor(0xda3633, 0.8f * alpha) : HexColor(0x30363d, 0.8f * alpha);
        dl->AddRectFilled(cardMin, cardMax,
            ImGui::ColorConvertFloat4ToU32(bgCol), 6.0f);
        dl->AddRect(cardMin, cardMax,
            ImGui::ColorConvertFloat4ToU32(borderCol), 6.0f);

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

        if (g_tempVerifyDone) {
            if (isFailed)
                ImGui::TextColored(ImVec4(COL_RED.x, COL_RED.y, COL_RED.z, alpha), "FAILED");
            else
                ImGui::TextColored(ImVec4(COL_GREEN_BR.x, COL_GREEN_BR.y, COL_GREEN_BR.z, alpha), "OK");
        }

        ImGui::PopStyleVar();
        ImGui::EndGroup();

        if (ImGui::IsMouseHoveringRect(cardMin, cardMax)) {
            ImGui::BeginTooltip();
            ImGui::TextColored(COL_ACCENT, "Block #%zu", tb.getIndex());
            ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", tb.getCurrentHash().c_str());
            ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", tb.getSha3Hash().c_str());
            ImGui::TextColored(COL_PURPLE, "Prev:     %s", tb.getPreviousHash().c_str());
            ImGui::EndTooltip();
        }

        ImGui::SetCursorScreenPos(ImVec2(cardMax.x, startY));

        if (ti + 1 < g_tempChain.size()) {
            float arrowCenterY = startY + cardH * 0.5f;
            dl->AddLine(ImVec2(cardMax.x + 4.0f, arrowCenterY),
                        ImVec2(cardMax.x + arrowW - 6.0f, arrowCenterY),
                        ImGui::ColorConvertFloat4ToU32(COL_MUTED), 2.0f);
            dl->AddTriangleFilled(
                ImVec2(cardMax.x + arrowW - 6.0f, arrowCenterY - 5.0f),
                ImVec2(cardMax.x + arrowW - 6.0f, arrowCenterY + 5.0f),
                ImVec2(cardMax.x + arrowW, arrowCenterY),
                ImGui::ColorConvertFloat4ToU32(COL_MUTED));
            ImGui::SetCursorScreenPos(ImVec2(cardMax.x + arrowW, startY));
        }

        if (isNewest && scrollToEnd) {
            ImGui::SetScrollHereX(1.0f);
        }
    }

    float totalW = g_tempChain.size() * cardW + (g_tempChain.size() - 1) * arrowW;
    ImGui::SetCursorPos(ImVec2(totalW, 0));
    ImGui::Dummy(ImVec2(1, 1));

    ImGui::EndChild();
}

// ---------------------------------------------------------------------------
// Integrity (coordinator)
// ---------------------------------------------------------------------------

static void RenderIntegrity(cw1::Blockchain& chain) {
    DrawSectionHeading("Blockchain Characteristics: Immutability");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // Initialize temp chain from real chain on first visit
    if (g_tempChain.empty() && !g_tempGenerating) {
        g_tempChain = chain.getChain();
        g_tempRealCount = g_tempChain.size();
        g_tempPrevCount = g_tempChain.size();
    }

    RenderTempChainControls(chain);
    RenderTempChainVerification();
    RenderTempChainVisual();

    // Reset button
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

    // --- Soft Delete ---
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

    // --- Deleted Blocks & Restore ---
    auto deletedIndices = chain.getDeletedIndices();
    if (!deletedIndices.empty()) {
        ImGui::Spacing(); ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextColored(COL_YELLOW, "Soft-Deleted Blocks (original data preserved)");
        ImGui::Spacing();

        ImGuiTableFlags tflags = ImGuiTableFlags_BordersInnerV |
                                 ImGuiTableFlags_BordersInnerH |
                                 ImGuiTableFlags_RowBg |
                                 ImGuiTableFlags_SizingStretchProp;
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
        if (ImGui::BeginTable("##deleted_blocks_table", 5, tflags, ImVec2(-1, 0))) {
            ImGui::TableSetupColumn("Block #",      ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("VIN",           ImGuiTableColumnFlags_WidthStretch, 1.5f);
            ImGui::TableSetupColumn("Manufacturer",  ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Model",         ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Action",        ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableHeadersRow();

            for (std::size_t idx : deletedIndices) {
                const cw1::CarRecord* orig = chain.getDeletedOriginal(idx);
                if (!orig) continue;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(COL_RED, "#%zu", idx);
                ImGui::TableNextColumn();
                ImGui::TextColored(COL_MUTED, "%s", orig->vin.c_str());
                ImGui::TableNextColumn();
                ImGui::TextColored(COL_MUTED, "%s", orig->manufacturer.c_str());
                ImGui::TableNextColumn();
                ImGui::TextColored(COL_MUTED, "%s", orig->model.c_str());
                ImGui::TableNextColumn();

                std::string btnLabel = "Restore##restore_" + std::to_string(idx);
                if (DrawPrimaryButton(btnLabel.c_str(), ImVec2(90.0f, 28.0f))) {
                    std::string msg;
                    const bool ok = chain.restoreBlock(idx, msg);
                    PushToast(ok ? msg : ("Restore failed: " + msg),
                              ok ? COL_GREEN_BR : COL_RED);

                    cw1::OperationTimer vt;
                    g_lastVerify = chain.verifyIntegrity();
                    g_lastVerifySeconds = vt.elapsedSeconds();
                    g_verifyDone = true;
                }
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
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

    // -- Initialize Fuel Intelligence --
    if (chain.isDatabaseOpen()) {
        cw1::DatabaseManager* dbm = chain.getDatabase();
        if (dbm != nullptr) {
            g_fuelMgr.attach(dbm->rawHandle());
            g_fuelMgr.seedFallbackDataIfEmpty();
            g_fuelMgr.loadRecentHistory(16);
            g_fuelInitialized = true;
        }
    }

    
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
