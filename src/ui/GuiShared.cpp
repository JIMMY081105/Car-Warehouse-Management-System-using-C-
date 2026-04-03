// Global state definitions and shared helper functions used across the UI.

#include "ui/GuiApp.hpp"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <utility>

#include "security/SecurityUtil.hpp"
#include "utils/OperationTimer.hpp"
#include "utils/TimeUtil.hpp"
#include "utils/VehicleData.hpp"

const ImVec4 COL_BG_MAIN     = HexColor(0x0d1117);
const ImVec4 COL_BG_PANEL    = HexColor(0x161b22);
const ImVec4 COL_BG_CARD     = HexColor(0x21262d);
const ImVec4 COL_BG_ELEV     = HexColor(0x1b222c);
const ImVec4 COL_BG_HOVER    = HexColor(0x30363d);
const ImVec4 COL_TEXT        = HexColor(0xe6edf3);
const ImVec4 COL_MUTED       = HexColor(0x8b949e);
const ImVec4 COL_VERY_MUTED  = HexColor(0x484f58);
const ImVec4 COL_ACCENT      = HexColor(0x1f6feb);
const ImVec4 COL_ACCENT_HO   = HexColor(0x388bfd);
const ImVec4 COL_ACCENT_SOFT = HexColor(0x1f6feb, 0.20f);
const ImVec4 COL_GREEN       = HexColor(0x238636);
const ImVec4 COL_GREEN_BR    = HexColor(0x2ea043);
const ImVec4 COL_RED         = HexColor(0xda3633);
const ImVec4 COL_YELLOW      = HexColor(0xd29922);
const ImVec4 COL_PURPLE      = HexColor(0x8957e5);
const ImVec4 COL_ORANGE      = HexColor(0xdb6d28);
const ImVec4 COL_BORDER_SOFT = HexColor(0x30363d, 0.80f);

ImFont* g_fontBody = nullptr;
ImFont* g_fontSection = nullptr;
ImFont* g_fontTitle = nullptr;
ImFont* g_fontHero = nullptr;

std::string g_projectRoot = ".";

View g_view = View::DASHBOARD;
std::string g_selectedVin;
char g_searchBuf[256] = {};
cw1::ValidationResult g_lastVerify = {false, "Not yet verified", {}};
bool g_verifyDone = false;
double g_lastVerifySeconds = 0.0;
double g_lastAddBlockSeconds = 0.0;
double g_lastSearchSeconds = 0.0;
int g_deleteBlockIndex = -1;
double g_lastDeleteSeconds = 0.0;
std::string g_cachedSearchQuery;
std::vector<std::string> g_cachedSearchVins;
View g_lastLoggedView = View::DASHBOARD;
std::string g_lastLoggedVin;
std::vector<Toast> g_toasts;

cw1::FuelPriceManager g_fuelMgr;
bool g_fuelInitialized = false;

cw1::AuthManager g_authMgr;
cw1::PendingApprovalManager g_pendingMgr;
bool g_showLoginScreen = true;
char g_loginUser[64] = {};
char g_loginPass[64] = {};
std::string g_loginError;
char g_rejectReasonBuf[256] = {};

int g_formStage = 0;
char g_formVin[64] = {};
int g_formMfrIndex = 0;
int g_formModelIndex = 0;
int g_formBranchIndex = 0;
int g_formColorIndex = 0;
int g_formYear = 2025;
char g_formFactory[128] = {};
char g_formWarehouse[64] = {};
char g_formReceivedBy[64] = {};
char g_formInspector[64] = {};
bool g_formPassed = true;
char g_formQcNotes[256] = {};
char g_formDealerId[64] = {};
char g_formDestination[64] = {};
int g_formTransportIndex = 0;
char g_formBuyerId[64] = {};
double g_formSalePrice = 0.0;
char g_formWarranty[32] = {};
char g_formMfrId[64] = {};
char g_formSupplierId[64] = {};
char g_formRetailerId[64] = {};
int g_auditN = 20;

bool g_tempGenerating = false;
float g_tempAccumulator = 0.0f;
float g_tempInterval = 5.0f;
std::vector<cw1::Block> g_tempChain;
cw1::ValidationResult g_tempVerify = {false, "Not verified", {}};
bool g_tempVerifyDone = false;
double g_tempVerifySeconds = 0.0;
int g_tempTamperIndex = -1;
int g_tempDeleteIndex = -1;
std::size_t g_tempRealCount = 0;
std::size_t g_tempPrevCount = 0;
float g_tempNewBlockAnim = 1.0f;

ai::ChatbotService g_chatbot;
char g_chatInput[2048] = {};
bool g_chatProjectMode = false;

ImVec4 HexColor(std::uint32_t hex, float a) {
    return ImVec4(
        static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
        static_cast<float>((hex >> 8) & 0xFF) / 255.0f,
        static_cast<float>(hex & 0xFF) / 255.0f,
        a);
}

void ApplyGitHubDarkTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 10.0f;
    s.FrameRounding = 7.0f;
    s.PopupRounding = 8.0f;
    s.TabRounding = 7.0f;
    s.ScrollbarRounding = 8.0f;
    s.GrabRounding = 6.0f;
    s.ChildRounding = 8.0f;
    s.FramePadding = ImVec2(10.0f, 7.0f);
    s.ItemSpacing = ImVec2(10.0f, 10.0f);
    s.WindowPadding = ImVec2(16.0f, 16.0f);
    s.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    s.CellPadding = ImVec2(10.0f, 8.0f);
    s.ScrollbarSize = 12.0f;
    s.GrabMinSize = 8.0f;
    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 1.0f;
    s.ChildBorderSize = 1.0f;
    s.PopupBorderSize = 1.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text] = COL_TEXT;
    c[ImGuiCol_TextDisabled] = COL_MUTED;
    c[ImGuiCol_WindowBg] = COL_BG_MAIN;
    c[ImGuiCol_ChildBg] = COL_BG_PANEL;
    c[ImGuiCol_PopupBg] = COL_BG_PANEL;
    c[ImGuiCol_Border] = COL_BORDER_SOFT;
    c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_FrameBg] = COL_BG_ELEV;
    c[ImGuiCol_FrameBgHovered] = COL_BG_HOVER;
    c[ImGuiCol_FrameBgActive] = HexColor(0x1f6feb, 0.35f);
    c[ImGuiCol_TitleBg] = COL_BG_PANEL;
    c[ImGuiCol_TitleBgActive] = COL_BG_PANEL;
    c[ImGuiCol_TitleBgCollapsed] = COL_BG_MAIN;
    c[ImGuiCol_MenuBarBg] = COL_BG_PANEL;
    c[ImGuiCol_ScrollbarBg] = COL_BG_MAIN;
    c[ImGuiCol_ScrollbarGrab] = COL_VERY_MUTED;
    c[ImGuiCol_ScrollbarGrabHovered] = COL_MUTED;
    c[ImGuiCol_ScrollbarGrabActive] = COL_ACCENT;
    c[ImGuiCol_CheckMark] = COL_ACCENT;
    c[ImGuiCol_SliderGrab] = COL_ACCENT;
    c[ImGuiCol_SliderGrabActive] = COL_ACCENT_HO;
    c[ImGuiCol_Button] = COL_BG_ELEV;
    c[ImGuiCol_ButtonHovered] = COL_BG_HOVER;
    c[ImGuiCol_ButtonActive] = COL_ACCENT_SOFT;
    c[ImGuiCol_Header] = COL_BG_CARD;
    c[ImGuiCol_HeaderHovered] = COL_BG_HOVER;
    c[ImGuiCol_HeaderActive] = COL_ACCENT_SOFT;
    c[ImGuiCol_Separator] = COL_BORDER_SOFT;
    c[ImGuiCol_SeparatorHovered] = COL_MUTED;
    c[ImGuiCol_SeparatorActive] = COL_ACCENT;
    c[ImGuiCol_ResizeGrip] = COL_BG_HOVER;
    c[ImGuiCol_ResizeGripHovered] = COL_MUTED;
    c[ImGuiCol_ResizeGripActive] = COL_ACCENT;
    c[ImGuiCol_Tab] = COL_BG_PANEL;
    c[ImGuiCol_TabHovered] = COL_BG_HOVER;
    c[ImGuiCol_TabSelected] = COL_BG_CARD;
    c[ImGuiCol_TabSelectedOverline] = COL_ACCENT;
    c[ImGuiCol_TabDimmed] = COL_BG_PANEL;
    c[ImGuiCol_TabDimmedSelected] = COL_BG_CARD;
    c[ImGuiCol_PlotLines] = COL_ACCENT;
    c[ImGuiCol_PlotLinesHovered] = COL_ACCENT_HO;
    c[ImGuiCol_PlotHistogram] = COL_ACCENT;
    c[ImGuiCol_PlotHistogramHovered] = COL_ACCENT_HO;
    c[ImGuiCol_TableHeaderBg] = COL_BG_ELEV;
    c[ImGuiCol_TableBorderStrong] = COL_BORDER_SOFT;
    c[ImGuiCol_TableBorderLight] = HexColor(0x30363d, 0.55f);
    c[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_TableRowBgAlt] = HexColor(0x161b22, 0.78f);
    c[ImGuiCol_TextSelectedBg] = HexColor(0x1f6feb, 0.3f);
    c[ImGuiCol_DragDropTarget] = COL_ACCENT;
    c[ImGuiCol_NavHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = COL_ACCENT;
    c[ImGuiCol_NavWindowingDimBg] = HexColor(0x0d1117, 0.7f);
    c[ImGuiCol_ModalWindowDimBg] = HexColor(0x0d1117, 0.6f);
}

