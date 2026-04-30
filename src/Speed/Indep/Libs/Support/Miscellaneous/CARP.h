#ifndef SUPPORT_MISC_CARP_H
#define SUPPORT_MISC_CARP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"

namespace CARP {

struct EventStaticData;
struct EventList;

// total size: 0x40
struct Trigger {
    bool ValidateMatrix() const;
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;

    UMath::Vector4 fMatRow0Width;  // offset 0x0, size 0x10
    unsigned int fType : 4;        // offset 0x10, size 0x4
    unsigned int fShape : 4;       // offset 0x10, size 0x4
    unsigned int fFlags : 24;      // offset 0x10, size 0x4
    float fHeight;                 // offset 0x14, size 0x4
    CARP::EventList *fEvents;      // offset 0x18, size 0x4
    unsigned short fIterStamp;     // offset 0x1C, size 0x2
    unsigned short fFingerprint;   // offset 0x1E, size 0x2
    UMath::Vector4 fMatRow2Length; // offset 0x20, size 0x10
    UMath::Vector4 fPosRadius;     // offset 0x30, size 0x10
};

unsigned int ResolveTagReferences(const UGroup *g, unsigned int deltaAddress);

}; // namespace CARP

#endif
