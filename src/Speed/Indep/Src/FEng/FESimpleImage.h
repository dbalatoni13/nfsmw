#ifndef FESIMPLEIMAGE_H_
#define FESIMPLEIMAGE_H_

#include "FEObject.h"

// total size: 0x5C
// Decl: 24
class FESimpleImage : public FEObject {
  public:
    FESimpleImage() { // Decl: 26
        Type = FE_SimpleImage;
    }
    FESimpleImage(const FESimpleImage &Object, bool bReference) : FEObject(Object, bReference) {}
    ~FESimpleImage() override {} // Decl: 28

    FEObject *Clone(bool bReference) override { // Decl: 30
        return FNEW FESimpleImage(*this, bReference);
    }
};

#endif
