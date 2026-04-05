// Read-only chatbot service used by the AI assistant panel.

#include "ai/ChatbotService.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>
#include <sqlite3.h>

#include "blockchain/Blockchain.hpp"
#include "blockchain/BlockStage.hpp"
#include "security/Auth.hpp"
#include "security/PendingApprovalManager.hpp"
#include "security/Role.hpp"
#include "utils/AuditLog.hpp"
#include "utils/FuelPriceManager.hpp"
#include "utils/FuelPriceTypes.hpp"

namespace ai {

// Small local helpers.

static const char* stageName(cw1::BlockStage s) {
    switch (s) {
        case cw1::BlockStage::PRODUCTION:       return "PRODUCTION";
        case cw1::BlockStage::WAREHOUSE_INTAKE:  return "WAREHOUSE_INTAKE";
        case cw1::BlockStage::QUALITY_CHECK:     return "QUALITY_CHECK";
        case cw1::BlockStage::DEALER_DISPATCH:   return "DEALER_DISPATCH";
        case cw1::BlockStage::CUSTOMER_SALE:     return "CUSTOMER_SALE";
        default:                                  return "UNKNOWN";
    }
}

// Converts a narrow UTF-8 string to a wide string for the WinHTTP API.
static std::wstring toWide(const std::string& s) {
    if (s.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(),
                                  static_cast<int>(s.size()), nullptr, 0);
    std::wstring ws(static_cast<size_t>(len), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(),
                        static_cast<int>(s.size()), &ws[0], len);
    return ws;
}

// Minimal SQLite result collector used by the read-only DB helpers.
struct SqlRows {
    std::vector<std::string>              columns;
    std::vector<std::vector<std::string>> rows;
};

static int sqlCb(void* data, int argc, char** argv, char** colNames) {
    auto* r = static_cast<SqlRows*>(data);
    if (r->columns.empty()) {
        for (int i = 0; i < argc; ++i)
            r->columns.push_back(colNames[i] ? colNames[i] : "");
    }
    r->rows.emplace_back();
    auto& row = r->rows.back();
    for (int i = 0; i < argc; ++i)
        row.push_back(argv[i] ? argv[i] : "NULL");
    return 0;
}

// Construction and cleanup.

ChatbotService::ChatbotService() = default;

ChatbotService::~ChatbotService() {
    if (worker_.joinable()) worker_.join();
}

// API key loading.

bool ChatbotService::loadApiKey() {
    // 1) Try reading from env/gemini_api_key.txt relative to project root.
    if (!projectRoot_.empty()) {
        std::ifstream f(projectRoot_ + "/env/gemini_api_key.txt");
        if (f.is_open()) {
            std::string line;
            if (std::getline(f, line)) {
                // Trim whitespace / carriage returns.
                while (!line.empty() && (line.back() == '\r' || line.back() == '\n'
                       || line.back() == ' ' || line.back() == '\t'))
                    line.pop_back();
                if (!line.empty() && line != "PASTE_YOUR_GEMINI_API_KEY_HERE") {
                    apiKey_ = line;
                    return true;
                }
            }
        }
    }

    // 2) Also try common relative paths (CWD might be the build dir).
    for (const char* prefix : {".", "..", "../..", "../../.."}) {
        std::ifstream f(std::string(prefix) + "/env/gemini_api_key.txt");
        if (f.is_open()) {
            std::string line;
            if (std::getline(f, line)) {
                while (!line.empty() && (line.back() == '\r' || line.back() == '\n'
                       || line.back() == ' ' || line.back() == '\t'))
                    line.pop_back();
                if (!line.empty() && line != "PASTE_YOUR_GEMINI_API_KEY_HERE") {
                    apiKey_ = line;
                    return true;
                }
            }
        }
    }

    // 3) Fall back to environment variable.
    const char* key = std::getenv("GEMINI_API_KEY");
    if (key && key[0] != '\0') { apiKey_ = key; return true; }

    apiKey_.clear();
    return false;
}

bool        ChatbotService::hasApiKey()    const noexcept { return !apiKey_.empty(); }
std::string ChatbotService::statusText()   const {
    if (waiting_) return "Thinking...";
    if (apiKey_.empty()) return "Missing API Key";
    return "API Ready";
}
void               ChatbotService::setProjectRoot(const std::string& r) { projectRoot_ = r; }
const std::string& ChatbotService::projectRoot()  const noexcept        { return projectRoot_; }
void               ChatbotService::setOnlineMode(bool o) noexcept       { onlineMode_ = o; }
bool               ChatbotService::isOnlineMode() const noexcept        { return onlineMode_; }

// Chat flow.

void ChatbotService::sendMessage(const std::string& userMessage,
                                  cw1::Blockchain& chain,
                                  const cw1::FuelPriceManager& fuelMgr,
                                  const cw1::PendingApprovalManager& pendingMgr,
                                  const cw1::AuthManager& authMgr) {
    if (waiting_) return;

    if (apiKey_.empty()) {
        history_.push_back({ChatMessage::Role::ASSISTANT,
            "Error: No API key found. Place your Gemini API key in env/gemini_api_key.txt and restart the application.",
            currentTimestamp(), true});
        return;
    }

    // Record the user's message.
    history_.push_back({ChatMessage::Role::USER, userMessage, currentTimestamp()});

    // Build context strings on the main thread (safe to read app state here).
    std::string sysPrompt  = buildSystemPrompt();
    std::string context    = buildContext(chain, fuelMgr, pendingMgr, authMgr);
    std::string fullSystem = sysPrompt + "\n\n" + context;

    // Collect recent conversation turns for multi-turn context.
    std::vector<ChatMessage> recent;
    const size_t maxTurns = 20;
    const size_t start = history_.size() > maxTurns ? history_.size() - maxTurns : 0;
    for (size_t i = start; i < history_.size(); ++i)
        recent.push_back(history_[i]);

    waiting_ = true;
    if (worker_.joinable()) worker_.join();

    worker_ = std::thread([this, fullSystem = std::move(fullSystem),
                           recent = std::move(recent)]() {
        std::string raw   = callGeminiApi(fullSystem, recent);
        bool        isErr = (raw.size() >= 6 && raw.substr(0, 6) == "ERROR:");
        std::string text  = isErr ? raw : cleanMarkdown(raw);

        std::lock_guard<std::mutex> lk(pendingMutex_);
        pending_.content = std::move(text);
        pending_.isError = isErr;
        pending_.ready   = true;
    });
}

void ChatbotService::pollResponse() {
    std::lock_guard<std::mutex> lk(pendingMutex_);
    if (pending_.ready) {
        history_.push_back({ChatMessage::Role::ASSISTANT,
                           std::move(pending_.content),
                           currentTimestamp(),
                           pending_.isError});
        pending_.ready = false;
        waiting_ = false;
    }
}

bool ChatbotService::isWaiting() const noexcept { return waiting_; }
const std::vector<ChatMessage>& ChatbotService::history() const noexcept { return history_; }
void ChatbotService::clearHistory() { if (!waiting_) history_.clear(); }

ContextOptions&       ChatbotService::options()       noexcept { return opts_; }
const ContextOptions& ChatbotService::options() const noexcept { return opts_; }

// Static lists used by project mode.

const std::vector<std::string>& ChatbotService::availableCodeFiles() {
    static const std::vector<std::string> files = {
        "src/main_gui.cpp",
        "src/blockchain/Blockchain.cpp",
        "src/blockchain/Block.cpp",
        "src/security/Auth.cpp",
        "src/security/PendingApprovalManager.cpp",
        "src/security/AccessControl.cpp",
        "src/utils/DatabaseManager.cpp",
        "src/utils/FuelPriceManager.cpp",
        "src/utils/AuditLog.cpp",
        "include/blockchain/Blockchain.hpp",
        "include/blockchain/Block.hpp",
        "include/security/Auth.hpp",
        "include/security/PendingApprovalManager.hpp",
        "include/security/AccessControl.hpp",
        "include/security/Role.hpp",
        "include/utils/DatabaseManager.hpp",
        "include/utils/AuditLog.hpp",
        "include/utils/FuelPriceManager.hpp",
        "CMakeLists.txt",
    };
    return files;
}

const std::vector<std::string>& ChatbotService::availableDbTables() {
    static const std::vector<std::string> tables = {
        "blocks",
        "pending_requests",
        "audit_log",
        "fuel_prices",
        "deleted_originals",
    };
    return tables;
}

// Read-only file access.

bool ChatbotService::isPathSafe(const std::string& p) const {
    if (p.find("..") != std::string::npos) return false;
    if (p.find(':')  != std::string::npos) return false;
    if (!p.empty() && (p[0] == '/' || p[0] == '\\')) return false;
    return true;
}

std::string ChatbotService::readCodeFile(const std::string& relativePath) const {
    if (!isPathSafe(relativePath))
        return "[Path not allowed: " + relativePath + "]";
    if (projectRoot_.empty())
        return "[Project root not configured — code file reading unavailable]";

    std::string fullPath = projectRoot_ + "/" + relativePath;
    std::ifstream file(fullPath);
    if (!file.is_open())
        return "[Could not open file: " + relativePath + "]";

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    constexpr size_t kMax = 8000;
    if (content.size() > kMax)
        content = content.substr(0, kMax)
                + "\n\n[... truncated at " + std::to_string(kMax) + " characters ...]";
    return content;
}

// Read-only database inspection.

std::string ChatbotService::getTableSummary(sqlite3* db,
                                             const std::string& table,
                                             int maxRows) const {
    if (!db) return "[No database connection]\n";

    // Only allow our known table names.
    const auto& allowed = availableDbTables();
    if (std::find(allowed.begin(), allowed.end(), table) == allowed.end())
        return "[Table '" + table + "' not in allowed list]\n";

    std::ostringstream out;

    // Row count.
    {
        SqlRows r;
        std::string sql = "SELECT COUNT(*) FROM [" + table + "]";
        sqlite3_exec(db, sql.c_str(), sqlCb, &r, nullptr);
        std::string cnt = (!r.rows.empty() && !r.rows[0].empty()) ? r.rows[0][0] : "?";
        out << "Table: " << table << " (" << cnt << " rows)\n";
    }

    // Column schema.
    {
        SqlRows r;
        std::string sql = "PRAGMA table_info([" + table + "])";
        sqlite3_exec(db, sql.c_str(), sqlCb, &r, nullptr);
        out << "  Columns: ";
        for (size_t i = 0; i < r.rows.size(); ++i) {
            if (i > 0) out << ", ";
            if (r.rows[i].size() > 2)
                out << r.rows[i][1] << "(" << r.rows[i][2] << ")";
            else if (r.rows[i].size() > 1)
                out << r.rows[i][1];
        }
        out << "\n";
    }

    // Pick a concise column set for known tables so context stays small.
    std::string cols = "*";
    if (table == "blocks")
        cols = "block_index, vin, manufacturer, model, stage, timestamp, created_by, is_tombstone";
    else if (table == "pending_requests")
        cols = "request_id, requested_by, requested_stage, vin, status, approved_by, request_timestamp";
    else if (table == "audit_log")
        cols = "id, action, details, timestamp";
    else if (table == "fuel_prices")
        cols = "effective_date, ron95, ron97, diesel_peninsular, diesel_east";
    else if (table == "deleted_originals")
        cols = "block_index, vin, manufacturer, model, stage";

    // Latest N rows.
    {
        SqlRows r;
        std::string sql = "SELECT " + cols + " FROM [" + table
                        + "] ORDER BY rowid DESC LIMIT " + std::to_string(maxRows);
        sqlite3_exec(db, sql.c_str(), sqlCb, &r, nullptr);
        if (!r.rows.empty()) {
            out << "  Latest " << r.rows.size() << " rows:\n    ";
            for (size_t i = 0; i < r.columns.size(); ++i) {
                if (i > 0) out << " | ";
                out << r.columns[i];
            }
            out << "\n";
            for (const auto& row : r.rows) {
                out << "    ";
                for (size_t i = 0; i < row.size(); ++i) {
                    if (i > 0) out << " | ";
                    std::string v = row[i];
                    if (v.size() > 40) v = v.substr(0, 37) + "...";
                    out << v;
                }
                out << "\n";
            }
        }
    }
    return out.str();
}

std::string ChatbotService::getDbOverview(sqlite3* db) const {
    if (!db) return "[No database connection]\n";

    std::ostringstream out;
    out << "=== DATABASE OVERVIEW ===\n";

    SqlRows tables;
    sqlite3_exec(db,
        "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name",
        sqlCb, &tables, nullptr);

    for (const auto& row : tables.rows) {
        if (row.empty()) continue;
        SqlRows cnt;
        std::string sql = "SELECT COUNT(*) FROM [" + row[0] + "]";
        sqlite3_exec(db, sql.c_str(), sqlCb, &cnt, nullptr);
        std::string n = (!cnt.rows.empty() && !cnt.rows[0].empty()) ? cnt.rows[0][0] : "?";
        out << "  " << row[0] << ": " << n << " rows\n";
    }
    return out.str();
}

// Prompt building.

std::string ChatbotService::buildSystemPrompt() const {
    if (onlineMode_) {
        return
            "You are a helpful, knowledgeable AI assistant. Answer any question the "
            "user asks using your general knowledge. You can discuss any topic "
            "freely including science, math, programming, history, current events, "
            "advice, explanations, and anything else.\n\n"
            "RULES:\n"
            "1. Be helpful and answer all questions to the best of your ability.\n"
            "2. Keep answers concise and practical.\n"
            "3. Do NOT use markdown formatting like **bold**, ### headings, or "
            "```code blocks``` in your response.  Use plain text only.\n"
            "4. When listing items use simple dashes (-) or numbers.\n";
    }

    return
        "You are a helpful AI assistant integrated into a Car Warehouse Blockchain "
        "System. This is a C++ coursework project that tracks vehicle supply chain "
        "stages using a blockchain data structure.\n\n"
        "RULES:\n"
        "1. You are READ-ONLY. You cannot modify any data, code, database, "
        "blockchain, or system state.\n"
        "2. Answer questions based on the provided context.\n"
        "3. If context is insufficient to answer, say so clearly.\n"
        "4. Keep answers concise and practical.\n"
        "5. Do NOT use markdown formatting like **bold**, ### headings, or "
        "```code blocks``` in your response.  Use plain text only.\n"
        "6. When listing items use simple dashes (-) or numbers.\n"
        "7. For code references describe them in plain text.\n\n"
        "System overview:\n"
        "- Blockchain: car records as blocks with SHA-256 + SHA-3 dual hashes, "
        "linked-list mirror for coursework data-structure requirement.\n"
        "- 5 supply-chain stages: PRODUCTION, WAREHOUSE_INTAKE, QUALITY_CHECK, "
        "DEALER_DISPATCH, CUSTOMER_SALE.\n"
        "- RBAC with 5 roles: ADMIN (full access), WAREHOUSE_STAFF, QC_INSPECTOR, "
        "DEALER, AUDITOR (read-only).\n"
        "- Pending approval workflow: staff submit block requests, admin approves "
        "or rejects.\n"
        "- SQLite database for persistence (tables: blocks, audit_log, "
        "pending_requests, fuel_prices, deleted_originals).\n"
        "- Linked-list audit log for event tracking.\n"
        "- Fuel price intelligence module with forecasting.\n"
        "- Soft delete and restore for blocks (tombstone pattern).";
}

// Collect live context for project mode.
std::string ChatbotService::buildContext(
        cw1::Blockchain& chain,
        const cw1::FuelPriceManager& fuelMgr,
        const cw1::PendingApprovalManager& pendingMgr,
        const cw1::AuthManager& authMgr) const {

    // Online mode: no project context at all.
    if (onlineMode_) return {};

    std::ostringstream ctx;

    // Basic app state always goes in project mode.
    {
        ctx << "=== CURRENT APPLICATION STATE ===\n";

        if (authMgr.isLoggedIn()) {
            const auto& u = authMgr.currentUser();
            ctx << "Logged in as: " << u.username
                << " (" << cw1::roleToString(u.role) << ")\n";
        }

        ctx << "Total blockchain blocks: " << chain.totalBlocks() << "\n";
        const auto allVins = chain.getAllVins();
        ctx << "Total vehicles: " << allVins.size() << "\n";
        ctx << "Pending approval requests: " << pendingMgr.pendingCount() << "\n\n";

        // Per-vehicle summary with block details.
        ctx << "Vehicle details:\n";
        for (const auto& vin : allVins) {
            const auto hist = chain.getCarHistory(vin);
            if (hist.empty()) continue;
            const auto& first = hist.front()->getRecord();
            ctx << vin << " (" << first.manufacturer << " " << first.model
                << ", " << first.color << ", " << first.productionYear << "):\n";
            for (const auto* blk : hist) {
                const auto& r = blk->getRecord();
                ctx << "  #" << blk->getIndex() << " " << stageName(r.stage) << ": ";
                switch (r.stage) {
                    case cw1::BlockStage::PRODUCTION:
                        ctx << "factory=" << r.factoryLocation; break;
                    case cw1::BlockStage::WAREHOUSE_INTAKE:
                        ctx << "warehouse=" << r.warehouseLocation
                            << ", receivedBy=" << r.receivedBy; break;
                    case cw1::BlockStage::QUALITY_CHECK:
                        ctx << "inspector=" << r.inspectorId
                            << ", passed=" << (r.passed ? "yes" : "no")
                            << ", notes=" << r.qcNotes; break;
                    case cw1::BlockStage::DEALER_DISPATCH:
                        ctx << "dealer=" << r.dealerId
                            << ", destination=" << r.destination
                            << ", transport=" << r.transportMode; break;
                    case cw1::BlockStage::CUSTOMER_SALE:
                        ctx << "buyer=" << r.buyerId
                            << ", price=" << r.salePrice
                            << ", warranty=" << r.warrantyExpiry; break;
                }
                if (!blk->getCreatedBy().empty())
                    ctx << ", createdBy=" << blk->getCreatedBy();
                ctx << "\n";
            }
        }

        // Pending request details.
        if (pendingMgr.pendingCount() > 0) {
            ctx << "\nPending requests:\n";
            for (const auto& rq : pendingMgr.getPendingRequests()) {
                ctx << "  #" << rq.requestId << " " << rq.payload.vin
                    << " " << stageName(rq.requestedStage)
                    << " by " << rq.requestedBy
                    << " at " << rq.requestTimestamp << "\n";
            }
        }

        // All-request summary.
        {
            const auto& all = pendingMgr.getAllRequests();
            int ap = 0, rj = 0;
            for (const auto& rq : all) {
                if (rq.status == cw1::RequestStatus::APPROVED) ++ap;
                else if (rq.status == cw1::RequestStatus::REJECTED) ++rj;
            }
            ctx << "Total requests ever: " << all.size()
                << " (pending=" << pendingMgr.pendingCount()
                << ", approved=" << ap << ", rejected=" << rj << ")\n";
        }
        ctx << "\n";
    }

    // Fuel data is cheap to include and useful on the dashboard side.
    {
        ctx << "=== FUEL PRICE DATA ===\n";
        const auto& hist = fuelMgr.history();
        if (!hist.empty()) {
            const auto& latest = hist.back();
            ctx << "Latest effective date: " << latest.effectiveDate << "\n"
                << "RON95: RM " << latest.ron95 << "/L | "
                << "RON97: RM " << latest.ron97 << "/L | "
                << "Diesel(Pen): RM " << latest.dieselPeninsular << "/L | "
                << "Diesel(East): RM " << latest.dieselEast << "/L\n";
            auto f95 = fuelMgr.forecast(cw1::FuelSeries::RON95);
            auto f97 = fuelMgr.forecast(cw1::FuelSeries::RON97);
            ctx << "RON95 forecast: current=" << f95.currentPrice
                << " predicted=" << f95.predictedNextPrice
                << " trend=" << (f95.stable ? "Stable" : (f95.increasing ? "Up" : "Down")) << "\n";
            ctx << "RON97 forecast: current=" << f97.currentPrice
                << " predicted=" << f97.predictedNextPrice
                << " trend=" << (f97.stable ? "Stable" : (f97.increasing ? "Up" : "Down")) << "\n";
        } else {
            ctx << "No fuel price data available.\n";
        }
        ctx << "\n";
    }

    // Recent audit entries help explain what the user just did.
    {
        ctx << "=== RECENT AUDIT LOG (last 10) ===\n";
        auto entries = chain.getAuditLog().getRecentEntries(10);
        if (entries.empty()) {
            ctx << "No audit entries.\n";
        } else {
            for (const auto* e : entries)
                ctx << "[" << e->timestamp << "] "
                    << cw1::actionToString(e->action) << ": "
                    << e->details << "\n";
        }
        ctx << "\n";
    }

    // Table counts and samples help with database questions.
    if (chain.isDatabaseOpen()) {
        sqlite3* db = chain.getDatabase()->rawHandle();
        if (db) {
            ctx << getDbOverview(db) << "\n";
            // Include summary of each known table.
            for (const auto& tbl : availableDbTables()) {
                ctx << getTableSummary(db, tbl, 5) << "\n";
            }
        }
    }

    return ctx.str();
}

// Worker-thread API call.

std::string ChatbotService::callGeminiApi(
        const std::string& systemPrompt,
        const std::vector<ChatMessage>& msgs) const {

    // Build the JSON payload expected by Gemini.
    std::string body;
    body.reserve(systemPrompt.size() + 4096);
    body += "{\n  \"systemInstruction\":{\"parts\":[{\"text\":\"";
    body += jsonEscape(systemPrompt);
    body += "\"}]},\n  \"contents\":[\n";

    bool first = true;
    for (const auto& m : msgs) {
        if (!first) body += ",\n";
        first = false;
        const char* role = (m.role == ChatMessage::Role::USER) ? "user" : "model";
        body += "    {\"role\":\"";
        body += role;
        body += "\",\"parts\":[{\"text\":\"";
        body += jsonEscape(m.content);
        body += "\"}]}";
    }

    body += "\n  ],\n"
            "  \"generationConfig\":{\"temperature\":0.7,\"maxOutputTokens\":4096}\n"
            "}";

    // Send the request with WinHTTP to avoid extra runtime dependencies.
    const std::wstring path = toWide(
        "/v1beta/models/gemini-2.5-flash:generateContent?key=" + apiKey_);

    HINTERNET hSession = WinHttpOpen(
        L"CarWarehouseBlockchain/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "ERROR: Failed to open HTTP session.";
    WinHttpSetTimeouts(hSession, 30000, 30000, 30000, 30000);

    HINTERNET hConnect = WinHttpConnect(
        hSession, L"generativelanguage.googleapis.com",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return "ERROR: Failed to connect to Gemini API server.";
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"POST", path.c_str(),
        nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "ERROR: Failed to create HTTP request.";
    }

    const wchar_t* hdrs = L"Content-Type: application/json\r\n";
    BOOL ok = WinHttpSendRequest(
        hRequest, hdrs, static_cast<DWORD>(-1L),
        const_cast<char*>(body.c_str()),
        static_cast<DWORD>(body.size()),
        static_cast<DWORD>(body.size()), 0);
    if (!ok) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "ERROR: Failed to send request (check internet connection).";
    }

    if (!WinHttpReceiveResponse(hRequest, nullptr)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "ERROR: No response received from Gemini API.";
    }

    // Read the full response body.
    std::string response;
    DWORD bytesAvail = 0;
    do {
        bytesAvail = 0;
        WinHttpQueryDataAvailable(hRequest, &bytesAvail);
        if (bytesAvail > 0) {
            std::vector<char> buf(bytesAvail);
            DWORD bytesRead = 0;
            WinHttpReadData(hRequest, buf.data(), bytesAvail, &bytesRead);
            response.append(buf.data(), bytesRead);
        }
    } while (bytesAvail > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    // Pull out a readable API error when the request fails.
    if (response.find("\"error\"") != std::string::npos) {
        size_t pos = response.find("\"message\"");
        if (pos != std::string::npos) {
            pos = response.find('"', pos + 10);
            if (pos != std::string::npos) {
                pos = response.find('"', pos);
                if (pos != std::string::npos) {
                    ++pos;
                    size_t end = response.find('"', pos);
                    if (end != std::string::npos)
                        return "ERROR: Gemini API — "
                             + response.substr(pos, end - pos);
                }
            }
        }
        return "ERROR: Gemini API returned an error response.";
    }

    return extractJsonText(response);
}

// JSON helpers

std::string ChatbotService::jsonEscape(const std::string& s) {
    std::string r;
    r.reserve(s.size() + 64);
    for (char c : s) {
        switch (c) {
            case '"':  r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\n': r += "\\n";  break;
            case '\r': r += "\\r";  break;
            case '\t': r += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x",
                                  static_cast<unsigned int>(
                                      static_cast<unsigned char>(c)));
                    r += buf;
                } else {
                    r += c;
                }
        }
    }
    return r;
}