void LoadDemoData(cw1::Blockchain& chain) {
    {
        cw1::CarRecord r;
        r.vin = "VIN1001";
        r.manufacturer = "Perodua";
        r.model = "Axia";
        r.color = "Silver";
        r.productionYear = 2024;
        r.manufacturerId = "MFR-2522";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Shah Alam Plant";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-A1";
        r.receivedBy = "Ahmad bin Ismail";
        r.supplierId = "SUP-197588";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-001";
        r.passed = true;
        r.qcNotes = "All systems nominal";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KB-01";
        r.destination = "Kota Bharu";
        r.transportMode = "Truck";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::CUSTOMER_SALE;
        r.buyerId = "CUST-10201";
        r.salePrice = 38000.00;
        r.warrantyExpiry = "2029-03-01";
        r.retailerId = "RTL-91428";
        chain.addBlock(r);
    }
    {
        cw1::CarRecord r;
        r.vin = "VIN1002";
        r.manufacturer = "Toyota";
        r.model = "Vios";
        r.color = "White";
        r.productionYear = 2023;
        r.manufacturerId = "MFR-3011";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Toyota Bukit Raja";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-B2";
        r.receivedBy = "Lim Wei Jie";
        r.supplierId = "SUP-203344";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-003";
        r.passed = true;
        r.qcNotes = "Minor paint scratch - touched up";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::DEALER_DISPATCH;
        r.dealerId = "DLR-KL-05";
        r.destination = "Kuala Lumpur";
        r.transportMode = "Truck";
        chain.addBlock(r);
    }
    {
        cw1::CarRecord r;
        r.vin = "VIN1003";
        r.manufacturer = "Honda";
        r.model = "City";
        r.color = "Blue";
        r.productionYear = 2025;
        r.manufacturerId = "MFR-4500";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Honda Pegoh, Melaka";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-C3";
        r.receivedBy = "Raj Kumar";
        r.supplierId = "SUP-310021";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::QUALITY_CHECK;
        r.inspectorId = "QC-007";
        r.passed = false;
        r.qcNotes = "Brake pad below spec - pending replacement";
        chain.addBlock(r);
    }
    {
        cw1::CarRecord r;
        r.vin = "VIN1004";
        r.manufacturer = "Proton";
        r.model = "X50";
        r.color = "Red";
        r.productionYear = 2025;
        r.manufacturerId = "MFR-1088";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Proton Tanjung Malim";
        chain.addBlock(r);
        r.stage = cw1::BlockStage::WAREHOUSE_INTAKE;
        r.warehouseLocation = "WH-D4";
        r.receivedBy = "Nurul Aina";
        r.supplierId = "SUP-405512";
        chain.addBlock(r);
    }
    {
        cw1::CarRecord r;
        r.vin = "VIN1005";
        r.manufacturer = "Perodua";
        r.model = "Myvi";
        r.color = "Black";
        r.productionYear = 2025;
        r.manufacturerId = "MFR-2523";
        r.stage = cw1::BlockStage::PRODUCTION;
        r.factoryLocation = "Perodua Rawang";
        chain.addBlock(r);
    }
}

ImVec4 StageColor(cw1::BlockStage stage) {
    switch (stage) {
        case cw1::BlockStage::PRODUCTION: return COL_MUTED;
        case cw1::BlockStage::WAREHOUSE_INTAKE: return COL_ACCENT;
        case cw1::BlockStage::QUALITY_CHECK: return COL_YELLOW;
        case cw1::BlockStage::DEALER_DISPATCH: return COL_ORANGE;
        case cw1::BlockStage::CUSTOMER_SALE: return COL_GREEN;
        default: return COL_TEXT;
    }
}

float StageProgress(cw1::BlockStage stage) {
    return (static_cast<float>(stage) + 1.0f) / 5.0f;
}

std::string Truncate(const std::string& text, std::size_t maxLen) {
    return (text.size() <= maxLen) ? text : text.substr(0, maxLen) + "...";
}

std::string PointerToString(const void* ptr) {
    std::ostringstream out;
    out << ptr;
    return out.str();
}

void DrawSectionHeading(const char* text) {
    if (g_fontSection != nullptr) {
        ImGui::PushFont(g_fontSection);
    }
    ImGui::TextColored(COL_ACCENT, "%s", text);
    if (g_fontSection != nullptr) {
        ImGui::PopFont();
    }
}

bool DrawPrimaryButton(const char* label, const ImVec2& size) {
    ImGui::PushStyleColor(ImGuiCol_Button, COL_ACCENT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x1158c7));
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

