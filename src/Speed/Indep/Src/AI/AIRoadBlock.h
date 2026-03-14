#ifndef AI_AIROADBLOCK_H
#define AI_AIROADBLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

enum RBElementType {
    kNone = 0,
    kCar = 1,
    kBarrier = 2,
    kSpikeStrip = 3,
};

struct RoadblockElement {
    RoadblockElement(enum RBElementType e, float offx, float offz, float a) {}

    RBElementType mElementType; // offset 0x0, size 0x4
    float mOffsetX;             // offset 0x4, size 0x4
    float mOffsetZ;             // offset 0x8, size 0x4
    float mAngle;               // offset 0xC, size 0x4
};

// total size: 0x68
struct RoadblockSetup {
    float mMinimumWidthRequired;   // offset 0x0, size 0x4
    int mRequiredVehicles;         // offset 0x4, size 0x4
    RoadblockElement mContents[6]; // offset 0x8, size 0x60
};

extern RoadblockSetup RoadblockCandidateList[16];
extern RoadblockSetup SPIKES_RoadblockCandidateList[10];

// total size: 0xB4
struct AIRoadBlock : Sim::Activity, IRoadBlock {
protected:
    UTL::Std::vector<IVehicle *, _type_IRoadBlockVehicles> VehicleList;                     // offset 0x58, size 0x10
    UTL::Std::vector<IPlaceableScenery *, _type_IRoadBlockSmackables> RoadblockSmackableList; // offset 0x68, size 0x10
    IPursuit *Pursuit;             // offset 0x78, size 0x4
    bool mDodged;                  // offset 0x7C, size 0x1
    short mNumCopsDamaged;         // offset 0x80, size 0x2
    short mNumCopsDestroyed;       // offset 0x82, size 0x2
    short mNumSpikeStrips;         // offset 0x84, size 0x2
    UMath::Vector3 mRoadBlockCentre; // offset 0x88, size 0xC
    UMath::Vector3 mRoadBlockDir;    // offset 0x94, size 0xC
    UMath::Vector3 mPerpCheatPoint;  // offset 0xA0, size 0xC
    float mPerpCheatTime;            // offset 0xAC, size 0x4
    bool mPerpCheating;              // offset 0xB0, size 0x1

public:
    AIRoadBlock(Sim::Param params);
    ~AIRoadBlock() override;

    static Sim::IActivity *Construct(Sim::Param params);

    bool AddVehicle(IVehicle *vehicle) override;
    void AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip) override;
    bool RemoveVehicle(IVehicle *vehicle) override;
    void ReleaseAllSmackables() override;
    float GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle) override;
    int GetNumCops() override;

    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    IVehicle *IsComprisedOf(HSIMABLE obj) override;

    void SetPursuit(IPursuit *pursuit) override;

    IPursuit *GetPursuit() override {
        return Pursuit;
    }

    void SetDodged(bool dodged) override {
        mDodged = dodged;
    }

    bool GetDodged() override {
        return mDodged;
    }

    void IncNumCopsDestroyed() override {
        mNumCopsDestroyed++;
    }

    short GetNumCopsDestroyed() override {
        return mNumCopsDestroyed;
    }

    void IncNumCopsDamaged() override {
        mNumCopsDamaged++;
    }

    short GetNumCopsDamaged() override {
        return mNumCopsDamaged;
    }

    const UMath::Vector3 &GetRoadBlockCentre() override {
        return mRoadBlockCentre;
    }

    const UMath::Vector3 &GetRoadBlockDir() override {
        return mRoadBlockDir;
    }

    void SetRoadBlockCentre(const UMath::Vector3 &centre, const UMath::Vector3 &dir) override {
        mRoadBlockCentre = centre;
        mRoadBlockDir = dir;
    }

    short GetNumSpikeStrips() override {
        return mNumSpikeStrips;
    }

    const IRoadBlock::Vehicles &GetVehicles() const override {
        return VehicleList;
    }

    const IRoadBlock::Smackables &GetSmackables() const override {
        return RoadblockSmackableList;
    }

    bool IsPerpCheating() const override {
        return mPerpCheating;
    }
};

#endif
