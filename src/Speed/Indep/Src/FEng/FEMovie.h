#ifndef _FEMOVIE
#define _FEMOVIE

#include "FEObject.h"

// total size: 0x60
struct FEMovie : public FEObject {
    unsigned long CurTime; // offset 0x5C, size 0x4

    inline FEMovie() : FEObject(), CurTime(0) {}
    inline FEMovie(const FEMovie& Object, bool bReference);
    ~FEMovie() override;

    FEObject* Clone(bool bReference) override;

    inline void Update(unsigned long tDelta) { CurTime += tDelta; }
};

#endif
