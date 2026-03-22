#ifndef FENG_FESIMPLEIMAGE_H
#define FENG_FESIMPLEIMAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEObject.h"

// total size: 0x5C
struct FESimpleImage : public FEObject {
    inline FESimpleImage() {}
    inline FESimpleImage(const FESimpleImage& Object, bool bReference)
        : FEObject(Object, bReference) {}
    ~FESimpleImage() override;
    FEObject* Clone(bool bReference) override;
};

#endif
