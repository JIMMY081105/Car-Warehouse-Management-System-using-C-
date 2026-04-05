// Login screen and pending-approval screens.

#include "ui/GuiApp.hpp"

#include <cstdio>
#include <string>

#include "security/AccessControl.hpp"
#include "utils/TimeUtil.hpp"

void RenderLoginScreen(cw1::Blockchain& chain) {
    ImGuiIO& io = ImGui::GetIO();

    // Size the card from the current window so it still fits on smaller screens.
    const float screenW = io.DisplaySize.x;
    const float screenH = io.DisplaySize.y;
    const float cardW = (screenW < 500.0f)
                            ? (screenW - 40.0f)
                            : (screenW * 0.32f < 420.0f
                                   ? 420.0f
                                   : (screenW * 0.32f > 520.0f ? 520.0f : screenW * 0.32f));
    const float cardH = screenH * 0.7f < 480.0f
                            ? 480.0f
                            : (screenH * 0.7f > 600.0f ? 600.0f : screenH * 0.7f);
    const float titleGap = screenH * 0.06f < 40.0f ? 40.0f : screenH * 0.06f;
    const float totalH = titleGap + cardH;
    const float cardX = (screenW - cardW) * 0.5f;
    const float startY = (screenH - totalH) * 0.5f < 10.0f
                             ? 10.0f
                             : (screenH - totalH) * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, COL_BG_MAIN);
    ImGui::Begin("##loginbg", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleColor();

    if (g_fontHero != nullptr) {
        ImGui::PushFont(g_fontHero);
    }
    const char* bigTitle = "Car Warehouse Blockchain";
    const float titleW = ImGui::CalcTextSize(bigTitle).x;
    ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - titleW) * 0.5f, startY));
    ImGui::TextColored(COL_ACCENT, "%s", bigTitle);
    if (g_fontHero != nullptr) {
        ImGui::PopFont();
    }

    ImGui::SetCursorPos(ImVec2(cardX, startY + titleGap));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_PANEL);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##logincard", ImVec2(cardW, cardH), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::Spacing();
    ImGui::Spacing();
    if (g_fontTitle != nullptr) {
        ImGui::PushFont(g_fontTitle);
    }
    ImGui::TextColored(COL_TEXT, "  Login");
    if (g_fontTitle != nullptr) {
        ImGui::PopFont();
    }
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "  Please log in to continue.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(COL_MUTED, "  Username");
    ImGui::SetNextItemWidth(cardW - 40.0f);
    ImGui::SetCursorPosX(20.0f);
    ImGui::InputText("##login_user", g_loginUser, sizeof(g_loginUser));

    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "  Password");
    ImGui::SetNextItemWidth(cardW - 40.0f);
    ImGui::SetCursorPosX(20.0f);
    ImGui::InputText("##login_pass", g_loginPass, sizeof(g_loginPass),
                     ImGuiInputTextFlags_Password);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SetCursorPosX(20.0f);
    if (DrawPrimaryButton("  Login  ", ImVec2(cardW - 40.0f, 38.0f))) {
        if (g_authMgr.login(g_loginUser, g_loginPass)) {
            g_showLoginScreen = false;
            g_loginError.clear();
            g_loginPass[0] = '\0';

            WriteAuditEvent(
                chain,
                cw1::AuditAction::USER_LOGIN,
                "User " + std::string(g_loginUser)
                    + " (" + cw1::roleToString(g_authMgr.currentUser().role)
                    + ") logged in successfully");
            PushToast("Welcome, " + g_authMgr.currentUser().displayName + "!",
                      COL_GREEN_BR);
        } else {
            g_loginError = "Invalid username or password.";
            WriteAuditEvent(chain, cw1::AuditAction::USER_LOGIN,
                            "Login failed for username: " + std::string(g_loginUser));
        }
    }

    if (!g_loginError.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(COL_RED, "  %s", g_loginError.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "  Demo Accounts");
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 4.0f));
    if (ImGui::BeginTable("##demoaccts", 3,
                          ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg,
                          ImVec2(cardW - 40.0f, 0.0f))) {
        ImGui::TableSetupColumn("Username", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Password", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Role", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        const auto demoRow = [](const char* user, const char* pass, const char* role) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ACCENT_HO, "%s", user);
            ImGui::TableNextColumn();
            ImGui::Text("%s", pass);
            ImGui::TableNextColumn();
            ImGui::TextColored(COL_VERY_MUTED, "%s", role);
        };

        demoRow("admin01", "admin123", "Admin");
        demoRow("staff01", "staff123", "Warehouse Staff");
        demoRow("qc01", "qc123", "QC Inspector");
        demoRow("dealer01", "dealer123", "Dealer");
        demoRow("auditor01", "audit123", "Auditor");

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::End();
}

