// Main entry point - sets up the window, loads fonts, and runs the render loop.

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

#include "ui/GuiApp.hpp"
#include "utils/BranchMap.hpp"
#include "utils/VehicleData.hpp"

namespace {

ImFont* TryLoadFont(ImGuiIO& io, const char* path, float sizePx) {
    FILE* file = std::fopen(path, "rb");
    if (file == nullptr) {
        return nullptr;
    }
    std::fclose(file);
    return io.Fonts->AddFontFromFileTTF(path, sizePx);
}

void LoadFonts(ImGuiIO& io, float dpiScale) {
#ifdef _WIN32
    g_fontBody = TryLoadFont(io, "C:/Windows/Fonts/segoeui.ttf", 16.0f * dpiScale);
    g_fontSection = TryLoadFont(io, "C:/Windows/Fonts/segoeuib.ttf", 22.0f * dpiScale);
    g_fontTitle = TryLoadFont(io, "C:/Windows/Fonts/segoeuib.ttf", 26.0f * dpiScale);
#endif

    if (g_fontBody == nullptr) {
        g_fontBody = TryLoadFont(io, "fonts/SegoeUI.ttf", 16.0f * dpiScale);
    }
    if (g_fontSection == nullptr) {
        g_fontSection = TryLoadFont(io, "fonts/SegoeUI-Bold.ttf", 22.0f * dpiScale);
    }
    if (g_fontTitle == nullptr) {
        g_fontTitle = TryLoadFont(io, "fonts/SegoeUI-Bold.ttf", 26.0f * dpiScale);
    }

    if (g_fontBody == nullptr) {
        g_fontBody = TryLoadFont(io, "fonts/JetBrainsMono-Regular.ttf", 16.0f * dpiScale);
    }
    if (g_fontSection == nullptr) {
        g_fontSection = TryLoadFont(io, "fonts/JetBrainsMono-Regular.ttf", 22.0f * dpiScale);
    }
    if (g_fontTitle == nullptr) {
        g_fontTitle = TryLoadFont(io, "fonts/JetBrainsMono-Regular.ttf", 26.0f * dpiScale);
    }

    g_fontHero = TryLoadFont(io, "C:/Windows/Fonts/segoeuib.ttf", 42.0f * dpiScale);
    if (g_fontHero == nullptr) {
        g_fontHero = TryLoadFont(io, "fonts/JetBrainsMono-Regular.ttf", 42.0f * dpiScale);
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
    if (g_fontHero == nullptr) {
        g_fontHero = g_fontTitle;
    }

    io.FontDefault = g_fontBody;
    io.FontGlobalScale = 1.0f / dpiScale;
}

std::string DetectProjectRoot() {
    const char* candidates[] = {".", "..", "../..", "../../.."};
    for (const char* root : candidates) {
        const std::string testPath = std::string(root) + "/CMakeLists.txt";
        FILE* file = std::fopen(testPath.c_str(), "r");
        if (file != nullptr) {
            std::fclose(file);
            return root;
        }
    }
    return ".";
}

void InitialiseData(cw1::Blockchain& chain) {
    bool loadedFromDatabase = false;

    try {
        if (!chain.openDatabase(g_projectRoot + "/database/cw1_blockchain.db")) {
            PushToast("SQLite database unavailable. Continuing with in-memory data.",
                      COL_YELLOW, 4.0f);
        }

        if (chain.isDatabaseOpen()) {
            const bool dbLoadOk = chain.loadFromDB();
            loadedFromDatabase = dbLoadOk && chain.totalBlocks() > 0;
            if (!loadedFromDatabase) {
                PushToast(dbLoadOk
                              ? "No persisted chain found. Seeding demo blockchain data instead."
                              : "Database load failed. Seeding demo blockchain data instead.",
                          COL_YELLOW, 4.0f);
            }
        }

        if (!loadedFromDatabase) {
            LoadDemoData(chain);
            if (chain.isDatabaseOpen() && !chain.saveToDB()) {
                PushToast("Database sync failed. Continuing with in-memory chain.",
                          COL_YELLOW, 4.0f);
            }
        }

        RefreshChainVerification(chain);

        if (g_fuelMgr.openDatabase(g_projectRoot + "/database/fuel_prices.db")) {
            g_fuelMgr.seedFallbackDataIfEmpty();
            g_fuelMgr.loadRecentHistory(52);
            g_fuelInitialized = true;
        }

        if (chain.isDatabaseOpen()) {
            cw1::DatabaseManager* db = chain.getDatabase();
            if (db != nullptr) {
                g_pendingMgr.attach(db->rawHandle());
            }
        }
    } catch (const std::exception& ex) {
        ReportGuiException(chain, cw1::AuditAction::PERSISTENCE_IO,
                           "Startup initialisation failed", ex);
        if (chain.totalBlocks() == 0) {
            LoadDemoData(chain);
        }
        RefreshChainVerification(chain);
    }
}

void UpdateTempChainAutoGeneration() {
    if (!g_tempGenerating || g_tempChain.empty()) {
        return;
    }

    g_tempAccumulator += ImGui::GetIO().DeltaTime;
    if (g_tempAccumulator < g_tempInterval) {
        return;
    }
    g_tempAccumulator -= g_tempInterval;

    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    constexpr int kManufacturerCount = 18;
    constexpr int kColorCount = 12;
    const int manufacturerIndex = std::rand() % kManufacturerCount;
    const ModelList& models = k_modelsByMfr[manufacturerIndex];
    const int modelIndex = std::rand() % models.count;
    const int colorIndex = std::rand() % kColorCount;

    cw1::CarRecord record;
    record.vin = "TEMP-" + std::to_string(g_tempChain.size());
    record.manufacturer = k_manufacturers[manufacturerIndex];
    record.model = models.models[modelIndex];
    record.color = k_colors[colorIndex];
    record.productionYear = 2024 + (std::rand() % 3);
    record.manufacturerId = "MFR-TEMP";
    record.stage = static_cast<cw1::BlockStage>(std::rand() % 5);

    switch (record.stage) {
        case cw1::BlockStage::PRODUCTION:
            record.factoryLocation = "Demo Factory";
            break;
        case cw1::BlockStage::WAREHOUSE_INTAKE:
            record.warehouseLocation = "WH-TEMP";
            record.receivedBy = "Demo Receiver";
            record.supplierId = "SUP-TEMP";
            break;
        case cw1::BlockStage::QUALITY_CHECK:
            record.inspectorId = "QC-TEMP";
            record.passed = (std::rand() % 2 == 0);
            record.qcNotes = "Auto-generated QC";
            break;
        case cw1::BlockStage::DEALER_DISPATCH:
            record.dealerId = "DLR-TEMP";
            record.destination = "Demo City";
            record.transportMode = k_transportModes[std::rand() % 4];
            break;
        case cw1::BlockStage::CUSTOMER_SALE:
            record.buyerId = "CUST-TEMP";
            record.retailerId = "RTL-TEMP";
            record.salePrice = 30000.0 + static_cast<double>(std::rand() % 70000);
            record.warrantyExpiry = "2030-01-01";
            break;
    }

    const std::string previousHash = g_tempChain.back().getCurrentHash();
    g_tempChain.emplace_back(g_tempChain.size(), previousHash, record);
    PushToast("Temp block #" + std::to_string(g_tempChain.size() - 1)
                  + " generated (not saved to DB)",
              COL_ACCENT);
}

void RenderContent(cw1::Blockchain& chain) {
    g_chatbot.pollResponse();

    switch (g_view) {
        case View::DASHBOARD: RenderDashboard(chain); break;
        case View::CAR_DETAIL: RenderCarDetail(chain); break;
        case View::ADD_BLOCK: RenderAddBlock(chain); break;
        case View::PENDING_APPROVALS: RenderPendingApprovals(chain); break;
        case View::GLOBAL_CHAIN: RenderGlobalChain(chain); break;
        case View::AUDIT_LOG: RenderAuditLog(chain); break;
        case View::INTEGRITY: RenderIntegrity(chain); break;
        case View::DELETE_BLOCK: RenderDeleteBlock(chain); break;
        case View::AI_ASSISTANT: RenderAIAssistant(chain); break;
    }
}

void RenderFrame(GLFWwindow* window, cw1::Blockchain& chain) {
    ImGuiIO& io = ImGui::GetIO();

    if (g_showLoginScreen || !g_authMgr.isLoggedIn()) {
        RenderLoginScreen(chain);
        RenderToasts(io.DeltaTime);

        ImGui::Render();
        int displayWidth = 0;
        int displayHeight = 0;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        return;
    }

    UpdateTempChainAutoGeneration();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_MAIN);
    ImGui::Begin("##root", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoScrollbar |
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
    ImGui::BeginChild("##contentInner", ImVec2(-1, -1), false);
    ImGui::PopStyleColor();
    ImGui::Spacing();
    ImGui::Spacing();

    RenderContent(chain);

    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::End();

    RenderToasts(io.DeltaTime);

    ImGui::Render();
    int displayWidth = 0;
    int displayHeight = 0;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(0.051f, 0.067f, 0.090f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

}  // namespace

int main() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window =
        glfwCreateWindow(1400, 900, "Car Warehouse Blockchain System", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    float dpiScale = 1.0f;
    {
        float xscale = 1.0f;
        float yscale = 1.0f;
        glfwGetWindowContentScale(window, &xscale, &yscale);
        dpiScale = xscale;
        if (dpiScale < 1.0f) {
            dpiScale = 1.0f;
        }
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ApplyGitHubDarkTheme();
    LoadFonts(io, dpiScale);

    g_projectRoot = DetectProjectRoot();

    cw1::Blockchain chain;
    InitialiseData(chain);

    g_chatbot.setProjectRoot(g_projectRoot);
    g_chatbot.loadApiKey();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderFrame(window, chain);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
