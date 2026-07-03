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

static const u32 DEFAULT_NUM_FEOBJECTS = 1024; // size: 0x4, Decl: speed/indep/src/feng/FEObjectSorter.h:30

#endif
