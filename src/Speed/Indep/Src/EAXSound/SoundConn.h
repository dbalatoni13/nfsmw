#ifndef EAXSOUND_SOUNDCONN_H
#define EAXSOUND_SOUNDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace SoundConn {

#include "Speed/Indep/Src/Sim/SimConn.h"

// total size: 0x10
class Pkt_Heli_Open : public Sim::Packet {
  public:
    static unsigned int SType() {}

    Pkt_Heli_Open(const Attrib::Collection *spec, WUID worldid) : m_VehicleSpec(spec), mWorldID(worldid) {}

    // Virtual overrides
    // Packet
    UCrc32 ConnectionClass() override {}

    unsigned int Size() override {}

    unsigned int Type() override {}

  private:
    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
};

void UpdateServices(float dT);

}; // namespace SoundConn

#endif
