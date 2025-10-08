#ifndef INTERFACES_SIMABLES_IRENDERABLE_H
#define INTERFACES_SIMABLES_IRENDERABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"

class IRenderable : public UTL::COM::IUnknown {
  public:
    IRenderable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IRenderable() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual bool InView() const;
    virtual bool IsRenderable() const;
    virtual HMODEL GetModelHandle() const;
    virtual const IModel *GetModel() const;
    virtual IModel *GetModel();
    virtual IModel *DistanecToView();
};

#endif
