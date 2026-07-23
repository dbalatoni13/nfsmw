#ifndef IRENDERABLE_H
#define IRENDERABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"

class IRenderable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IRenderable);

    virtual bool InView() const = 0;
    virtual bool IsRenderable() const = 0;
    virtual HMODEL GetModelHandle() const = 0;
    virtual const IModel *GetModel() const = 0;
    virtual IModel *GetModel() = 0;
    virtual float DistanceToView() const = 0;
};

#endif
