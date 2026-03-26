#pragma once

// Static vehicle data tables: manufacturer models, colors, transport modes,
// and stage names.  Kept separate from the GUI code so the data is reusable
// and the main_gui.cpp file stays focused on rendering.

struct ModelList { const char** models; int count; };

// Supply-chain stage display names (parallel with BlockStage enum order).
extern const char* k_stageNames[];
extern const int   k_stageNameCount;

// Per-manufacturer model arrays.
extern const ModelList k_modelsByMfr[];   // indexed same as k_manufacturers[]

// Vehicle body colors.
extern const char* k_colors[];
extern const int   k_colorCount;

// Transport / logistics modes.
extern const char* k_transportModes[];
extern const int   k_transportModeCount;
