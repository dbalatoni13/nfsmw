#ifndef EVENTS_EMOMENTSTRM_H
#define EVENTS_EMOMENTSTRM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x44
class EMomentStrm : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        const char * fAttribStrm; // offset: 0x4, size 0x4
        unsigned int fAttribKey; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EMomentStrm(const char * pAttribStrm, unsigned int pAttribKey, UMath::Vector4 pPosition, UMath::Vector4 pVector, UMath::Vector4 pVelocity, unsigned int phSimable);

    override virtual ~EMomentStrm();

    override virtual const char *GetEventName() {
        return "EMomentStrm";
    }

  private:
    const char * fAttribStrm; // offset: 0x8, size 0x4
    unsigned int fAttribKey; // offset: 0xc, size 0x4

    UMath::Vector4 fPosition; // offset: 0xc, size 0x10
    UMath::Vector4 fVector; // offset: 0x1c, size 0x10
    UMath::Vector4 fVelocity; // offset: 0x2c, size 0x10
    unsigned int fhSimable; // offset: 0x3c, size 0x4
};

void EMomentStrm_MakeEvent_Callback(const void *staticData);

#endif
