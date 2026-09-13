#ifndef __SUBTITLEINFO_HPP__
#define __SUBTITLEINFO_HPP__

#include "types.h"

// total size: 0x8
// Decl: 5
struct SubtitleInfo {
    uint16 startTime;  // offset 0x0, size 0x2, Decl: 6
    uint32 stringHash; // offset 0x4, size 0x4, Decl: 7
};

#endif
