// Writes a simple PDF report from chain, audit, and fuel data.

#include "ui/GuiApp.hpp"

#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

namespace {

std::string PdfEscape(const std::string& text) {
    std::string escaped;
    escaped.reserve(text.size() + 16);
    for (char ch : text) {
        if (ch == '(' || ch == ')' || ch == '\\') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

}  // namespace

bool ExportBlockchainPDF(cw1::Blockchain& chain,
                         const cw1::FuelPriceManager& fuelMgr,
                         const std::string& outputPath) {
    FILE* file = std::fopen(outputPath.c_str(), "wb");
    if (file == nullptr) {
        return false;
    }

    std::vector<std::string> lines;
    lines.push_back("CAR WAREHOUSE BLOCKCHAIN SYSTEM - REPORT");
    lines.push_back("========================================");
    {
        std::time_t now = std::time(nullptr);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        lines.push_back(std::string("Generated: ") + buffer);
    }
    lines.push_back("");
    lines.push_back("SUMMARY");
    lines.push_back("-------");
    {
        char buffer[128];
        std::snprintf(buffer, sizeof(buffer), "Total blocks: %zu", chain.totalBlocks());
        lines.push_back(buffer);
        std::snprintf(buffer, sizeof(buffer), "Total vehicles: %zu", chain.getAllVins().size());
        lines.push_back(buffer);
        const cw1::ValidationResult verify = chain.verifyIntegrity();
        lines.push_back(std::string("Chain integrity: ") + (verify.ok ? "VERIFIED" : "FAILED"));
    }
    lines.push_back("");
    lines.push_back("BLOCKCHAIN BLOCKS");
    lines.push_back("-----------------");

    const auto& blocks = chain.getChain();
    for (const auto& block : blocks) {
        const auto& record = block.getRecord();
        if (chain.isDeleted(block.getIndex())) {
            char buffer[128];
            std::snprintf(buffer, sizeof(buffer), "Block #%zu  [DELETED]", block.getIndex());
            lines.push_back(buffer);
            continue;
        }

        char buffer[512];
        std::snprintf(buffer, sizeof(buffer),
                      "Block #%zu | VIN: %s | %s %s | %s | Stage: %s",
                      block.getIndex(),
                      record.vin.c_str(),
                      record.manufacturer.c_str(),
                      record.model.c_str(),
                      record.color.c_str(),
                      cw1::stageToString(record.stage).c_str());
        lines.push_back(buffer);
        std::snprintf(buffer, sizeof(buffer), "  Hash:  %s", block.getCurrentHash().c_str());
        lines.push_back(buffer);
        std::snprintf(buffer, sizeof(buffer), "  Prev:  %s", block.getPreviousHash().c_str());
        lines.push_back(buffer);
        std::snprintf(buffer, sizeof(buffer), "  SHA3:  %s", block.getSha3Hash().c_str());
        lines.push_back(buffer);
        std::snprintf(buffer, sizeof(buffer),
                      "  Nonce: %llu | Time: %s | By: %s",
                      static_cast<unsigned long long>(block.getNonce()),
                      block.getTimestamp().c_str(),
                      block.getCreatedBy().empty() ? "system" : block.getCreatedBy().c_str());
        lines.push_back(buffer);
    }

    lines.push_back("");
    lines.push_back("RECENT AUDIT LOG (last 30)");
    lines.push_back("--------------------------");
    {
        const auto entries = chain.getAuditLog().getRecentEntries(30);
        if (entries.empty()) {
            lines.push_back("No audit entries.");
        } else {
            for (const auto* entry : entries) {
                char buffer[512];
                std::snprintf(buffer, sizeof(buffer), "[%s] %s: %s",
                              entry->timestamp.c_str(),
                              cw1::actionToString(entry->action).c_str(),
                              entry->details.c_str());
                lines.push_back(buffer);
            }
        }
    }

    lines.push_back("");
    const auto& fuelHistory = fuelMgr.history();
    if (!fuelHistory.empty()) {
        lines.push_back("FUEL PRICE DATA");
        lines.push_back("---------------");
        lines.push_back("Date         | RON95  | RON97  | Diesel(Pen) | Diesel(East)");
        for (const auto& week : fuelHistory) {
            char buffer[128];
            std::snprintf(buffer, sizeof(buffer), "%-12s | RM%.2f | RM%.2f | RM%.2f      | RM%.2f",
                          week.effectiveDate.c_str(),
                          week.ron95,
                          week.ron97,
                          week.dieselPeninsular,
                          week.dieselEast);
            lines.push_back(buffer);
        }
    }

    // Keep pagination simple by treating the report as fixed-width text.
    constexpr int kLinesPerPage = 62;
    std::vector<std::vector<std::string>> pages;
    for (std::size_t i = 0; i < lines.size(); i += kLinesPerPage) {
        const std::size_t end = (i + kLinesPerPage < lines.size())
                                    ? i + kLinesPerPage
                                    : lines.size();
        pages.emplace_back(lines.begin() + static_cast<std::ptrdiff_t>(i),
                           lines.begin() + static_cast<std::ptrdiff_t>(end));
    }
    if (pages.empty()) {
        pages.push_back({});
    }

    std::vector<std::string> streams;
    for (std::size_t pageIndex = 0; pageIndex < pages.size(); ++pageIndex) {
        std::string stream = "BT\n/F1 9 Tf\n";
        char temp[256];
        int y = 792;
        for (std::size_t lineIndex = 0; lineIndex < pages[pageIndex].size(); ++lineIndex) {
            if (pageIndex == 0 && lineIndex == 0) {
                std::snprintf(temp, sizeof(temp), "/F1 13 Tf\n1 0 0 1 50 %d Tm\n", y);
                stream += temp;
                stream += "(" + PdfEscape(pages[pageIndex][lineIndex]) + ") Tj\n";
                stream += "/F1 9 Tf\n";
            } else {
                std::snprintf(temp, sizeof(temp), "1 0 0 1 50 %d Tm\n", y);
                stream += temp;
                stream += "(" + PdfEscape(pages[pageIndex][lineIndex]) + ") Tj\n";
            }
            y -= 12;
        }
        std::snprintf(temp, sizeof(temp), "1 0 0 1 50 35 Tm\n(Page %zu of %zu) Tj\n",
                      pageIndex + 1, pages.size());
        stream += temp;
        stream += "ET\n";
        streams.push_back(std::move(stream));
    }

    std::vector<long> offsets;
    std::fprintf(file, "%%PDF-1.4\n");

    offsets.push_back(std::ftell(file));
    std::fprintf(file, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");

    offsets.push_back(std::ftell(file));
    std::fprintf(file, "2 0 obj\n<< /Type /Pages /Kids [");
    for (std::size_t pageIndex = 0; pageIndex < pages.size(); ++pageIndex) {
        if (pageIndex > 0) {
            std::fprintf(file, " ");
        }
        std::fprintf(file, "%d 0 R", 4 + static_cast<int>(pageIndex) * 2);
    }
    std::fprintf(file, "] /Count %zu >>\nendobj\n", pages.size());

    offsets.push_back(std::ftell(file));
    std::fprintf(file, "3 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>\nendobj\n");

    for (std::size_t pageIndex = 0; pageIndex < pages.size(); ++pageIndex) {
        const int pageObject = 4 + static_cast<int>(pageIndex) * 2;
        const int streamObject = pageObject + 1;

        offsets.push_back(std::ftell(file));
        std::fprintf(file,
                     "%d 0 obj\n<< /Type /Page /Parent 2 0 R "
                     "/MediaBox [0 0 595 842] "
                     "/Contents %d 0 R "
                     "/Resources << /Font << /F1 3 0 R >> >> >>\nendobj\n",
                     pageObject, streamObject);

        offsets.push_back(std::ftell(file));
        std::fprintf(file, "%d 0 obj\n<< /Length %zu >>\nstream\n",
                     streamObject, streams[pageIndex].size());
        std::fwrite(streams[pageIndex].data(), 1, streams[pageIndex].size(), file);
        std::fprintf(file, "endstream\nendobj\n");
    }

    const long xrefPos = std::ftell(file);
    const int totalObjects = 3 + static_cast<int>(pages.size()) * 2;
    std::fprintf(file, "xref\n0 %d\n", totalObjects + 1);
    std::fprintf(file, "0000000000 65535 f \r\n");
    for (long offset : offsets) {
        std::fprintf(file, "%010ld 00000 n \r\n", offset);
    }

    std::fprintf(file, "trailer\n<< /Size %d /Root 1 0 R >>\n", totalObjects + 1);
    std::fprintf(file, "startxref\n%ld\n%%%%EOF\n", xrefPos);

    std::fclose(file);
    return true;
}
