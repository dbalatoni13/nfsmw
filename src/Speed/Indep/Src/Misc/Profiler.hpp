#ifndef PROFILER_HPP
#define PROFILER_HPP

#include "types.h"

#define NO_PROFILER                                                   // :73
#define PROFILE_TOP_LEVEL 1                                           // :89
#define PROFILE_RENDERING 2                                           // :90
#define PROFILE_PHYSICS 4                                             // :91
#define PROFILE_SIM 8                                                 // :92
#define PROFILE_SOUND 16                                              // :93
#define PROFILE_MISC 32                                               // :94
#define PROFILE_RESOURCE_LOADING 64                                   // :95
#define PROFILE_VINYLS 128                                            // :96
#define PROFILE_FE 256                                                // :97
#define profile_node(name, categories) profile_node(NULL, categories) // :243

// total size: 0xC
// Decl: 102
struct ProfileDataEntry {
    const char *SectionName; // offset 0x0, size 0x4, Decl: 103
    int Depth;               // offset 0x4, size 0x4, Decl: 104
    uint32 Ticks;            // offset 0x8, size 0x4, Decl: 105
};

// total size: 0x1
// Decl: 114
class ProfileManager {
  public:
    void Init(int buffer_size, int max_sections) {} // Decl: 120

    void Close() {} // Decl: 121

    void BeginProfiling(int categories) {} // Decl: 123

    void EndProfiling(bool profile_section_results) {} // Decl: 124

    struct ProfileDataEntry *BeginSection(const char *section_name, int categories) {} // Decl: 126

    void EndSection() {} // Decl: 127

    float GetCapturedTime() {} // Decl: 129

    void AccumulateResults() {} // Decl: 131

    void ClearAccumulatedResults() {} // Decl: 132

    void PrintResults() {} // Decl: 134
};

extern ProfileManager TheProfiler;

class ProfileNode {
  public:
    ProfileNode() {}                                        // Decl: 251
    ProfileNode(const char *section_name, int categories) { // Decl: 252
    }

    void Begin(const char *section_name, int categories) {} // Decl: 254
    void End() {}                                           // Decl: 255
    float GetTime() {}                                      // Decl: 256
};

#endif
