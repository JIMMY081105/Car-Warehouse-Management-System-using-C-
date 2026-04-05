// Branch datasets and map drawing entry point.

#pragma once

// Simple array wrapper used by ImGui combo boxes and the branch map.
struct BranchList { const char** branches; int count; };

extern const char* k_manufacturers[];
extern const int   k_manufacturerCount;

extern const BranchList k_branchesByMfr[];

// Renders the interactive branch-distribution globe on the dashboard.
void RenderBranchMap(float mapW, float mapH);
