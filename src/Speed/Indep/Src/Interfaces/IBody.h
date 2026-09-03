#ifndef INTERFACES_IBODY_H
#define INTERFACES_IBODY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

class IBody : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IBody);

    virtual void GetTransform(UMath::Matrix4 &) const = 0;
    virtual void GetLinearVelocity(UMath::Vector3 &) const = 0;
    virtual void GetAngularVelocity(UMath::Vector3 &) const = 0;
    virtual void GetDimension(UMath::Vector3 &dim) const = 0;
    virtual const Attrib::Instance &GetAttributes() const = 0;
    virtual unsigned int GetWorldID() const = 0;
};

#endif
