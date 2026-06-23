#ifndef INTTERFACES_ISERVICEABLE_H
#define INTTERFACES_ISERVICEABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

namespace Sim {

struct Packet;

struct IServiceable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IServiceable);

    virtual bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt);
};

}; // namespace Sim

#endif
