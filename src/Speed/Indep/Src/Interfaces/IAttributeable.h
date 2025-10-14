#ifndef INTERFACES_SIMABLES_IATTRIBUTEABLE_H
#define INTERFACES_SIMABLES_IATTRIBUTEABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

class IAttributeable {
  public:
    IAttributeable() {}

    virtual void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey);

    static void Register(IAttributeable *who, unsigned int class_key) {}

    static void UnRegister(IAttributeable *who) {}
};

#endif
