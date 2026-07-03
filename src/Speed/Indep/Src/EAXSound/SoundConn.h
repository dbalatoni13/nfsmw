#ifndef SOUND_CONN_H
#define SOUND_CONN_H

#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

#define SOUND_SERVICE UCrc32(UCRC32_EAXSOUND)
#define DECLARE_SOUNDPACKET(_PKT_, _HANDLER_)                                                                                                        \
    friend class ::_HANDLER_;                                                                                                                        \
    UCrc32 ConnectionClass() override {                                                                                                              \
        static UCrc32 hash(#_HANDLER_);                                                                                                              \
        return hash;                                                                                                                                 \
    }                                                                                                                                                \
    DECLARE_SIMPACKET(_PKT_, #_PKT_)

struct HeliSoundConn;

namespace SoundConn {

// total size: 0x10
class Pkt_Heli_Open : public Sim::Packet {
  public:
    Pkt_Heli_Open(const Attrib::Collection *spec, WUID worldid) : m_VehicleSpec(spec), mWorldID(worldid) {}

    DECLARE_SOUNDPACKET(Pkt_Heli_Open, HeliSoundConn);

  private:
    const Attrib::Collection *m_VehicleSpec; // offset 0x4, size 0x4
    WUID mWorldID;                           // offset 0x8, size 0x4
};

void InitServices();
void RestoreServices();
void UpdateServices(float dT);

}; // namespace SoundConn

#endif