std::string ChatbotService::extractJsonText(const std::string& json) {
    // Locate the first "text" key inside the Gemini response and return its
    // string value, properly unescaping JSON sequences.
    const std::string key = "\"text\"";
    size_t pos = json.find(key);
    if (pos == std::string::npos)
        return "ERROR: Could not parse API response (no 'text' key).";

    pos = json.find(':', pos + key.size());
    if (pos == std::string::npos)
        return "ERROR: Malformed API response.";

    pos = json.find('"', pos + 1);
    if (pos == std::string::npos)
        return "ERROR: Malformed API response.";
    ++pos;  // step past opening quote

    std::string result;
    result.reserve(json.size() - pos);
    while (pos < json.size()) {
        const char c = json[pos];
        if (c == '"') break;                       // closing quote
        if (c == '\\' && pos + 1 < json.size()) {
            const char nx = json[pos + 1];
            switch (nx) {
                case '"':  result += '"';  pos += 2; continue;
                case '\\': result += '\\'; pos += 2; continue;
                case '/':  result += '/';  pos += 2; continue;
                case 'n':  result += '\n'; pos += 2; continue;
                case 'r':  result += '\r'; pos += 2; continue;
                case 't':  result += '\t'; pos += 2; continue;
                case 'u': {
                    if (pos + 5 < json.size()) {
                        unsigned cp = 0;
                        std::sscanf(json.c_str() + pos + 2, "%4x", &cp);
                        if (cp < 0x80) {
                            result += static_cast<char>(cp);
                        } else if (cp < 0x800) {
                            result += static_cast<char>(0xC0 | (cp >> 6));
                            result += static_cast<char>(0x80 | (cp & 0x3F));
                        } else {
                            result += static_cast<char>(0xE0 | (cp >> 12));
                            result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (cp & 0x3F));
                        }
                        pos += 6;
                        continue;
                    }
                    break;
                }
                default: break;
            }
        }
        result += c;
        ++pos;
    }
    return result;
}

