//
//
//
//
//
//
//
//
//
//
#ifndef __AIROADBLOCK_H
#define __AIROADBLOCK_H 1 // Decl: 12

#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

// total size: 0xB4
// Decl: 19
class AIRoadBlock : public Sim::Activity, public IRoadBlock {
  public:
    AIRoadBlock(Sim::Param params);
    ~AIRoadBlock() override;

    static IActivity *Construct(Sim::Param params);

    // Overrides: IRoadBlock
    bool AddVehicle(IVehicle *vehicle) override;
    void AddSmackable(IPlaceableScenery *smackable, bool isSpikeStrip) override;
    bool RemoveVehicle(IVehicle *vehicle) override;
    void ReleaseAllSmackables() override;

    // Overrides: IRoadBlock
    // Decl: 47
    void SetPursuit(IPursuit *pursuit) override {
        if (this->Pursuit != pursuit) {
            if (this->Pursuit != nullptr) {
                this->Detach(this->Pursuit);
                this->Pursuit = nullptr;
            }
            this->Attach(pursuit);
            this->Pursuit = pursuit;
        }
    }

    // Overrides: IRoadBlock
    // Decl: 61
    IPursuit *GetPursuit() override {
        return this->Pursuit;
    }

    // Overrides: IRoadBlock
    int GetNumCops() override;

    // Overrides: IRoadBlock
    // Decl: 64
    void SetDodged(bool dodged) override {
        this->mDodged = dodged;
    }
    // Decl: 65
    bool GetDodged() override {
        return this->mDodged;
    }

    // Overrides: IRoadBlock
    // Decl: 67
    void IncNumCopsDestroyed() override {
        this->mNumCopsDestroyed++;
    }
    // Decl: 68
    short GetNumCopsDestroyed() override {
        return this->mNumCopsDestroyed;
    }

    // Overrides: IRoadBlock
    // Decl: 70
    void IncNumCopsDamaged() override {
        this->mNumCopsDamaged++;
    }
    // Decl: 71
    short GetNumCopsDamaged() override {
        return this->mNumCopsDamaged;
    }

    // Overrides: IRoadBlock
    // Decl: 73
    const UMath::Vector3 &GetRoadBlockCentre() override {
        return this->mRoadBlockCentre;
    }
    // Decl: 74
    const UMath::Vector3 &GetRoadBlockDir() override {
        return this->mRoadBlockDir;
    }
    // Decl: 75
    void SetRoadBlockCentre(const UMath::Vector3 &centre, const UMath::Vector3 &dir) override {
        this->mRoadBlockCentre = centre;
        this->mRoadBlockDir = dir;
    }

    // Overrides: IRoadBlock
    float GetMinDistanceToTarget(float dT, float &distxz, IVehicle **minDistVehicle) override;

    // Overrides: IRoadBlock
    // Decl: 77
    short GetNumSpikeStrips() override {
        return this->mNumSpikeStrips;
    }

    // Overrides: IRoadBlock
    // Decl: 79
    const Vehicles &GetVehicles() const override {
        return this->VehicleList;
    }
    // Decl: 80
    const Smackables &GetSmackables() const override {
        return this->RoadblockSmackableList;
    }
    IVehicle *IsComprisedOf(HSIMABLE obj) override;
    // Decl: 82
    bool IsPerpCheating() const override {
        return this->mPerpCheating;
    }

  protected:
    // Overrides: IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    Vehicles VehicleList;              // offset 0x58, size 0x10, Decl: 90
    Smackables RoadblockSmackableList; // offset 0x68, size 0x10, Decl: 91
    IPursuit *Pursuit;                 // offset 0x78, size 0x4, Decl: 93
    bool mDodged;                      // offset 0x7C, size 0x1, Decl: 94
    short mNumCopsDamaged;             // offset 0x80, size 0x2, Decl: 95
    short mNumCopsDestroyed;           // offset 0x82, size 0x2, Decl: 96
    short mNumSpikeStrips;             // offset 0x84, size 0x2, Decl: 97
    UMath::Vector3 mRoadBlockCentre;   // offset 0x88, size 0xC, Decl: 99
    UMath::Vector3 mRoadBlockDir;      // offset 0x94, size 0xC, Decl: 100
    UMath::Vector3 mPerpCheatPoint;    // offset 0xA0, size 0xC, Decl: 101
    float mPerpCheatTime;              // offset 0xAC, size 0x4, Decl: 102
    bool mPerpCheating;                // offset 0xB0, size 0x1, Decl: 103
};

// Decl: 109
enum RBElementType {
    kNone = 0,
    kCar = 1,
    kBarrier = 2,
    kSpikeStrip = 3,
};

// Decl: 119
struct RoadblockElement {
    RoadblockElement(RBElementType e, float offx, float offz, float a)
        : mElementType(e), //
          mOffsetX(offx),  //
          mOffsetZ(offz),  //
          mAngle(a) {}

    RBElementType mElementType; // offset 0x0, size 0x4
    float mOffsetX;             // offset 0x4, size 0x4
    float mOffsetZ;             // offset 0x8, size 0x4
    float mAngle;               // offset 0xC, size 0x4
};

static const int MAX_RB_ELEMENTS = 6; // Decl: 128

#define RE(a, b, c, d) RoadblockElement(a, b, c, d) // Decl: 130
#define NoRE RE(kNone, 0.f, 0.f, 0)                 // Decl: 131

// total size: 0x68
// Decl: 133
struct RoadblockSetup {
    float mMinimumWidthRequired;                 // offset 0x0, size 0x4
    int mRequiredVehicles;                       // offset 0x4, size 0x4
    RoadblockElement mContents[MAX_RB_ELEMENTS]; // offset 0x8, size 0x60
};

extern RoadblockSetup RoadblockCandidateList[16];
extern RoadblockSetup SPIKES_RoadblockCandidateList[10];

#endif
