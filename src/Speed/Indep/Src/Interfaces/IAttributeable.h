#ifndef INTERFACES_SIMABLES_IATTRIBUTEABLE_H
#define INTERFACES_SIMABLES_IATTRIBUTEABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

class IAttributeable {
  public:
    IAttributeable() {}

    virtual void OnAttributeChange(const Attrib::Collection *collection, Attrib::Key attribkey);

    static void Register(IAttributeable *who, Attrib::Key class_key) {}

    static void UnRegister(IAttributeable *who) {}
};

#endif