// Strip markdown before showing the reply in ImGui.

std::string ChatbotService::cleanMarkdown(const std::string& raw) {
    if (raw.empty()) return raw;

    std::string result;
    result.reserve(raw.size());
    std::istringstream stream(raw);
    std::string line;

    while (std::getline(stream, line)) {
        // Drop code-fence lines entirely.
        {
            size_t p = line.find("```");
            if (p != std::string::npos) {
                // Keep any text before the fence on the same line.
                std::string before = line.substr(0, p);
                if (!before.empty() &&
                    before.find_first_not_of(' ') != std::string::npos)
                    result += before + "\n";
                continue;
            }
        }

        // Strip heading markers  (### Heading -> Heading).
        {
            size_t i = 0;
            while (i < line.size() && line[i] == '#') ++i;
            if (i > 0 && i < line.size() && line[i] == ' ')
                line = line.substr(i + 1);
        }

        // Strip blockquote markers.
        if (line.size() >= 2 && line[0] == '>' && line[1] == ' ')
            line = line.substr(2);

        // Remove **bold** and *italic* markers; convert bullet * to -.
        std::string cleaned;
        cleaned.reserve(line.size());
        for (size_t i = 0; i < line.size(); ) {
            if (i + 1 < line.size() && line[i] == '*' && line[i + 1] == '*') {
                i += 2;                              // skip **
            } else if (line[i] == '*') {
                // If it looks like a bullet point (* text) at line start keep as -.
                bool atStart = (i == 0) ||
                    (line.substr(0, i).find_first_not_of(' ') == std::string::npos);
                if (atStart && i + 1 < line.size() && line[i + 1] == ' ') {
                    cleaned += '-';
                    ++i;
                } else {
                    ++i;                             // skip lone *
                }
            } else if (line[i] == '`') {
                ++i;                                 // strip backtick
            } else {
                cleaned += line[i];
                ++i;
            }
        }

        // Flatten [text](url) links -> just text.
        std::string final_line;
        final_line.reserve(cleaned.size());
        for (size_t i = 0; i < cleaned.size(); ) {
            if (cleaned[i] == '[') {
                size_t cb = cleaned.find(']', i + 1);
                if (cb != std::string::npos &&
                    cb + 1 < cleaned.size() && cleaned[cb + 1] == '(') {
                    size_t cp = cleaned.find(')', cb + 2);
                    if (cp != std::string::npos) {
                        final_line += cleaned.substr(i + 1, cb - i - 1);
                        i = cp + 1;
                        continue;
                    }
                }
            }
            final_line += cleaned[i];
            ++i;
        }

        result += final_line + "\n";
    }

    // Trim trailing blank lines.
    while (result.size() > 1 && result.back() == '\n' &&
           result[result.size() - 2] == '\n')
        result.pop_back();

    return result;
}

// Misc

std::string ChatbotService::currentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now));
    return buf;
}

}  // namespace ai
