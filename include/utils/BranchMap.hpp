#pragma once

struct BranchList { const char** branches; int count; };

// Manufacturer names (parallel with k_branchesByMfr)
extern const char* k_manufacturers[];
extern const int   k_manufacturerCount;

// Branch / factory locations per manufacturer
extern const BranchList k_branchesByMfr[];

// Renders an interactive 3-D globe with branch pins.
void RenderBranchMap(float mapW, float mapH);
