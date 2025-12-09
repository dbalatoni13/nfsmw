#ifndef SIM_SIMEFFECT_H
#define SIM_SIMEFFECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Sim {

// total size: 0x5C
class Effect : public UTL::COM::Object, public bTNode<Sim::Effect>, public IServiceable {
  public:
    static void Fire(const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude, WUID owner,
                     const Attrib::Collection *participant, const Attrib::Collection *context, WUID actee);

    // TODO does this constructor exist?
    Effect();
    Effect(WUID owner, const Attrib::Collection *participant);
    void Stop();
    void Set(const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude, const Attrib::Collection *context,
             bool relative, WUID actee);
    void Fire(const bMatrix4 *worldmatrix, unsigned int emitterkey, float emitter_intensity, const bVector3 *inherit_velocity) const;

    bool IsPaused() {
        return mPaused;
    }

    void Pause(bool b) {
        mPaused = b;
    }

    // Overrides
    // IUnknown
    ~Effect() override;

    // IServiceable
    bool OnService(HSIMSERVICE hCon, Packet *pkt) override;

  private:
    UMath::Vector3 mPosition;               // offset 0x20, size 0xC
    float mTime;                            // offset 0x2C, size 0x4
    UMath::Vector3 mMagnitude;              // offset 0x30, size 0xC
    HSIMSERVICE mService;                   // offset 0x3C, size 0x4
    const Attrib::Collection *mParticipant; // offset 0x40, size 0x4
    const Attrib::Collection *mContext;     // offset 0x44, size 0x4
    const Attrib::Collection *mEffect;      // offset 0x48, size 0x4
    bool mPaused;                           // offset 0x4C, size 0x1
    bool mTracking;                         // offset 0x50, size 0x1
    unsigned int mOwner;                    // offset 0x54, size 0x4
    unsigned int mActee;                    // offset 0x58, size 0x4
};

}; // namespace Sim

#endif
