#ifndef WORLDCONN_H
#define WORLDCONN_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/effects.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "WorldTypes.h"

#include <types.h>

#define WORLD_SERVICE UCrc32(UCRC32_WORLD)
#define DECLARE_WORLDPACKET(_PKT_, _HANDLER_)                                                                                                        \
    virtual UCrc32 ConnectionClass() {                                                                                                               \
        static UCrc32 hash(#_HANDLER_);                                                                                                              \
        return hash;                                                                                                                                 \
    }                                                                                                                                                \
    DECLARE_SIMPACKET(_PKT_, #_PKT_)

DECLARE_CONTAINER_TYPE(WorldConnServerMap);

namespace WorldConn {

// total size: 0x10
class Reference {
  public:
    Reference(unsigned int);
    ~Reference();
    void Set(WUID);
    void Lock();
    void Unlock();

    bool IsValid() const {
        return mMatrix != nullptr;
    }

    WUID GetWorldID() const {
        return mWorldID;
    }

    const bMatrix4 *GetMatrix() const {
        return mMatrix;
    }

    const bVector3 *GetVelocity() const {
        return mVelocity;
    }

    const bVector3 *GetAcceleration() const {
        return mAcceleration;
    }

  private:
    WUID mWorldID;                 // offset 0x0, size 0x4
    const bMatrix4 *mMatrix;       // offset 0x4, size 0x4
    const bVector3 *mVelocity;     // offset 0x8, size 0x4
    const bVector3 *mAcceleration; // offset 0xC, size 0x4
};

// total size: 0x14
class Server {
  public:
    struct Body {
        USE_FASTALLOC(Server::Body);

        Body() {
            time = 0.0f;
            refcount = 1;
            bIdentity(&matrix);
            bFill(&velocity, 0.0f, 0.0f, 0.0f);
            bFill(&acceleration, 0.0f, 0.0f, 0.0f);
        }

        bMatrix4 matrix;       // offset 0x0, size 0x40
        bVector3 velocity;     // offset 0x40, size 0x10
        bVector3 acceleration; // offset 0x50, size 0x10
        float time;            // offset 0x60, size 0x4
        int refcount;          // offset 0x64, size 0x4
    };

    typedef UTL::Std::map<unsigned int, Body *, _type_WorldConnServerMap> BodyMap;

    USE_FASTALLOC(Server);

    Server();
    virtual ~Server();

    Body *LockID(WUID id);
    void UnlockID(WUID id);

    virtual unsigned int GetFrame() const {
        return eFrameCounter;
    }

    BodyMap mBodies; // offset 0x0, size 0x10
};

// total size: 0x48
class Pkt_Body_Open : public Sim::Packet {
  public:
    Pkt_Body_Open(unsigned int id, const UMath::Matrix4 &matrix)
        : mMatrix(matrix), //
          mID(id) {}

    DECLARE_WORLDPACKET(Pkt_Body_Open, WorldBodyConn);

    ~Pkt_Body_Open() override {}

    UMath::Matrix4 mMatrix; // offset 0x4, size 0x40
    WUID mID;               // offset 0x44, size 0x4
};

// total size: 0x50
class Pkt_Body_Service : public Sim::Packet {
  public:
    DECLARE_WORLDPACKET(Pkt_Body_Service, WorldBodyConn);

    ~Pkt_Body_Service() override {}

    void SetMatrix(const UMath::Matrix4 &matrix) {
        mMatrix = matrix;
    }

    void SetVelocity(const UMath::Vector3 &velocity) {
        mVelocity = velocity;
    }

    UMath::Matrix4 mMatrix;   // offset 0x4, size 0x40
    UMath::Vector3 mVelocity; // offset 0x44, size 0xC
};

// total size: 0x48
class Pkt_Body_Send : public Sim::Packet {
  public:
    DECLARE_WORLDPACKET(Pkt_Body_Send, WorldBodyConn);

    Pkt_Body_Send(unsigned int id, const UMath::Matrix4 &matrix)
        : mMatrix(matrix), //
          mID(id) {}

    ~Pkt_Body_Send() override {}

    UMath::Matrix4 mMatrix; // offset 0x4, size 0x40
    WUID mID;               // offset 0x44, size 0x4
};

// total size: 0x30
class Pkt_Effect_Send : public Sim::Packet {
  public:
    DECLARE_WORLDPACKET(Pkt_Effect_Send, WorldBodyConn);

    Pkt_Effect_Send(const Attrib::Collection *effect_group, const UMath::Vector3 &pos, const UMath::Vector3 &mag, WUID owner,
                    const Attrib::Collection *owner_attrib, const Attrib::Collection *context, WUID actee)
        : mPosition(pos),                 //
          mOwner(owner),                  //
          mMagnitude(mag),                //
          mEffectGroup(effect_group),     //
          mOwnerAttributes(owner_attrib), //
          mContext(context),              //
          mActee(actee) {}

    ~Pkt_Effect_Send() override {}

    ALIGN_16 UMath::Vector3 mPosition;          // offset 0x4, size 0xC
    WUID mOwner;                                // offset 0x10, size 0x4
    ALIGN_16 UMath::Vector3 mMagnitude;         // offset 0x14, size 0xC
    const Attrib::Collection *mEffectGroup;     // offset 0x20, size 0x4
    const Attrib::Collection *mOwnerAttributes; // offset 0x24, size 0x4
    const Attrib::Collection *mContext;         // offset 0x28, size 0x4
    WUID mActee;                                // offset 0x2C, size 0x4
};

// total size: 0x18
class Pkt_Effect_Open : public Sim::Packet {
  public:
    DECLARE_WORLDPACKET(Pkt_Effect_Open, WorldBodyConn);

    Pkt_Effect_Open(const Attrib::Collection *effect_group, unsigned int owner, const Attrib::Collection *owner_attrib,
                    const Attrib::Collection *context,
                    unsigned int actee)
        : mEffectGroup(effect_group),     //
          mOwner(owner),                  //
          mOwnerAttributes(owner_attrib), //
          mContext(context),              //
          mActee(actee) {}

    ~Pkt_Effect_Open() override {}

    const Attrib::Collection *mEffectGroup;     // offset 0x4, size 0x4
    WUID mOwner;                                // offset 0x8, size 0x4
    const Attrib::Collection *mOwnerAttributes; // offset 0xC, size 0x4
    const Attrib::Collection *mContext;         // offset 0x10, size 0x4
    WUID mActee;                                // offset 0x14, size 0x4
};

// total size: 0x20
class Pkt_Effect_Service : public Sim::Packet {
  public:
    DECLARE_WORLDPACKET(Pkt_Effect_Service, WorldBodyConn);

    ~Pkt_Effect_Service() override {}

    void SetPosition(const UMath::Vector3 &pos) {
        mPosition = pos;
    }

    void SetMagnitude(const UMath::Vector3 &mag) {
        mMagnitude = mag;
    }

    void SetTracking(bool b) {
        mTracking = b;
    }

    ALIGN_16 UMath::Vector3 mPosition;  // offset 0x4, size 0xC
    bool mTracking;                     // offset 0x10, size 0x1
    ALIGN_16 UMath::Vector3 mMagnitude; // offset 0x14, size 0xC
};

void InitServices();
void RestoreServices();
void InitServices();
void UpdateServices(float dT);
void RestoreServices();

} // namespace WorldConn

#endif
