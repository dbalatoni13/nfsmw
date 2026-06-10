#ifndef FEOBJECTCALLBACK_H_
#define FEOBJECTCALLBACK_H_

#include "FEObject.h"

// File: speed/indep/src/feng/FEObjectCallback.h
// total size: 0x4
// Decl: speed/indep/src/feng/FEObjectCallback.h:20
class FEObjectCallback {
  public:
    FEObjectCallback() {}
    virtual ~FEObjectCallback() {} // Decl: speed/indep/src/feng/FEObjectCallback.h:20
    virtual bool Callback(FEObject *obj) = 0;
};

#endif
