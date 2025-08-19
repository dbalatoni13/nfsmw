#ifndef INTTERFACES_SIMACTIVITIES_ITRAFFIC_CENTER_H
#define INTTERFACES_SIMACTIVITIES_ITRAFFIC_CENTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

class ITrafficCenter : public UTL::Collections::Listable<ITrafficCenter, 8> {
  public:
    ITrafficCenter() {}

    virtual bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity);

    virtual ~ITrafficCenter() {}
};

#endif
