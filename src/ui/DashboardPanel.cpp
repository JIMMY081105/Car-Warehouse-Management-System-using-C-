// Dashboard page - shows stats, fuel chart, and recent blocks.

#include "ui/GuiApp.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "utils/BranchMap.hpp"

namespace {

float RenderDashboardStats(cw1::Blockchain& chain, float statsW, float cardH) {
    char totalBlocks[32];
    char totalVehicles[32];
    char totalAuditEvents[32];
    std::snprintf(totalBlocks, sizeof(totalBlocks), "%zu", chain.totalBlocks());
    std::snprintf(totalVehicles, sizeof(totalVehicles), "%zu", chain.getAllVins().size());
    std::snprintf(totalAuditEvents, sizeof(totalAuditEvents), "%zu", chain.getAuditLog().size());

    const float startY = ImGui::GetCursorPosY();
    ImGui::BeginGroup();
    {
        const float halfWidth = (statsW - 8.0f) / 2.0f;
        DrawStatCard(totalBlocks, "Total Blocks", COL_ACCENT, ImVec2(halfWidth, cardH));
        ImGui::SameLine(0, 8);
        DrawStatCard(totalVehicles, "Unique Vehicles", COL_GREEN_BR, ImVec2(halfWidth, cardH));

        ImGui::Spacing();

        DrawStatCard(g_verifyDone && g_lastVerify.ok ? "VERIFIED" : (g_verifyDone ? "TAMPERED" : "N/A"),
                     "Integrity",
                     g_verifyDone && g_lastVerify.ok ? COL_GREEN_BR : COL_RED,
                     ImVec2(halfWidth, cardH));
        ImGui::SameLine(0, 8);
        DrawStatCard(totalAuditEvents, "Audit Events", COL_PURPLE, ImVec2(halfWidth, cardH));
    }
    ImGui::EndGroup();
    return ImGui::GetCursorPosY() - startY;
}

void RenderDashboardMap(float mapWidth, float mapHeight) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(8, 15, 30, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##mapChild", ImVec2(mapWidth, mapHeight), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    RenderBranchMap(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void RenderFuelChart(const std::vector<cw1::FuelWeeklyRecord>& history,
                     float chartW, float chartH) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##fuelChart", ImVec2(chartW, chartH), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const float graphWidth = ImGui::GetContentRegionAvail().x;
    const float graphHeight = ImGui::GetContentRegionAvail().y - 16.0f;

    float minY = 1e9f;
    float maxY = -1e9f;
    for (const auto& week : history) {
        const float values[] = {
            static_cast<float>(week.ron95),
            static_cast<float>(week.ron97),
            static_cast<float>(week.dieselPeninsular),
            static_cast<float>(week.dieselEast),
        };
        for (float value : values) {
            if (value < minY) {
                minY = value;
            }
            if (value > maxY) {
                maxY = value;
            }
        }
    }

    float padding = (maxY - minY) * 0.15f;
    if (padding < 0.05f) {
        padding = 0.05f;
    }
    minY -= padding;
    maxY += padding;
    if (minY < 0.0f) {
        minY = 0.0f;
    }

    const ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    constexpr float kLabelWidth = 44.0f;

    drawList->AddRectFilled(ImVec2(origin.x + kLabelWidth, origin.y),
                            ImVec2(origin.x + graphWidth, origin.y + graphHeight),
                            IM_COL32(13, 17, 23, 255), 4.0f);

    for (int grid = 0; grid <= 3; ++grid) {
        const float fraction = static_cast<float>(grid) / 3.0f;
        const float y = origin.y + graphHeight - fraction * graphHeight;
        drawList->AddLine(ImVec2(origin.x + kLabelWidth, y),
                          ImVec2(origin.x + graphWidth, y),
                          IM_COL32(48, 54, 61, 100), 1.0f);
        char label[16];
        std::snprintf(label, sizeof(label), "%.2f", minY + fraction * (maxY - minY));
        drawList->AddText(ImVec2(origin.x, y - 7), IM_COL32(139, 148, 158, 180), label);
    }

