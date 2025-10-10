#ifndef GENERATED_ATTRIBSYS_GENERICACCESSOR_H
#define GENERATED_ATTRIBSYS_GENERICACCESSOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {

namespace Gen {

class GenericAccessor : private Attrib::Instance {
  public:
    bool NO_CAR_EFFECT(bool &val, unsigned int index) const {
        // NO_CAR_EFFECT
        Attrib::TAttrib<bool> attr = Attrib::TAttrib<bool>(Get(0x1f989f01));
        if (attr.IsValid()) {
            val = attr.Get(index);
            return true;
        } else {
            return false;
        }
    }

  private:
    GenericAccessor();
};

}; // namespace Gen

}; // namespace Attrib

#endif
