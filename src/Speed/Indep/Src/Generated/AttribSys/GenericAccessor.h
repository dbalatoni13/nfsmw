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
    const Attrib::StringKey &BEHAVIOR_ORDER(unsigned int index) const {
        const Attrib::StringKey *resultptr =
            reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x104e9d16, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }

    unsigned int Num_BEHAVIOR_ORDER() const { return Get(0x104e9d16).GetLength(); }

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
