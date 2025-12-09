#ifndef WORLD_WORLDCONN_H
#define WORLD_WORLDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WorldTypes.h"

namespace WorldConn {

// total size: 0x10
class Reference {
  public:
    Reference(unsigned int);
    ~Reference();
    void Set(unsigned int);

    const bMatrix4 *GetMatrix() const {
        return mMatrix;
    }

  private:
    unsigned int mWorldID;         // offset 0x0, size 0x4
    const bMatrix4 *mMatrix;       // offset 0x4, size 0x4
    const bVector3 *mVelocity;     // offset 0x8, size 0x4
    const bVector3 *mAcceleration; // offset 0xC, size 0x4
};

// total size: 0x30
class Pkt_Effect_Send : public Sim::Packet {
  public:
    Pkt_Effect_Send(const Attrib::Collection *effect_group, const UMath::Vector3 &pos, const UMath::Vector3 &mag, WUID owner,
                    const Attrib::Collection *owner_attrib, const Attrib::Collection *context, WUID actee)
        : mPosition(pos),                 //
          mOwner(owner),                  //
          mMagnitude(mag),                //
          mEffectGroup(effect_group),     //
          mOwnerAttributes(owner_attrib), //
          mContext(context),              //
          mActee(actee) {}

    // Virtual functions
    // Packet
    override virtual ~Pkt_Effect_Send() {}

    UMath::Vector3 mPosition;                   // offset 0x4, size 0xC
    WUID mOwner;                                // offset 0x10, size 0x4
    UMath::Vector3 mMagnitude;                  // offset 0x14, size 0xC
    const Attrib::Collection *mEffectGroup;     // offset 0x20, size 0x4
    const Attrib::Collection *mOwnerAttributes; // offset 0x24, size 0x4
    const Attrib::Collection *mContext;         // offset 0x28, size 0x4
    WUID mActee;                                // offset 0x2C, size 0x4
};

// total size: 0x20
class Pkt_Effect_Service : public Sim::Packet {
  public:
    void SetPosition(const UMath::Vector3 &pos) {
        mPosition = pos;
    }

    void SetMagnitude(const UMath::Vector3 &mag) {
        mMagnitude = mag;
    }

    void SetTracking(bool b) {
        mTracking = b;
    }

    // Virtual functions
    // Packet
    override virtual ~Pkt_Effect_Service() {}

  private:
    UMath::Vector3 mPosition;  // offset 0x4, size 0xC
    bool mTracking;            // offset 0x10, size 0x1
    UMath::Vector3 mMagnitude; // offset 0x14, size 0xC
};

} // namespace WorldConn

#endif