bool DrawDangerButton(const char* label, const ImVec2& size) {
    ImGui::PushStyleColor(ImGuiCol_Button, HexColor(0x5d2227));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, HexColor(0x7c2f35));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x94353d));
    const bool clicked = ImGui::Button(label, size);
    ImGui::PopStyleColor(3);
    return clicked;
}

void DrawMetricBadge(const char* text, ImVec4 textColor, ImVec4 bgColor) {
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

void DrawStatCard(const char* value, const char* label, ImVec4 color, ImVec2 size) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    const bool autoHeight = (size.y <= 0.0f);
    const ImVec2 childSize(size.x, autoHeight ? 0.0f : size.y);
    const ImGuiChildFlags childFlags = ImGuiChildFlags_Borders
                                     | (autoHeight ? ImGuiChildFlags_AutoResizeY : 0);
    ImGui::BeginChild(label, childSize, childFlags,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    if (g_fontSection != nullptr) {
        ImGui::PushFont(g_fontSection);
    }
    ImGui::TextColored(color, "%s", value);
    if (g_fontSection != nullptr) {
        ImGui::PopFont();
    }
    ImGui::TextColored(COL_MUTED, "%s", label);
    ImGui::Spacing();
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void DrawHashWithTooltip(const cw1::Block& block, std::size_t maxLen) {
    ImGui::TextColored(COL_PURPLE, "%s", Truncate(block.getCurrentHash(), maxLen).c_str());
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextColored(COL_PURPLE, "SHA-256:  %s", block.getCurrentHash().c_str());
        ImGui::TextColored(COL_ORANGE, "SHA3-128: %s", block.getSha3Hash().c_str());
        ImGui::EndTooltip();
    }
}

void LogViewOnce(cw1::Blockchain& chain, View currentView,
                 const std::string& vin, const std::string& details) {
    if (currentView != g_lastLoggedView || vin != g_lastLoggedVin) {
        chain.getAuditLog().log(cw1::AuditAction::CHAIN_VIEWED, details);
        g_lastLoggedView = currentView;
        g_lastLoggedVin = vin;
    }
}

void PushToast(const std::string& msg, ImVec4 color, float dur) {
    g_toasts.push_back({msg, dur, color});
}

void ReportGuiFailure(cw1::Blockchain& chain,
                      cw1::AuditAction action,
                      const std::string& message) {
    chain.getAuditLog().log(action, message);
    PushToast(message, COL_RED, 4.0f);
}

void ReportGuiException(cw1::Blockchain& chain,
                        cw1::AuditAction action,
                        const char* context,
                        const std::exception& ex) {
    ReportGuiFailure(chain, action, std::string(context) + ": " + ex.what());
}

cw1::Role CurrentUserRole() {
    return g_authMgr.isLoggedIn() ? g_authMgr.currentUser().role
                                  : cw1::Role::AUDITOR;
}

bool VerifyRequestSignature(const cw1::PendingBlockRequest& request) {
    if (request.creatorSignature.empty()) {
        return false;
    }

    const cw1::User* creator = g_authMgr.findUser(request.requestedBy);
    if (creator == nullptr) {
        return false;
    }

    return cw1::SecurityUtil::verifySignature(
        request.creatorSignature,
        request.requestedBy,
        request.requestTimestamp,
        cw1::stageToString(request.requestedStage),
        request.payload.serialize(),
        creator->secretKey);
}

void WriteAuditEvent(cw1::Blockchain& chain,
                     cw1::AuditAction action,
                     const std::string& message) {
    chain.getAuditLog().log(action, message);
    if (chain.isDatabaseOpen()) {
        cw1::DatabaseManager* db = chain.getDatabase();
        if (db != nullptr) {
            db->insertAuditEntry(action, message, cw1::TimeUtil::nowIso8601());
        }
    }
}

void ResetAddBlockForm() {
    g_formVin[0] = '\0';
    g_formMfrIndex = 0;
    g_formModelIndex = 0;
    g_formBranchIndex = 0;
    g_formColorIndex = 0;
    g_formYear = 2025;
    g_formFactory[0] = '\0';
    g_formWarehouse[0] = '\0';
    g_formReceivedBy[0] = '\0';
    g_formInspector[0] = '\0';
    g_formPassed = true;
    g_formQcNotes[0] = '\0';
    g_formDealerId[0] = '\0';
    g_formDestination[0] = '\0';
    g_formTransportIndex = 0;
    g_formBuyerId[0] = '\0';
    g_formSalePrice = 0.0;
    g_formWarranty[0] = '\0';
    g_formMfrId[0] = '\0';
    g_formSupplierId[0] = '\0';
    g_formRetailerId[0] = '\0';
}

void RefreshChainVerification(cw1::Blockchain& chain) {
    g_lastVerifySeconds = cw1::measureSeconds([&]() {
        g_lastVerify = chain.verifyIntegrity();
    });
    g_verifyDone = true;
}
