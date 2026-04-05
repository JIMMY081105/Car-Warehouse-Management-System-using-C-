// Static vehicle data used by forms and demo records.

#pragma once

// Wrapper for manufacturer-specific model arrays used by the add-block form.
struct ModelList { const char** models; int count; };

extern const char* k_stageNames[];
extern const int   k_stageNameCount;

extern const ModelList k_modelsByMfr[];

extern const char* k_colors[];
extern const int   k_colorCount;

extern const char* k_transportModes[];
extern const int   k_transportModeCount;
