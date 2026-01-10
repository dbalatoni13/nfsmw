#ifndef EAXSOUND_EAXSOUNDTYPES_H
#define EAXSOUND_EAXSOUNDTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

namespace Speech {

// total size: 0x8
struct copPair {
    inline bool operator<(const struct copPair &from) const {}

    HSIMABLE hsimable;  // offset 0x0, size 0x4
    struct EAXCop *cop; // offset 0x4, size 0x4
};

struct _type_copMap {
    const char *name() {
        return "copMap";
    }
};

class copMap : public UTL::Std::vector<copPair, _type_copMap> {
  public:
    copMap(int size) {}
};

struct _type_copList {
    const char *name() {
        return "copList";
    }
};

class copList : public UTL::Std::vector<EAXCop *, _type_copList>, public AudioMemBase {};

struct _type_voiceIDs {
    const char *name() {
        return "voiceIDs";
    }
};

struct voiceIDs : public UTL::Std::vector<int, _type_voiceIDs> {};

// total size: 0x70
struct VoiceUsage {
    VoiceUsage() {}

    voiceIDs voices;          // offset 0x0, size 0x10
    voiceIDs cs_Rhino;        // offset 0x10, size 0x10
    voiceIDs cs_SuperPursuit; // offset 0x20, size 0x10
    voiceIDs cs_City;         // offset 0x30, size 0x10
    voiceIDs cs_Coastal;      // offset 0x40, size 0x10
    voiceIDs cs_Rosewood;     // offset 0x50, size 0x10
    voiceIDs cs_Alpine;       // offset 0x60, size 0x10
};

}; // namespace Speech

#endif
