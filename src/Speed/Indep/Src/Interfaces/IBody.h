#ifndef INTERFACES_IBODY_H
#define INTERFACES_IBODY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

class IBody : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IBody);

    virtual void GetTransform(UMath::Matrix4 &) const;
    virtual void GetLinearVelocity(UMath::Vector3 &) const;
    virtual void GetAngularVelocity(UMath::Vector3 &) const;
    virtual void GetDimension(UMath::Vector3 &dim) const;
    virtual const Attrib::Instance &GetAttributes() const;
    virtual unsigned int GetWorldID() const;
};

#endif
