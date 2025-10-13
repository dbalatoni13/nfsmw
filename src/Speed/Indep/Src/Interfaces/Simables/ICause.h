#ifndef INTERFACES_SIMABLES_ICAUSE_H
#define INTERFACES_SIMABLES_ICAUSE_H

#include "Speed/Indep/Src/Sim/Collision.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

struct HCAUSE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HCAUSE__ *HCAUSE;

struct ISimable;

class ICause : public UTL::COM::IUnknown, public UTL::Collections::Instanceable<HCAUSE, ICause, 10> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ICause(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ICause() {}

    virtual void OnCausedCollision(const COLLISION_INFO &cinfo, ISimable *from, ISimable *to);
    virtual void OnCausedExplosion(IExplosion *explosion, ISimable *to);
};

#endif
