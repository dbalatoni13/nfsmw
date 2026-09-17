#ifndef IEXPLODEABLE_H
#define IEXPLODEABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IExplosion;

class IExplodeable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IExplodeable);

    virtual bool OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) = 0;
};

#endif
