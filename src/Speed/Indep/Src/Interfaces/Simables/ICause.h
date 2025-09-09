#ifndef INTERFACES_SIMABLES_ICAUSE_H
#define INTERFACES_SIMABLES_ICAUSE_H

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
    ICause(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ICause() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    // virtual void OnCausedCollision(const Sim::Collision::Info &cinfo, ISimable *from, ISimable *to);

    // virtual void OnCausedExplosion(struct IExplosion *, ISimable *);
};

#endif
