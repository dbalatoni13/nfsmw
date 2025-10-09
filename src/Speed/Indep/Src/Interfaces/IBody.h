#ifndef INTERFACES_IBODY_H
#define INTERFACES_IBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

class IBody : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IBody() {}

    virtual void GetTransform(UMath::Matrix4 &) const;
    virtual void GetLinearVelocity(UMath::Vector3 &) const;
    virtual void GetAngularVelocity(UMath::Vector3 &) const;
    virtual void GetDimension(UMath::Vector3 &dim) const;
    virtual const Attrib::Instance &GetAttributes() const;
    virtual unsigned int GetWorldID() const;
};

#endif
