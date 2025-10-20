#ifndef INTTERFACES_ISERVICEABLE_H
#define INTTERFACES_ISERVICEABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

struct HSIMSERVICE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HSIMSERVICE__ *HSIMSERVICE;

namespace Sim {

struct Packet;

struct IServiceable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IServiceable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  public:
    virtual bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt);
};

void SetStream(UMath::Vector3 &location, bool blocking);

}; // namespace Sim

#endif
