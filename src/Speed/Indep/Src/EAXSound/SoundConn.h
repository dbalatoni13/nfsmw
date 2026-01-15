#ifndef EAXSOUND_SOUNDCONN_H
#define EAXSOUND_SOUNDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace SoundConn {

// total size: 0x10
class Pkt_Heli_Open : public Sim::Packet {
  public:
    Pkt_Heli_Open(const Attrib::Collection *spec, WUID worldid) : m_VehicleSpec(spec), mWorldID(worldid) {}

    // Virtual overrides
    // Packet
    UCrc32 ConnectionClass() override {
        static UCrc32 hash = "HeliSoundConn";
        return hash;
    }

    unsigned int Size() override {
        return sizeof(*this);
    }

    unsigned int Type() override {
        return SType();
    }

    static unsigned int SType() {
        static UCrc32 hash = "Pkt_Heli_Open";
        return hash.GetValue();
    }

  private:
    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
};

void UpdateServices(float dT);

}; // namespace SoundConn

#endif
