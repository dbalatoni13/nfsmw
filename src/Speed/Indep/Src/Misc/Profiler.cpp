#include "Speed/Indep/Src/Misc/Profiler.hpp"

ProfileManager TheProfiler; // size: 0x1, address: 0xFFFFFFFF, Decl: 16

bool EnableProfilingAlways; // size: 0x4, address: 0xFFFFFFFF, Decl: 667

bool ClearAccumulatedProfile; // size: 0x4, address: 0xFFFFFFFF, Decl: 668

bool PrintOneShotProfile; // size: 0x4, address: 0xFFFFFFFF, Decl: 670

bool PrintAccumulatedProfile; // size: 0x4, address: 0xFFFFFFFF, Decl: 671

int PrintAccumulatedProfileOncePaused; // size: 0x4, address: 0xFFFFFFFF, Decl: 673

int PrintAccumulatedProfileOnFrame; // size: 0x4, address: 0xFFFFFFFF, Decl: 677

// Decl: 689
void TweakerServiceProfiler(bool in_game, bool is_paused) {}
