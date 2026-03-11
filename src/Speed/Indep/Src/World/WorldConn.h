#ifndef WORLD_WORLDCONN_H
#define WORLD_WORLDCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/stringhash.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/effects.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WorldTypes.h"

#include <types.h>

class EmitterGroup;

DECLARE_CONTAINER_TYPE(WorldConnServerMap);

namespace WorldConn {

// total size: 0x14
class Server {
  public:
    struct Body {
        void *operator new(std::size_t size) {
            return gFastMem.Alloc(size, nullptr);
        }

        void operator delete(void *mem, std::size_t size) {
            if (mem) {
                gFastMem.Free(mem, size, nullptr);
            }
        }

        bMatrix4 matrix;       // offset 0x0, size 0x40
        bVector3 velocity;     // offset 0x40, size 0x10
        bVector3 acceleration; // offset 0x50, size 0x10
        float time;            // offset 0x60, size 0x4
        int refcount;          // offset 0x64, size 0x4
    };

    typedef UTL::Std::map<unsigned int, Body *, _type_WorldConnServerMap> BodyMap;

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    Server();
    virtual ~Server();

    Body *LockID(unsigned int id);
    void UnlockID(unsigned int id);

    virtual unsigned int GetFrame() const;

    BodyMap mBodies; // offset 0x0, size 0x10
};

// total size: 0x10
class Reference {
  public:
    Reference(unsigned int);
    ~Reference();
    void Set(unsigned int);
    void Lock();
    void Unlock();

    bool IsValid() const {
        return mMatrix != nullptr;
    }

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
    ~Pkt_Effect_Send() override {}

    ALIGN_16 UMath::Vector3 mPosition;          // offset 0x4, size 0xC
    WUID mOwner;                                // offset 0x10, size 0x4
    ALIGN_16 UMath::Vector3 mMagnitude;         // offset 0x14, size 0xC
    const Attrib::Collection *mEffectGroup;     // offset 0x20, size 0x4
    const Attrib::Collection *mOwnerAttributes; // offset 0x24, size 0x4
    const Attrib::Collection *mContext;         // offset 0x28, size 0x4
    WUID mActee;                                // offset 0x2C, size 0x4
};

// total size: 0x50
class Pkt_Body_Service : public Sim::Packet {
  public:
    void SetMatrix(const UMath::Matrix4 &matrix) {
        mMatrix = matrix;
    }

    void SetVelocity(const UMath::Vector3 &velocity) {
        mVelocity = velocity;
    }

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("WorldBodyConn");
        return hash;
    }

    unsigned int Size() override {
        return 0x50;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_Body_Service");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

    ~Pkt_Body_Service() override {}

    UMath::Matrix4 mMatrix;   // offset 0x4, size 0x40
    UMath::Vector3 mVelocity; // offset 0x44, size 0xC
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

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("WorldEffectConn");
        return hash;
    }

    unsigned int Size() override {
        return 0x20;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_Effect_Service");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

    ~Pkt_Effect_Service() override {}

  private:
    ALIGN_16 UMath::Vector3 mPosition;  // offset 0x4, size 0xC
    bool mTracking;                     // offset 0x10, size 0x1
    ALIGN_16 UMath::Vector3 mMagnitude; // offset 0x14, size 0xC
};

// total size: 0x48
class Pkt_Body_Open : public Sim::Packet {
  public:
    Pkt_Body_Open(unsigned int id, const UMath::Matrix4 &matrix)
        : mMatrix(matrix), //
          mID(id) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("WorldBodyConn");
        return hash;
    }

    unsigned int Size() override {
        return 0x48;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_Body_Open");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

    ~Pkt_Body_Open() override {}

    UMath::Matrix4 mMatrix; // offset 0x4, size 0x40
    unsigned int mID;       // offset 0x44, size 0x4
};

// total size: 0x18
class Pkt_Effect_Open : public Sim::Packet {
  public:
    Pkt_Effect_Open(const Attrib::Collection *effect_group, unsigned int owner, const Attrib::Collection *owner_attrib, const Attrib::Collection *context,
                    unsigned int actee)
        : mEffectGroup(effect_group),     //
          mOwner(owner),                  //
          mOwnerAttributes(owner_attrib), //
          mContext(context),              //
          mActee(actee) {}

    ~Pkt_Effect_Open() override {}

    const Attrib::Collection *mEffectGroup;     // offset 0x4, size 0x4
    unsigned int mOwner;                        // offset 0x8, size 0x4
    const Attrib::Collection *mOwnerAttributes; // offset 0xC, size 0x4
    const Attrib::Collection *mContext;         // offset 0x10, size 0x4
    unsigned int mActee;                        // offset 0x14, size 0x4
};

extern Server *_Server;
extern int world_refcount;

void InitServices();
void RestoreServices();
void UpdateServices(float dT);

} // namespace WorldConn

class WorldBodyConn : public Sim::Connection, public bTNode<WorldBodyConn> {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    WorldBodyConn(const Sim::ConnectionData &data);
    ~WorldBodyConn() override;

    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    void Update(float dT);
    static void FetchData(float dT);

    static bTList<WorldBodyConn> mList;

    unsigned int mID;                 // offset 0x18, size 0x4
    WorldConn::Server::Body *mDest;   // offset 0x1C, size 0x4
};

class WorldEffectConn : public Sim::Connection, public bTNode<WorldEffectConn> {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    WorldEffectConn(const Sim::ConnectionData &data, const WorldConn::Pkt_Effect_Open *oc);
    ~WorldEffectConn() override;

    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    void ResetEmitterGroup() {
        mEmitters = nullptr;
    }

    void Update(float dT);
    static void FetchData(float dT);

    static bTList<WorldEffectConn> mList;

    Attrib::Gen::effects mAttributes; // offset 0x18, size 0x14
    WorldConn::Reference mOwnerRef;   // offset 0x2C, size 0x10
    EmitterGroup *mEmitters;          // offset 0x3C, size 0x4
    bool mPaused;                     // offset 0x40, size 0x1
    bool mSilent;                     // offset 0x44, size 0x1
    void *mAudioEvent;               // offset 0x48, size 0x4
    unsigned int mActee;              // offset 0x4C, size 0x4
};

#endif
