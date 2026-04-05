// Shared GUI globals, helpers, and panel declarations.

#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

#include "imgui.h"

#include "ai/ChatbotService.hpp"
#include "blockchain/Block.hpp"
#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "blockchain/Validation.hpp"
#include "security/Auth.hpp"
#include "security/PendingApprovalManager.hpp"
#include "utils/FuelPriceManager.hpp"

enum class View {
    DASHBOARD,
    CAR_DETAIL,
    ADD_BLOCK,
    GLOBAL_CHAIN,
    AUDIT_LOG,
    INTEGRITY,
    DELETE_BLOCK,
    PENDING_APPROVALS,
    AI_ASSISTANT
};

struct Toast {
    std::string msg;
    float remainingSecs;
    ImVec4 color;
};

extern const ImVec4 COL_BG_MAIN;
extern const ImVec4 COL_BG_PANEL;
extern const ImVec4 COL_BG_CARD;
extern const ImVec4 COL_BG_ELEV;
extern const ImVec4 COL_BG_HOVER;
extern const ImVec4 COL_TEXT;
extern const ImVec4 COL_MUTED;
extern const ImVec4 COL_VERY_MUTED;
extern const ImVec4 COL_ACCENT;
extern const ImVec4 COL_ACCENT_HO;
extern const ImVec4 COL_ACCENT_SOFT;
extern const ImVec4 COL_GREEN;
extern const ImVec4 COL_GREEN_BR;
extern const ImVec4 COL_RED;
extern const ImVec4 COL_YELLOW;
extern const ImVec4 COL_PURPLE;
extern const ImVec4 COL_ORANGE;
extern const ImVec4 COL_BORDER_SOFT;

extern ImFont* g_fontBody;
extern ImFont* g_fontSection;
extern ImFont* g_fontTitle;
extern ImFont* g_fontHero;

extern std::string g_projectRoot;

extern View g_view;
extern std::string g_selectedVin;
extern char g_searchBuf[256];
extern cw1::ValidationResult g_lastVerify;
extern bool g_verifyDone;
extern double g_lastVerifySeconds;
extern double g_lastAddBlockSeconds;
extern double g_lastSearchSeconds;
extern int g_deleteBlockIndex;
extern double g_lastDeleteSeconds;
extern std::string g_cachedSearchQuery;
extern std::vector<std::string> g_cachedSearchVins;
extern View g_lastLoggedView;
extern std::string g_lastLoggedVin;
extern std::vector<Toast> g_toasts;

extern cw1::FuelPriceManager g_fuelMgr;
extern bool g_fuelInitialized;

extern cw1::AuthManager g_authMgr;
extern cw1::PendingApprovalManager g_pendingMgr;
extern bool g_showLoginScreen;
extern char g_loginUser[64];
extern char g_loginPass[64];
extern std::string g_loginError;
extern char g_rejectReasonBuf[256];

extern int g_formStage;
extern char g_formVin[64];
extern int g_formMfrIndex;
extern int g_formModelIndex;
extern int g_formBranchIndex;
extern int g_formColorIndex;
extern int g_formYear;
extern char g_formFactory[128];
extern char g_formWarehouse[64];
extern char g_formReceivedBy[64];
extern char g_formInspector[64];
extern bool g_formPassed;
extern char g_formQcNotes[256];
extern char g_formDealerId[64];
extern char g_formDestination[64];
extern int g_formTransportIndex;
extern char g_formBuyerId[64];
extern double g_formSalePrice;
extern char g_formWarranty[32];
extern char g_formMfrId[64];
extern char g_formSupplierId[64];
extern char g_formRetailerId[64];
extern int g_auditN;

extern bool g_tempGenerating;
extern float g_tempAccumulator;
extern float g_tempInterval;
extern std::vector<cw1::Block> g_tempChain;
extern cw1::ValidationResult g_tempVerify;
extern bool g_tempVerifyDone;
extern double g_tempVerifySeconds;
extern int g_tempTamperIndex;
extern int g_tempDeleteIndex;
extern std::size_t g_tempRealCount;
extern std::size_t g_tempPrevCount;
extern float g_tempNewBlockAnim;

extern ai::ChatbotService g_chatbot;
extern char g_chatInput[2048];
extern bool g_chatProjectMode;

ImVec4 HexColor(std::uint32_t hex, float a = 1.0f);
void ApplyGitHubDarkTheme();

bool ExportBlockchainPDF(cw1::Blockchain& chain,
                         const cw1::FuelPriceManager& fuelMgr,
                         const std::string& outputPath);

void LoadDemoData(cw1::Blockchain& chain);

ImVec4 StageColor(cw1::BlockStage stage);
float StageProgress(cw1::BlockStage stage);
std::string Truncate(const std::string& text, std::size_t maxLen);
std::string PointerToString(const void* ptr);

void DrawSectionHeading(const char* text);
bool DrawPrimaryButton(const char* label,
                       const ImVec2& size = ImVec2(0.0f, 0.0f));
bool DrawDangerButton(const char* label,
                      const ImVec2& size = ImVec2(0.0f, 0.0f));
void DrawMetricBadge(const char* text, ImVec4 textColor, ImVec4 bgColor);
void DrawStatCard(const char* value, const char* label, ImVec4 color, ImVec2 size);
void DrawHashWithTooltip(const cw1::Block& block, std::size_t maxLen = 12);

void LogViewOnce(cw1::Blockchain& chain, View currentView,
                 const std::string& vin, const std::string& details);
void PushToast(const std::string& msg, ImVec4 color, float dur = 3.0f);
void ReportGuiFailure(cw1::Blockchain& chain,
                      cw1::AuditAction action,
                      const std::string& message);
void ReportGuiException(cw1::Blockchain& chain,
                        cw1::AuditAction action,
                        const char* context,
                        const std::exception& ex);
cw1::Role CurrentUserRole();
bool VerifyRequestSignature(const cw1::PendingBlockRequest& request);
void WriteAuditEvent(cw1::Blockchain& chain,
                     cw1::AuditAction action,
                     const std::string& message);
void ResetAddBlockForm();
void RefreshChainVerification(cw1::Blockchain& chain);

void RenderHeader(const cw1::Blockchain& chain);
void RenderSidebar(cw1::Blockchain& chain);
void RenderDashboard(cw1::Blockchain& chain);
void RenderCarDetail(cw1::Blockchain& chain);
void RenderAddBlock(cw1::Blockchain& chain);
void RenderGlobalChain(cw1::Blockchain& chain);
void RenderAuditLog(cw1::Blockchain& chain);
void RenderIntegrity(cw1::Blockchain& chain);
void RenderDeleteBlock(cw1::Blockchain& chain);
void RenderLoginScreen(cw1::Blockchain& chain);
void RenderPendingApprovals(cw1::Blockchain& chain);
void RenderAIAssistant(cw1::Blockchain& chain);
void RenderToasts(float deltaTime);
