#ifndef INTERFACES_SIMABLES_IRENDERABLE_H
#define INTERFACES_SIMABLES_IRENDERABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"

class IRenderable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRenderable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IRenderable() {}

    virtual bool InView() const = 0;
    virtual bool IsRenderable() const = 0;
    virtual HMODEL GetModelHandle() const = 0;
    virtual const IModel *GetModel() const = 0;
    virtual IModel *GetModel() = 0;
    virtual IModel *DistanceToView() = 0;
};

#endif
