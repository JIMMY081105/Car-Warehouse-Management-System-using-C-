// Read-only chatbot service for the AI assistant panel.

#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace cw1 {
    class Blockchain;
    class FuelPriceManager;
    class PendingApprovalManager;
    class AuthManager;
}
struct sqlite3;

namespace ai {

// One message shown in the chat history.
struct ChatMessage {
    enum class Role { USER, ASSISTANT };
    Role        role;
    std::string content;
    std::string timestamp;
    bool        isError = false;
};

// Optional context toggles for project mode.
struct ContextOptions {
    bool includeAppState     = true;
    bool includeCodeFile     = false;
    bool includeDbSummary    = false;
    bool includeAuditSummary = false;
    bool includeFuelSummary  = false;
    int  selectedFileIndex   = 0;
    int  selectedTableIndex  = 0;
};

// Owns chat history, context building, and Gemini requests.
class ChatbotService {
public:
    ChatbotService();
    ~ChatbotService();

    ChatbotService(const ChatbotService&)            = delete;
    ChatbotService& operator=(const ChatbotService&) = delete;

    // Setup.
    bool               loadApiKey();
    bool               hasApiKey()    const noexcept;
    std::string        statusText()   const;
    void               setProjectRoot(const std::string& root);
    const std::string& projectRoot()  const noexcept;
    void               setOnlineMode(bool online) noexcept;
    bool               isOnlineMode() const noexcept;

    // Chat flow.
    void sendMessage(const std::string& userMessage,
                     cw1::Blockchain& chain,
                     const cw1::FuelPriceManager& fuelMgr,
                     const cw1::PendingApprovalManager& pendingMgr,
                     const cw1::AuthManager& authMgr);
    void pollResponse();                                   // call every frame
    bool isWaiting()  const noexcept;
    const std::vector<ChatMessage>& history() const noexcept;
    void clearHistory();

    // Context options.
    ContextOptions&       options()       noexcept;
    const ContextOptions& options() const noexcept;

    // Static lists.
    static const std::vector<std::string>& availableCodeFiles();
    static const std::vector<std::string>& availableDbTables();

    // Read-only helpers.
    std::string readCodeFile(const std::string& relativePath) const;
    std::string getTableSummary(sqlite3* db, const std::string& table,
                                int maxRows = 10) const;
    std::string getDbOverview(sqlite3* db) const;

private:
    std::string buildSystemPrompt() const;
    std::string buildContext(cw1::Blockchain& chain,
                            const cw1::FuelPriceManager& fuelMgr,
                            const cw1::PendingApprovalManager& pendingMgr,
                            const cw1::AuthManager& authMgr) const;
    std::string callGeminiApi(const std::string& systemPrompt,
                              const std::vector<ChatMessage>& recentMsgs) const;

    static std::string cleanMarkdown(const std::string& raw);
    static std::string jsonEscape(const std::string& s);
    static std::string extractJsonText(const std::string& json);
    static std::string currentTimestamp();
    bool               isPathSafe(const std::string& relativePath) const;

    // Stored state.
    std::string              apiKey_;
    std::string              projectRoot_;
    std::vector<ChatMessage> history_;
    ContextOptions           opts_;
    bool                     onlineMode_ = true;

    // Worker-thread result buffer.
    std::atomic<bool> waiting_{false};
    std::thread       worker_;
    std::mutex        pendingMutex_;
    struct PendingResult {
        std::string content;
        bool        isError = false;
        bool        ready   = false;
    } pending_;
};

}  // namespace ai
