#ifndef WORLD_INTERFACES_IVEHICLEDAMAGEBEHAVIOUR_H
#define WORLD_INTERFACES_IVEHICLEDAMAGEBEHAVIOUR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct IVehiclePartDamageBehaviour {
    // Functions
    IVehiclePartDamageBehaviour() {}

    virtual ~IVehiclePartDamageBehaviour() {}
};

class VehiclePartDamageBehaviour : public IVehiclePartDamageBehaviour {
public:
    // Inner declarations
    // total size: 0x10
    struct BreakableWindowInfoDataType {
        // Members
        int mPartSlotId; // offset 0x0, size 0x4
        unsigned int mOriginalHash; // offset 0x4, size 0x4
        unsigned int mReplaceTexId; // offset 0x8, size 0x4
        unsigned int mReplaceTexId2; // offset 0xC, size 0x4
    };
    // total size: 0x8
    struct BreakableWindowHashDataType {
        // Members
        unsigned int mWindowHash; // offset 0x0, size 0x4
        unsigned int mWindowHash2; // offset 0x4, size 0x4
    };

    // Functions
    VehiclePartDamageBehaviour(struct CarRenderInfo * carRenderInfo);

    // Overrides: IVehiclePartDamageBehaviour
    ~VehiclePartDamageBehaviour() override;

    // Overrides: IVehiclePartDamageBehaviour
    virtual void Init();

    virtual void InitAnimationPivot(unsigned int slotId, const char * markerName);

    // Overrides: IVehiclePartDamageBehaviour
    virtual void Reset();

    struct ePositionMarker * FindPositionMarker(const char * findPivotName);

    void DamageZone(int zone, int damageLevel);

    void ApplyDamage();

    void ManageGlassDamage();

    // Overrides: IVehiclePartDamageBehaviour
    virtual struct bMatrix4 * GetPartMatrix(unsigned int slotId);

    // Overrides: IVehiclePartDamageBehaviour
    virtual bool IsPartHidden(unsigned int slotId);

    // Overrides: IVehiclePartDamageBehaviour
    virtual void HidePart(unsigned int slotId);

    // Overrides: IVehiclePartDamageBehaviour
    virtual void Pose(struct bMatrix4 * worldMatrix);

    // Overrides: IVehiclePartDamageBehaviour
    virtual void Update(struct bMatrix4 * worldMatrix);

    void ManageHoodAnimation();

    float CalcPartRotation(struct bMatrix4 * worldMatrix, float maxAngle, float spring, float damper, float inertia);

    float ManageTrunkAnimation(struct bMatrix4 * worldMatrix);

    void ManageDoorAnimation();

    void AnimatePart(unsigned int slotId, const bVector3 & rotation, struct bMatrix4 * worldMatrix);

    // Overrides: IVehiclePartDamageBehaviour
    virtual void DamageVehicle(const struct Info & damageInfo);

    // Overrides: IVehiclePartDamageBehaviour
    virtual void UnitTest();

private:
    // Static members
    static struct BreakableWindowInfoDataType mBreakableWindowInfoList[5]; // size: 0x50
    static struct BreakableWindowHashDataType mDamageWindowHashList[1]; // size: 0x8
    static struct DamageZoneSlotMapDataType mSlotZoneMapList[120]; // size: 0x3C0

    // Members
    struct CarRenderInfo * mCarRenderInfo; // offset 0x4, size 0x4
    unsigned int mDamageZoneNum; // offset 0x8, size 0x4
    struct VehiclePartDamageZone * mDamageZoneList[10]; // offset 0xC, size 0x28
    struct VehicleDamagePart * mDamagePartList[23]; // offset 0x34, size 0x5C
    bVector3 linearAcc_ch; // offset 0x90, size 0x10
    bVector3 linearVel_ch; // offset 0xA0, size 0x10
    bVector3 angularVel_ch; // offset 0xB0, size 0x10
    float mTrunkVel; // offset 0xC0, size 0x4
    float mTrunkAngle; // offset 0xC4, size 0x4
};

#endif
