#ifndef INTERFACES_SIMABLES_ICAUSE_H
#define INTERFACES_SIMABLES_ICAUSE_H

#include "IExplosion.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

struct ISimable;

class ICause : public UTL::COM::IUnknown, public UTL::Collections::Instanceable<HCAUSE, ICause, 10> {
  public:
    DECL_INTERFACE(ICause);

    // virtual void OnCausedCollision(const COLLISION_INFO &cinfo, ISimable *from, ISimable *to);
    virtual void OnCausedExplosion(IExplosion *explosion, ISimable *to);
};

#endif
