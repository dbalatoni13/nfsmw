#ifndef __VEHICLE_PART_DAMAGE__
#define __VEHICLE_PART_DAMAGE__

#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/Interfaces/IVehicleDamageBehaviour.h"

extern SlotPool *VehicleDamagePartSlotPool;
extern SlotPool *VehiclePartDamageZoneSlotPool;

class VehicleDamagePart {
  public:
    USE_SLOTALLOC(VehicleDamagePartSlotPool);

    void SetPivot(float x, float y, float z) {
        this->mPivot.x = x;
        this->mPivot.y = y;
        this->mPivot.z = z;
    }

    bool IsHidden() const {
        return this->mHidden;
    }

    void Hide() {
        this->mHidden = true;
    }

    bMatrix4 *GetMatrix() {
        return &this->mMatrix;
    }

    void SetMatrix(bMatrix4 *matrix) {
        this->mMatrix = *matrix;
    }

    unsigned short GetDamageLevel() const {
        return this->mDamageLevel;
    }

    void SetDamageLevel(unsigned short damageLevel) {
        this->mDamageLevel = damageLevel;
    }

    CarPart *GetDamagePart(int ix) {
        return this->mDamageParts[ix];
    }

    int GetSlotID() const {
        return this->mSlotId;
    }

    const COORD3 &GetPivot() const {
        return this->mPivot;
    }

    VehicleDamagePart(class CarRenderInfo *carRenderInfo, int slotId);
    ~VehicleDamagePart();
    void Reset();

  private:
    unsigned short mDamageLevel; // offset 0x0, size 0x2
    int mSlotId;                 // offset 0x4, size 0x4
    CarPart *mDamageParts[2];    // offset 0x8, size 0x8
    COORD3 mPivot;               // offset 0x10, size 0xC
    bMatrix4 mMatrix;            // offset 0x1C, size 0x40
    bool mAnimatable;            // offset 0x5C, size 0x1
    bool mHidden;                // offset 0x60, size 0x1
    uint32 mSlotHash;            // offset 0x64, size 0x4
};

class VehiclePartDamageZone {
  public:
    // total size: 0x8
    struct DamageZoneSlotMapDataType {
        int mZoneId; // offset 0x0, size 0x4
        int mSlotId; // offset 0x4, size 0x4
    };

    USE_SLOTALLOC(VehiclePartDamageZoneSlotPool);

    VehiclePartDamageZone(int zoneId, DamageZoneSlotMapDataType *zoneSlotMappingDataList);
    ~VehiclePartDamageZone();
    void Reset();
    int GetSlotNum() const;
    int GetSlotID(int index) const;
    void SetDamageLevel(unsigned short damageLevel);

  private:
    int mZoneId;                                             // offset 0x0, size 0x4
    unsigned short mDamageLevel;                             // offset 0x4, size 0x2
    mutable UTL::Std::vector<int, _type_vector> mSlotIdList; // offset 0x8, size 0x10
};

// total size: 0xC8
class VehiclePartDamageBehaviour : public IVehiclePartDamageBehaviour {
  public:
    // total size: 0x10
    struct BreakableWindowInfoDataType {
        int mPartSlotId;             // offset 0x0, size 0x4
        unsigned int mOriginalHash;  // offset 0x4, size 0x4
        unsigned int mReplaceTexId;  // offset 0x8, size 0x4
        unsigned int mReplaceTexId2; // offset 0xC, size 0x4
    };

#define kMaxDamagePartNum (CARSLOTID_DAMAGE_MODEL_NUM)
#define kPartDamageZoneNum (DamageZone::DZ_MAX)
    // total size: 0x8
    struct BreakableWindowHashDataType {
        unsigned int mWindowHash;  // offset 0x0, size 0x4
        unsigned int mWindowHash2; // offset 0x4, size 0x4
    };

    VehiclePartDamageBehaviour(CarRenderInfo *carRenderInfo);

    // Overrides: IVehiclePartDamageBehaviour
    ~VehiclePartDamageBehaviour() override;

    // Overrides: IVehiclePartDamageBehaviour
    void Init() override;

    void InitAnimationPivot(unsigned int slotId, const char *markerName);

    // Overrides: IVehiclePartDamageBehaviour
    void Reset() override;

    ePositionMarker *FindPositionMarker(const char *findPivotName);

    void DamageZone(int zone, int damageLevel);

    void ApplyDamage();

    void ManageGlassDamage();

    // Overrides: IVehiclePartDamageBehaviour
    bMatrix4 *GetPartMatrix(unsigned int slotId) override;

    // Overrides: IVehiclePartDamageBehaviour
    bool IsPartHidden(unsigned int slotId) override;

    // Overrides: IVehiclePartDamageBehaviour
    void HidePart(unsigned int slotId) override;

    // Overrides: IVehiclePartDamageBehaviour
    void Update(bMatrix4 *worldMatrix) override;

    // Overrides: IVehiclePartDamageBehaviour
    void Pose(bMatrix4 *worldMatrix) override;

    void ManageHoodAnimation();

    float CalcPartRotation(bMatrix4 *worldMatrix, float maxAngle, float spring, float damper, float inertia);

    float ManageTrunkAnimation(bMatrix4 *worldMatrix);

    void ManageDoorAnimation();

    void AnimatePart(unsigned int slotId, const bVector3 &rotation, bMatrix4 *worldMatrix);

    // Overrides: IVehiclePartDamageBehaviour
    void DamageVehicle(const DamageZone::Info &damageInfo) override;

    // Overrides: IVehiclePartDamageBehaviour
    void UnitTest() override;

  private:
    static BreakableWindowInfoDataType mBreakableWindowInfoList[5];
    static BreakableWindowHashDataType mDamageWindowHashList[1];
    static VehiclePartDamageZone::DamageZoneSlotMapDataType mSlotZoneMapList[120];

    CarRenderInfo *mCarRenderInfo;                              // offset 0x4, size 0x4
    unsigned int mDamageZoneNum;                                // offset 0x8, size 0x4
    VehiclePartDamageZone *mDamageZoneList[kPartDamageZoneNum]; // offset 0xC, size 0x28
    VehicleDamagePart *mDamagePartList[kMaxDamagePartNum];      // offset 0x34, size 0x5C
    bVector3 linearAcc_ch;                                      // offset 0x90, size 0x10
    bVector3 linearVel_ch;                                      // offset 0xA0, size 0x10
    bVector3 angularVel_ch;                                     // offset 0xB0, size 0x10
    float mTrunkVel;                                            // offset 0xC0, size 0x4
    float mTrunkAngle;                                          // offset 0xC4, size 0x4
};

void InitVehicleDamage();
void CloseVehicleDamage();

#endif
