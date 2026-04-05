// Renders the AI assistant page and chat controls.

#include "ui/GuiApp.hpp"

void RenderAIAssistant(cw1::Blockchain& chain) {
    g_chatbot.pollResponse();

    if (g_fontTitle != nullptr) {
        ImGui::PushFont(g_fontTitle);
    }
    ImGui::TextColored(COL_ACCENT, "AI Assistant");
    if (g_fontTitle != nullptr) {
        ImGui::PopFont();
    }

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 140.0f + ImGui::GetCursorPosX());
    if (!g_chatbot.hasApiKey()) {
        DrawMetricBadge("Missing API Key", COL_RED, HexColor(0xda3633, 0.18f));
    } else if (g_chatbot.isWaiting()) {
        DrawMetricBadge("Thinking...", COL_YELLOW, HexColor(0xd29922, 0.16f));
    } else {
        DrawMetricBadge("Read-only", COL_GREEN_BR, HexColor(0x238636, 0.16f));
    }

    ImGui::TextColored(COL_MUTED,
                       "Ask any question online, or switch to Project mode to query code and database.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    DrawSectionHeading("Information Source");
    ImGui::Spacing();

    {
        const bool isOnline = !g_chatProjectMode;
        if (isOnline) {
            ImGui::PushStyleColor(ImGuiCol_Button, COL_ACCENT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x1158c7));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, COL_BG_ELEV);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, COL_BG_HOVER);
        }
        if (ImGui::Button("  Online  ", ImVec2(120.0f, 32.0f))) {
            g_chatProjectMode = false;
            g_chatbot.setOnlineMode(true);
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();

    {
        const bool isProject = g_chatProjectMode;
        if (isProject) {
            ImGui::PushStyleColor(ImGuiCol_Button, COL_ACCENT);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_ACCENT_HO);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, HexColor(0x1158c7));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, COL_BG_ELEV);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COL_BG_HOVER);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, COL_BG_HOVER);
        }
        if (ImGui::Button("  Project  ", ImVec2(120.0f, 32.0f))) {
            g_chatProjectMode = true;
            g_chatbot.setOnlineMode(false);
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    ImGui::TextColored(COL_VERY_MUTED, g_chatProjectMode
                                           ? "  Answers using your project code and database"
                                           : "  Answers using general online knowledge");

    if (g_chatProjectMode) {
        ImGui::Spacing();
        DrawSectionHeading("Quick Actions");

        const auto quickButton = [&](const char* label, const char* question) {
            if (DrawPrimaryButton(label) && !g_chatbot.isWaiting()) {
                g_chatbot.sendMessage(question, chain, g_fuelMgr, g_pendingMgr, g_authMgr);
            }
        };

        quickButton("Summarize Chain",
                    "Summarize the current blockchain including total blocks, vehicles, and their latest stages.");
        ImGui::SameLine();
        quickButton("Fuel Summary", "What are the latest fuel prices and forecasts?");
        ImGui::SameLine();
        quickButton("Explain Auth",
                    "Explain how the authentication and role-based access control system works in this project.");

        quickButton("Pending Workflow",
                    "Explain how the pending approval workflow works, including submission, approval, and rejection.");
        ImGui::SameLine();
        quickButton("Summarize DB",
                    "Summarize all database tables, their schemas, and row counts.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    constexpr float kBottomHeight = 44.0f;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::BeginChild("##chatHistory", ImVec2(-1.0f, -kBottomHeight), true);
    ImGui::SetWindowFontScale(1.2f);

    const auto& messages = g_chatbot.history();
    if (messages.empty() && !g_chatbot.isWaiting()) {
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::TextColored(COL_VERY_MUTED,
                           "No messages yet. Type a question below or use a quick action above.");
    }

    static std::size_t previousMessageCount = 0;
    const bool newMessage = (messages.size() != previousMessageCount);
    previousMessageCount = messages.size();

    for (std::size_t i = 0; i < messages.size(); ++i) {
        const auto& message = messages[i];
        ImGui::PushID(static_cast<int>(i));

        const bool isUser = (message.role == ai::ChatMessage::Role::USER);
        if (g_fontSection != nullptr) {
            ImGui::PushFont(g_fontSection);
        }
        ImGui::TextColored(isUser ? COL_ACCENT_HO : (message.isError ? COL_RED : COL_GREEN_BR),
                           "%s", isUser ? "You" : "AI Assistant");
        if (g_fontSection != nullptr) {
            ImGui::PopFont();
        }

        ImGui::SameLine();
        ImGui::TextColored(COL_VERY_MUTED, "  %s", message.timestamp.c_str());

        if (message.isError) {
            ImGui::PushStyleColor(ImGuiCol_Text, COL_RED);
        }
        ImGui::TextWrapped("%s", message.content.c_str());
        if (message.isError) {
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Separator, COL_BORDER_SOFT);
        ImGui::Separator();
        ImGui::PopStyleColor();
        ImGui::Spacing();

        ImGui::PopID();
    }

    if (g_chatbot.isWaiting()) {
        ImGui::TextColored(COL_YELLOW, "Thinking...");
    }

    if (newMessage || g_chatbot.isWaiting()) {
        // Follow the latest reply so the user does not have to scroll manually.
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_FrameBg, COL_BG_ELEV);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 160.0f);
    const bool enterPressed = ImGui::InputText("##chatInput", g_chatInput,
                                               sizeof(g_chatInput),
                                               ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopStyleColor();

    ImGui::SameLine();
    const bool sendClicked = DrawPrimaryButton("Send", ImVec2(70.0f, 0.0f));

    ImGui::SameLine();
    if (DrawDangerButton("Clear", ImVec2(70.0f, 0.0f))) {
        g_chatbot.clearHistory();
    }

    if ((enterPressed || sendClicked) && g_chatInput[0] != '\0' && !g_chatbot.isWaiting()) {
        g_chatbot.sendMessage(g_chatInput, chain, g_fuelMgr, g_pendingMgr, g_authMgr);
        g_chatInput[0] = '\0';
    }

    if (enterPressed || sendClicked) {
        ImGui::SetKeyboardFocusHere(-1);
    }
}
