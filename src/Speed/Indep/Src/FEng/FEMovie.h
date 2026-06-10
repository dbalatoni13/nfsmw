#ifndef FEMOVIEOBJECT_H_
#define FEMOVIEOBJECT_H_

#include "FEObject.h"

// File: speed/indep/src/feng/FEMovie.h
// total size: 0x60
// Decl: speed/indep/src/feng/FEMovie.h:26
class FEMovie : public FEObject {
  public:
    u32 CurTime; // offset 0x5C, size 0x4, Decl: speed/indep/src/feng/FEMovie.h:28

    FEMovie() : FEObject(), CurTime(0) {} // Decl: speed/indep/src/feng/FEMovie.h:30
    FEMovie(const struct FEMovie &Object, bool bReference) {}
    ~FEMovie() override {}

    FEObject *Clone(bool bReference) override {} // Decl: speed/indep/src/feng/FEMovie.h:34

    void Update(u32 tDelta) {}
};

#endif