void RenderPendingApprovals(cw1::Blockchain& chain) {
    DrawSectionHeading("Pending Block Approvals");
    ImGui::SameLine();
    ImGui::TextColored(COL_MUTED, " (%d pending)", g_pendingMgr.pendingCount());
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const bool canApprove = cw1::AccessControl::canApprove(CurrentUserRole());
    const auto& allRequests = g_pendingMgr.getAllRequests();
    if (allRequests.empty()) {
        ImGui::TextColored(COL_MUTED, "No pending requests.");
        return;
    }

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    const int columnCount = canApprove ? 8 : 7;
    if (ImGui::BeginTable("##pending_requests", columnCount, tableFlags, ImVec2(-1, -1))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 45);
        ImGui::TableSetupColumn("Requester", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("VIN", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Stage", ImGuiTableColumnFlags_WidthStretch, 1.2f);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Signature", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        if (canApprove) {
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 200);
        }
        ImGui::TableHeadersRow();

        for (int reverseIndex = static_cast<int>(allRequests.size()) - 1;
             reverseIndex >= 0; --reverseIndex) {
            const cw1::PendingBlockRequest& request =
                allRequests[static_cast<std::size_t>(reverseIndex)];

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("#%d", request.requestId);

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_TEXT, "%s", request.requestedByDisplay.c_str());
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Username: %s", request.requestedBy.c_str());
                ImGui::EndTooltip();
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_ACCENT, "%s", request.payload.vin.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(request.requestedStage), "%s",
                               cw1::stageToString(request.requestedStage).c_str());

            ImGui::TableNextColumn();
            switch (request.status) {
                case cw1::RequestStatus::PENDING:
                    ImGui::TextColored(COL_YELLOW, "PENDING");
                    break;
                case cw1::RequestStatus::APPROVED:
                    ImGui::TextColored(COL_GREEN_BR, "APPROVED");
                    if (ImGui::IsItemHovered() && !request.approvedBy.empty()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Approved by: %s", request.approvedBy.c_str());
                        ImGui::Text("At: %s", request.approvedTimestamp.c_str());
                        ImGui::EndTooltip();
                    }
                    break;
                case cw1::RequestStatus::REJECTED:
                    ImGui::TextColored(COL_RED, "REJECTED");
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Rejected by: %s", request.approvedBy.c_str());
                        if (!request.rejectReason.empty()) {
                            ImGui::Text("Reason: %s", request.rejectReason.c_str());
                        }
                        ImGui::EndTooltip();
                    }
                    break;
            }

            const bool signatureValid = VerifyRequestSignature(request);
            ImGui::TableNextColumn();
            if (!request.creatorSignature.empty()) {
                ImGui::TextColored(signatureValid ? COL_GREEN_BR : COL_RED,
                                   signatureValid ? "Valid" : "INVALID");
            } else {
                ImGui::TextColored(COL_MUTED, "None");
            }

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", request.requestTimestamp.c_str());

            if (!canApprove) {
                continue;
            }

            ImGui::TableNextColumn();
            if (request.status == cw1::RequestStatus::PENDING) {
                const std::string approveId = "Approve##ap" + std::to_string(request.requestId);
                if (DrawPrimaryButton(approveId.c_str(), ImVec2(80, 26))) {
                    // Approval both updates the request and commits the real block.
                    const std::string approver = g_authMgr.currentUser().username;
                    if (g_pendingMgr.approveRequest(request.requestId, approver)) {
                        chain.addBlockWithMetadata(request.payload, request.requestedBy,
                                                   approver, request.requestId,
                                                   request.creatorSignature, signatureValid);
                        WriteAuditEvent(
                            chain,
                            cw1::AuditAction::APPROVAL_ACTION,
                            "User " + approver
                                + " approved request #" + std::to_string(request.requestId)
                                + " and committed block #"
                                + std::to_string(chain.totalBlocks() - 1)
                                + " for VIN " + request.payload.vin);
                        PushToast("Request #" + std::to_string(request.requestId)
                                      + " approved -> Block #"
                                      + std::to_string(chain.totalBlocks() - 1),
                                  COL_GREEN_BR);
                    }
                }

                ImGui::SameLine();
                const std::string rejectId = "Reject##rj" + std::to_string(request.requestId);
                if (DrawDangerButton(rejectId.c_str(), ImVec2(80, 26))) {
                    ImGui::OpenPopup(("RejectPopup##" + std::to_string(request.requestId)).c_str());
                }

                if (ImGui::BeginPopup(("RejectPopup##" + std::to_string(request.requestId)).c_str())) {
                    ImGui::Text("Reason for rejection:");
                    ImGui::SetNextItemWidth(200);
                    ImGui::InputText("##rejreason", g_rejectReasonBuf, sizeof(g_rejectReasonBuf));
                    if (DrawDangerButton("Confirm Reject", ImVec2(140, 26))) {
                        const std::string approver = g_authMgr.currentUser().username;
                        if (g_pendingMgr.rejectRequest(request.requestId, approver,
                                                       g_rejectReasonBuf)) {
                            WriteAuditEvent(
                                chain,
                                cw1::AuditAction::APPROVAL_ACTION,
                                "User " + approver
                                    + " rejected request #" + std::to_string(request.requestId)
                                    + " for VIN " + request.payload.vin
                                    + " (reason: " + g_rejectReasonBuf + ")");
                            PushToast("Request #" + std::to_string(request.requestId)
                                          + " rejected.",
                                      COL_RED);
                            g_rejectReasonBuf[0] = '\0';
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            } else {
                ImGui::TextColored(COL_VERY_MUTED, "--");
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}
