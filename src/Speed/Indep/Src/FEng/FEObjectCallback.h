#ifndef FEOBJECTCALLBACK_H_
#define FEOBJECTCALLBACK_H_

// c3: el objetivo importa _vt.16FEObjectCallback y _._16FEObjectCallback en
// zFEng y zFe2; solo zFe los DEFINE, y como GLOBAL (no weak). Eso es la firma
// de #pragma interface + #pragma implementation en zFe.
#pragma interface

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
