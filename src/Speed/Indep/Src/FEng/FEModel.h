#ifndef FEMODEL_H_
#define FEMODEL_H_

#include "Speed/Indep/Src/FEng/FEObject.h"

// total size: 0x44
// Decl: 20
class FEModelData : public FEObjData {};

// total size: 0x5C
// Decl: 31
class FEModel : public FEObject {
  public:
    FEModel() {} // Decl: 33
    FEModel(const FEModel &Model, bool bReference) {}
    ~FEModel() override {}

    FEObject *Clone(bool bReference) override {} // Decl: 37

    FEModelData *GetModelData() {} // Decl: 39
};

#endif