    const std::size_t count = history.size();
    if (count >= 2) {
        const ImU32 lineColors[] = {
            IM_COL32(46, 160, 67, 255),
            IM_COL32(31, 111, 235, 255),
            IM_COL32(210, 153, 34, 255),
            IM_COL32(219, 109, 40, 255),
        };

        const auto mapY = [&](double value) {
            return origin.y + graphHeight
                 - (static_cast<float>(value) - minY) / (maxY - minY) * graphHeight;
        };
        const auto mapX = [&](std::size_t index) {
            return origin.x + kLabelWidth + 6.0f
                 + (static_cast<float>(index) / static_cast<float>(count - 1))
                 * (graphWidth - kLabelWidth - 12.0f);
        };

        struct SeriesInfo {
            ImU32 color;
            double cw1::FuelWeeklyRecord::*field;
        };

        const SeriesInfo series[] = {
            {lineColors[0], &cw1::FuelWeeklyRecord::ron95},
            {lineColors[1], &cw1::FuelWeeklyRecord::ron97},
            {lineColors[2], &cw1::FuelWeeklyRecord::dieselPeninsular},
            {lineColors[3], &cw1::FuelWeeklyRecord::dieselEast},
        };

        for (const auto& info : series) {
            for (std::size_t i = 1; i < count; ++i) {
                drawList->AddLine(ImVec2(mapX(i - 1), mapY(history[i - 1].*info.field)),
                                  ImVec2(mapX(i), mapY(history[i].*info.field)),
                                  info.color, 2.0f);
            }
            for (std::size_t i = 0; i < count; ++i) {
                drawList->AddCircleFilled(ImVec2(mapX(i), mapY(history[i].*info.field)),
                                          2.5f, info.color);
            }
        }

        const auto drawDateLabel = [&](std::size_t index) {
            const char* fullDate = history[index].effectiveDate.c_str();
            const char* shortDate = (std::strlen(fullDate) >= 10) ? fullDate + 5 : fullDate;
            drawList->AddText(ImVec2(mapX(index) - 12, origin.y + graphHeight + 2),
                              IM_COL32(139, 148, 158, 180), shortDate);
        };
        drawDateLabel(0);
        if (count > 2) {
            drawDateLabel(count / 2);
        }
        drawDateLabel(count - 1);

        const ImVec2 mouse = ImGui::GetMousePos();
        if (mouse.x >= mapX(0) && mouse.x <= mapX(count - 1)
            && mouse.y >= origin.y && mouse.y <= origin.y + graphHeight) {
            std::size_t nearest = 0;
            float bestDistance = 1e9f;
            for (std::size_t i = 0; i < count; ++i) {
                const float distance = std::fabs(mouse.x - mapX(i));
                if (distance < bestDistance) {
                    bestDistance = distance;
                    nearest = i;
                }
            }

            const float x = mapX(nearest);
            drawList->AddLine(ImVec2(x, origin.y), ImVec2(x, origin.y + graphHeight),
                              IM_COL32(200, 200, 200, 80), 1.0f);
            for (const auto& info : series) {
                drawList->AddCircleFilled(ImVec2(x, mapY(history[nearest].*info.field)),
                                          5.0f, info.color);
            }

            ImGui::BeginTooltip();
            ImGui::TextColored(COL_ACCENT, "%s", history[nearest].effectiveDate.c_str());
            ImGui::Separator();
            const char* names[] = {"RON95", "RON97", "Diesel(P)", "Diesel(E)"};
            for (int seriesIndex = 0; seriesIndex < 4; ++seriesIndex) {
                const double value = history[nearest].*(series[seriesIndex].field);
                ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(series[seriesIndex].color),
                                   "%-12s RM %.2f", names[seriesIndex], value);
            }
            ImGui::EndTooltip();
        }
    }

    {
        const float legendX = origin.x + graphWidth - 120.0f;
        const float legendY = origin.y + 4.0f;
        const char* labels[] = {"RON95", "RON97", "Diesel(P)", "Diesel(E)"};
        const ImU32 colors[] = {
            IM_COL32(46, 160, 67, 255),
            IM_COL32(31, 111, 235, 255),
            IM_COL32(210, 153, 34, 255),
            IM_COL32(219, 109, 40, 255),
        };
        for (int i = 0; i < 4; ++i) {
            drawList->AddRectFilled(ImVec2(legendX, legendY + i * 14.0f),
                                    ImVec2(legendX + 8, legendY + i * 14.0f + 8),
                                    colors[i], 2.0f);
            drawList->AddText(ImVec2(legendX + 12, legendY + i * 14.0f - 1),
                              IM_COL32(200, 207, 213, 200), labels[i]);
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void RenderFuelPriceTable(float width, float chartH) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, COL_BG_ELEV);
    ImGui::PushStyleColor(ImGuiCol_Border, COL_BORDER_SOFT);
    ImGui::BeginChild("##fuelPrices", ImVec2(width, chartH), true,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    constexpr double kDistance = 10.0;
    constexpr double kEfficiency = 3.0;

    struct FuelRow {
        const char* name;
        ImVec4 color;
        cw1::FuelSeries series;
    };

    const FuelRow rows[] = {
        {"RON95", COL_GREEN_BR, cw1::FuelSeries::RON95},
        {"RON97", COL_ACCENT, cw1::FuelSeries::RON97},
        {"Diesel (Peninsular)", COL_YELLOW, cw1::FuelSeries::DIESEL_PENINSULAR},
        {"Diesel (East Malaysia)", COL_ORANGE, cw1::FuelSeries::DIESEL_EAST},
    };

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("##fuelTbl", 4, tableFlags)) {
        ImGui::TableSetupColumn("Fuel", ImGuiTableColumnFlags_WidthStretch, 3.0f);
        ImGui::TableSetupColumn("Price/L", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Trend", ImGuiTableColumnFlags_WidthStretch, 1.8f);
        ImGui::TableSetupColumn("RM/10 miles", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableHeadersRow();

        for (const auto& row : rows) {
            const cw1::FuelForecast forecast = g_fuelMgr.forecast(row.series);
            const double cost = cw1::FuelPriceManager::EstimateDeliveryCost(
                forecast.currentPrice, kDistance, kEfficiency);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(row.color, "%s", row.name);

            ImGui::TableNextColumn();
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "RM %.2f", forecast.currentPrice);
            ImGui::Text("%s", buffer);

            ImGui::TableNextColumn();
            if (forecast.stable) {
                ImGui::TextColored(COL_MUTED, "--");
            } else {
                const ImVec4 trendColor = forecast.increasing ? COL_RED : COL_GREEN_BR;
                const ImVec2 pos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                constexpr float kArrowSize = 8.0f;
                const float centerY = pos.y + ImGui::GetTextLineHeight() * 0.5f;
                const ImU32 arrowColor = ImGui::ColorConvertFloat4ToU32(trendColor);
                if (forecast.increasing) {
                    drawList->AddTriangleFilled(
                        ImVec2(pos.x + kArrowSize * 0.5f, centerY - kArrowSize * 0.5f),
                        ImVec2(pos.x, centerY + kArrowSize * 0.5f),
                        ImVec2(pos.x + kArrowSize, centerY + kArrowSize * 0.5f),
                        arrowColor);
                } else {
                    drawList->AddTriangleFilled(
                        ImVec2(pos.x, centerY - kArrowSize * 0.5f),
                        ImVec2(pos.x + kArrowSize, centerY - kArrowSize * 0.5f),
                        ImVec2(pos.x + kArrowSize * 0.5f, centerY + kArrowSize * 0.5f),
                        arrowColor);
                }
                ImGui::Dummy(ImVec2(kArrowSize + 4.0f, 0.0f));
                ImGui::SameLine();
                std::snprintf(buffer, sizeof(buffer), "%+.2f", forecast.delta);
                ImGui::TextColored(trendColor, "%s", buffer);
            }

            ImGui::TableNextColumn();
            std::snprintf(buffer, sizeof(buffer), "RM %.2f", cost);
            ImGui::TextColored(COL_TEXT, "%s", buffer);
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextColored(COL_VERY_MUTED, "Truck: %.0f mi/L | Weekly update (Thu)", kEfficiency);
    ImGui::TextColored(COL_VERY_MUTED, "%s", g_fuelMgr.statusText().c_str());
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 110.0f);
    if (ImGui::SmallButton("Refresh")) {
        g_fuelMgr.loadRecentHistory(52);
        PushToast("Fuel data refreshed", COL_GREEN_BR);
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void RenderRecentBlocksTable(const std::vector<cw1::Block>& blocks,
                             const cw1::Blockchain& chain) {
    ImGui::TextColored(COL_MUTED, "RECENT BLOCKS");
    ImGui::Separator();
    ImGui::Spacing();

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 5.0f));
    if (ImGui::BeginTable("##recent", 6, tableFlags)) {
        ImGui::TableSetupColumn("Block #", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupColumn("Hash", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("VIN", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Manufacturer", ImGuiTableColumnFlags_WidthStretch, 2.0f);
        ImGui::TableSetupColumn("Stage", ImGuiTableColumnFlags_WidthStretch, 1.5f);
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableHeadersRow();

        const std::size_t count = blocks.size();
        for (std::size_t reverseIndex = 0; reverseIndex < count; ++reverseIndex) {
            const std::size_t index = count - 1 - reverseIndex;
            const cw1::Block& block = blocks[index];
            const cw1::CarRecord& record = block.getRecord();

            if (chain.isDeleted(block.getIndex())) {
                continue;
            }

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", block.getIndex());

            ImGui::TableNextColumn();
            DrawHashWithTooltip(block);

            ImGui::TableNextColumn();
            char selectableId[80];
            std::snprintf(selectableId, sizeof(selectableId), "%s##rv%zu",
                          record.vin.c_str(), index);
            ImGui::PushStyleColor(ImGuiCol_Text, COL_ACCENT);
            if (ImGui::Selectable(selectableId, false, ImGuiSelectableFlags_SpanAllColumns)) {
                g_selectedVin = record.vin;
                g_view = View::CAR_DETAIL;
            }
            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text("%s", record.manufacturer.c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(StageColor(record.stage), "%s",
                               cw1::stageToString(record.stage).c_str());

            ImGui::TableNextColumn();
            ImGui::TextColored(COL_MUTED, "%s", block.getTimestamp().c_str());
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

}  // namespace

void RenderDashboard(cw1::Blockchain& chain) {
    const auto& blocks = chain.getChain();
    DrawSectionHeading("Dashboard");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const float totalWidth = ImGui::GetContentRegionAvail().x;
    float mapWidth = totalWidth * 0.42f;
    if (mapWidth < 280.0f) {
        mapWidth = 280.0f;
    }
    const float statsWidth = totalWidth - mapWidth - 12.0f;
    constexpr float kMapHeight = 260.0f;
    const float cardHeight = (kMapHeight - 8.0f) / 2.0f;

    const float leftGroupHeight = RenderDashboardStats(chain, statsWidth, cardHeight);
    ImGui::SameLine(0, 12);
    ImGui::BeginGroup();
    RenderDashboardMap(mapWidth, leftGroupHeight);
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextColored(COL_MUTED, "FUEL PRICE INTELLIGENCE");
    ImGui::SameLine(totalWidth - mapWidth);
    ImGui::TextColored(COL_MUTED, "FUEL PRICES & DELIVERY COST / PER 10 MILES");
    ImGui::Separator();
    ImGui::Spacing();

    if (g_fuelInitialized && !g_fuelMgr.history().empty()) {
        const auto& history = g_fuelMgr.history();
        const float fuelTotalWidth = ImGui::GetContentRegionAvail().x;
        const float chartWidth = fuelTotalWidth - mapWidth - 12.0f;
        constexpr float kChartHeight = 260.0f;

        ImGui::BeginGroup();
        RenderFuelChart(history, chartWidth, kChartHeight);
        ImGui::EndGroup();

        ImGui::SameLine(0, 12);
        ImGui::BeginGroup();
        RenderFuelPriceTable(mapWidth, kChartHeight);
        ImGui::EndGroup();
    }

    ImGui::Spacing();
    ImGui::Spacing();
    RenderRecentBlocksTable(blocks, chain);
}
