// Shares manufacturer/branch metadata with the GUI and declares the branch globe visualisation used on the dashboard as an advanced presentation feature.

#pragma once

// Simple array wrapper used by ImGui combo boxes and the branch map.
struct BranchList { const char** branches; int count; };

extern const char* k_manufacturers[];
extern const int   k_manufacturerCount;

extern const BranchList k_branchesByMfr[];

// Renders the interactive branch-distribution globe on the dashboard.
void RenderBranchMap(float mapW, float mapH);
