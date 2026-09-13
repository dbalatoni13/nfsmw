#ifndef FEMOVIEOBJECT_H_
#define FEMOVIEOBJECT_H_

#include "FEObject.h"

// total size: 0x60
// Decl: 26
class FEMovie : public FEObject {
  public:
    u32 CurTime; // offset 0x5C, size 0x4, Decl: 28

    FEMovie() : FEObject(), CurTime(0) { // Decl: 30
        Type = FE_Movie;
    }
    FEMovie(const FEMovie &Object, bool bReference) : FEObject(Object, bReference) {
        CurTime = Object.CurTime;
    }
    ~FEMovie() override {}

    FEObject *Clone(bool bReference) override { // Decl: 34
        return FNEW FEMovie(*this, bReference);
    }

    void Update(u32 tDelta) {}
};

#endif
