#ifndef FESIMPLEIMAGE_H_
#define FESIMPLEIMAGE_H_

#include "FEObject.h"

// File: speed/indep/src/feng/FESimpleImage.h
// total size: 0x5C
// Decl: speed/indep/src/feng/FESimpleImage.h:24
class FESimpleImage : public FEObject {
  public:
    FESimpleImage() {} // Decl: speed/indep/src/feng/FESimpleImage.h:26
    FESimpleImage(const FESimpleImage &Object, bool bReference) : FEObject(Object, bReference) {}
    ~FESimpleImage() override {} // Decl: speed/indep/src/feng/FESimpleImage.h:28

    FEObject *Clone(bool bReference) override {} // Decl: speed/indep/src/feng/FESimpleImage.h:30
};

#endif
