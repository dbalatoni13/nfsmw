#ifndef INTTERFACES_SIMACTIVITIES_ITRAFFIC_CENTER_H
#define INTTERFACES_SIMACTIVITIES_ITRAFFIC_CENTER_H

#pragma interface

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

class ITrafficCenter : public UTL::Collections::Listable<ITrafficCenter, 8> {
  public:
    ITrafficCenter() {}

    // Pura en el original: no existe GetTrafficBasis__14ITrafficCenter... en el ELF,
    // y la ranura correspondiente de _vt.14ITrafficCenter (0x803D31D8) es
    // __pure_virtual. Las cinco clases que heredan la sobreescriben.
    virtual bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) = 0;

    virtual ~ITrafficCenter() {}
};

#endif
