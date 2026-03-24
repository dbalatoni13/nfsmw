#ifndef WORLD_VEHICLERENDERCONN_H
#define WORLD_VEHICLERENDERCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/CarRender.hpp"

struct EmitterGroup;

// TODO: Reference likely belongs in a shared world/ref header; keep it here until its original home is recovered.
class Reference {
  public:
    Reference();
    Reference(unsigned int worldid);
    Reference(const Reference &from);
    ~Reference();

    bool IsValid() const;
    void operator=(const Reference &from);
    void operator=(unsigned int id);
    unsigned int GetWorldID() const {
        return this->mWorldID;
    }
    const bMatrix4 *GetMatrix() const {
        return this->mMatrix;
    }
    const bVector3 *GetVelocity() const {
        return this->mVelocity;
    }
    const bVector3 *GetAcceleration() const {
        return this->mAcceleration;
    }

    void Set(unsigned int worldid);
    void Lock();
    void Unlock();

  private:
    unsigned int mWorldID;         // offset 0x0, size 0x4
    const bMatrix4 *mMatrix;       // offset 0x4, size 0x4
    const bVector3 *mVelocity;     // offset 0x8, size 0x4
    const bVector3 *mAcceleration; // offset 0xC, size 0x4
};

class VehicleRenderConn : public Sim::Connection, public UTL::Collections::Listable<VehicleRenderConn, 10> {
  public:
    enum eState {
        S_None = 0,
        S_Loading = 1,
        S_Loaded = 2,
        S_Updated = 3,
    };

    enum EventID {
        E_MISS_SHIFT = 0,
        E_PERFECT_SHIFT = 1,
        E_PERFECT_LAUNCH = 2,
        E_UPSHIFT = 3,
        E_DOWNSHIFT = 4,
    };

    class Effect : public bTNode<Effect> {
      public:
        Effect(const bMatrix4 *matrix);
        ~Effect();

        void Stop();
        void Fire(const bMatrix4 *worldmatrix, unsigned int emitterkey, float emitter_intensity,
                  const bVector3 *inherit_velocity) const;
        void Update(const bMatrix4 *worldmatrix, unsigned int emitterkey, float dT, float emitter_intensity,
                    const bVector3 *inherit_velocity);

        void *operator new(unsigned int size) {
            return gFastMem.Alloc(size, nullptr);
        }

        void operator delete(void *mem, unsigned int size) {
            if (mem) {
                gFastMem.Free(mem, size, nullptr);
            }
        }

        void ResetEmitterGroup() {
            mEmitterGroup = 0;
            mKey = 0;
        }

        bMatrix4 mLocalMatrix;       // offset 0x8, size 0x40
        EmitterGroup *mEmitterGroup; // offset 0x48, size 0x4
        unsigned int mKey;           // offset 0x4C, size 0x4
    };

    typedef UTL::Std::vector<VehicleRenderConn *, _type_vector> LoaderList;

    VehicleRenderConn(const Sim::ConnectionData &data, CarType type);
    virtual ~VehicleRenderConn();

    bool IsViewAnchor(eView *view) const;
    bool IsViewAnchor() const;
    bool CheckForRain(eView *view) const;
    bool CheckForRain() const;
    static VehicleRenderConn *Find(unsigned int worldid);
    void HandleEvent(EventID id);
    static void FetchData(float dT);
    static void UpdateLoading();
    unsigned int FindPart(CAR_PART_ID slot);
    unsigned int HidePart(CAR_PART_ID slot);
    void ShowPart(CAR_PART_ID slot);
    void Update(float dT);
    void SetupLoading(bool commit);
    void RefreshRenderInfo();
    bool Load(unsigned int worldID, CarRenderUsage usage, bool commit, const FECustomizationRecord *customizations);
    void Unload();
    static void RenderAll(eView *view, int reflection);
    static void RenderFlares(eView *view, int reflection, int renderFlareFlags);

    CarRenderInfo *GetRenderInfo() const {
        return this->mRenderInfo;
    }
    const bVector3 *GetPosition() const;
    const bMatrix4 *GetBodyMatrix() const {
        return this->mWorldRef.GetMatrix();
    }
    const bVector3 *GetVelocity() const {
        return this->mWorldRef.GetVelocity();
    }
    const bVector3 *GetAcceleration() const {
        return this->mWorldRef.GetAcceleration();
    }
    bool IsLoaded() const {
        return this->mState == S_Loaded;
    }
    eState GetState() const {
        return this->mState;
    }
    unsigned int GetWorldID() const {
        return this->mWorldRef.GetWorldID();
    }
    CarType GetCarType() const {
        return this->mCarType;
    }
    WCollider *GetWCollider() const {
        return this->mWCollider;
    }

  protected:
    bool CanUpdate() const;
    bool CanRender() const;
    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override;
    virtual void OnRender(eView *view, int reflection);
    virtual void OnFetch(float dT);
    virtual void OnLoaded(CarRenderInfo *render_info);
    virtual void GetRenderMatrix(bMatrix4 *matrix);
    virtual void OnEvent(EventID id);
    const bVector4 &GetModelOffset() const {
        return this->mModelOffset;
    }
    const Attrib::Gen::ecar &GetAttributes() const {
        return this->mAttributes;
    }

  public:
    static LoaderList sLoaderList; // size: 0x10
    static int mOpenSkinSlots;     // size: 0x4, address: 0x80437620

    Attrib::Gen::ecar mAttributes; // offset 0x14, size 0x14
    eState mState;                 // offset 0x28, size 0x4
    const CarType mCarType;        // offset 0x2C, size 0x4
    Reference mWorldRef;           // offset 0x30, size 0x10
    RideInfo *mRideInfo;           // offset 0x40, size 0x4
    CarRenderInfo *mRenderInfo;    // offset 0x44, size 0x4
    bVector4 mModelOffset;         // offset 0x48, size 0x10
    bool mHide;                    // offset 0x58, size 0x1
    WCollider *mWCollider;         // offset 0x5C, size 0x4
    int mSkinSlot;                 // offset 0x60, size 0x4
};

int SkinSlotToMask(int slot);

#endif
