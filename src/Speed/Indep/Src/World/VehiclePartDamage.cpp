#include "Speed/Indep/Src/World/Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

typedef float Mtx44[4][4];

extern SlotPool *VehicleDamagePartSlotPool;
extern SlotPool *VehiclePartDamageZoneSlotPool;
extern unsigned int unitTestDelay;
extern unsigned int uniTestLevel;
extern "C" void PSMTX44Copy(const Mtx44 src, Mtx44 dst);
extern const char lbl_8040BC8C[] asm("lbl_8040BC8C");
extern const char lbl_8040D09C[] asm("lbl_8040D09C");
extern const char lbl_8040D0A4[] asm("lbl_8040D0A4");
extern const char lbl_8040D0B0[] asm("lbl_8040D0B0");
extern const char lbl_8040D0BC[] asm("lbl_8040D0BC");
extern const char lbl_8040D0CC[] asm("lbl_8040D0CC");
extern const char lbl_8040D0DC[] asm("lbl_8040D0DC");
extern float lbl_8040D0E8;
extern float lbl_8040D0EC;
extern float lbl_8040D0F0;
extern float lbl_8040D0F4;
extern float lbl_8040D0F8;
extern float lbl_8040D100;
extern float lbl_8040D104;
extern float lbl_8040D108;
extern float lbl_8040D10C;
extern float lbl_8040D110;
extern float lbl_8040D114;
extern float lbl_8040D118;
extern float lbl_8040D11C;
extern float lbl_8040D120;
extern float lbl_8040D124;
extern float lbl_8040D128;
extern float lbl_8040D12C;
extern float lbl_8040D130;
extern float lbl_8040D134;
extern float lbl_8040D138;
extern float lbl_8040D13C;
extern float lbl_8040D140;
extern float lbl_8040D144;
extern float lbl_8040D14C;

extern VehicleDamagePart *VehicleDamagePart_ctor(VehicleDamagePart *part, CarRenderInfo *carRenderInfo, int slotId)
    asm("__17VehicleDamagePartP13CarRenderInfoi");
extern void VehicleDamagePart_dtor(VehicleDamagePart *part, int in_chrg) asm("_._17VehicleDamagePart");
extern void VehicleDamagePart_Reset(VehicleDamagePart *part) asm("Reset__17VehicleDamagePart");

extern VehiclePartDamageZone *VehiclePartDamageZone_ctor(
    VehiclePartDamageZone *zone,
    int zoneId,
    void *zoneSlotMappingDataList)
    asm("__21VehiclePartDamageZoneiPQ221VehiclePartDamageZone25DamageZoneSlotMapDataType");
extern void VehiclePartDamageZone_dtor(VehiclePartDamageZone *zone, int in_chrg) asm("_._21VehiclePartDamageZone");
extern void VehiclePartDamageZone_Reset(VehiclePartDamageZone *zone) asm("Reset__21VehiclePartDamageZone");
extern int VehiclePartDamageZone_GetSlotNum(const VehiclePartDamageZone *zone) asm("GetSlotNum__C21VehiclePartDamageZone");
extern int VehiclePartDamageZone_GetSlotID(const VehiclePartDamageZone *zone, int index) asm("GetSlotID__C21VehiclePartDamageZonei");
extern void VehiclePartDamageZone_SetDamageLevel(VehiclePartDamageZone *zone, unsigned short damageLevel)
    asm("SetDamageLevel__21VehiclePartDamageZoneUs");

VehiclePartDamageBehaviour::VehiclePartDamageBehaviour(CarRenderInfo *carRenderInfo)
    : mCarRenderInfo(carRenderInfo), //
      mDamageZoneNum(0), //
      mTrunkVel(0.0f), //
      mTrunkAngle(0.0f) {
    unsigned int slotId;
    int zoneId;

    for (slotId = 0; slotId < 0x17; slotId++) {
        this->mDamagePartList[slotId] = VehicleDamagePart_ctor(
            static_cast<VehicleDamagePart *>(bOMalloc(VehicleDamagePartSlotPool)),
            carRenderInfo,
            static_cast<int>(slotId));
    }

    for (zoneId = 0; zoneId < 10; zoneId++) {
        unsigned int zoneIndex = this->mDamageZoneNum;
        this->mDamageZoneNum = zoneIndex + 1;
        this->mDamageZoneList[zoneIndex] =
            VehiclePartDamageZone_ctor(
                static_cast<VehiclePartDamageZone *>(bOMalloc(VehiclePartDamageZoneSlotPool)),
                zoneId,
                mSlotZoneMapList);
    }
}

VehiclePartDamageBehaviour::~VehiclePartDamageBehaviour() {
    unsigned int slotId;

    for (slotId = 0; slotId < 0x17; slotId++) {
        if (this->mDamagePartList[slotId] != 0) {
            VehicleDamagePart_dtor(this->mDamagePartList[slotId], 3);
            this->mDamagePartList[slotId] = 0;
        }
    }

    for (slotId = 0; slotId < this->mDamageZoneNum; slotId++) {
        if (this->mDamageZoneList[slotId] != 0) {
            VehiclePartDamageZone_dtor(this->mDamageZoneList[slotId], 3);
            this->mDamageZoneList[slotId] = 0;
        }
    }
}

void VehiclePartDamageBehaviour::Reset() {
    unsigned int slotId;

    for (slotId = 0; slotId < 0x17; slotId++) {
        VehicleDamagePart_Reset(this->mDamagePartList[slotId]);
    }

    for (slotId = 0; slotId < this->mDamageZoneNum; slotId++) {
        VehiclePartDamageZone_Reset(this->mDamageZoneList[slotId]);
        this->DamageZone(static_cast<int>(slotId), 0);
    }

    this->ApplyDamage();
}

void VehiclePartDamageBehaviour::DamageZone(int zone, int damageLevel) {
    VehiclePartDamageZone *damageZone = this->mDamageZoneList[zone];

    if (damageZone != 0) {
        int slotIndex;
        int slotCount;

        VehiclePartDamageZone_SetDamageLevel(damageZone, static_cast<unsigned short>(damageLevel));

        for (slotIndex = 0, slotCount = VehiclePartDamageZone_GetSlotNum(damageZone); slotIndex < slotCount; slotIndex++) {
            int damageSlotId = VehiclePartDamageZone_GetSlotID(damageZone, slotIndex);
            VehicleDamagePart *damagePart = this->mDamagePartList[damageSlotId];

            if (damagePart != 0) {
                unsigned int nextDamageLevel = static_cast<unsigned int>(damageLevel);
                unsigned short *currentDamageLevel = reinterpret_cast<unsigned short *>(damagePart);

                if (static_cast<int>(nextDamageLevel) < static_cast<int>(*currentDamageLevel)) {
                    nextDamageLevel = *currentDamageLevel;
                }

                if (static_cast<int>(nextDamageLevel) < 1) {
                    nextDamageLevel = 1;
                }

                *currentDamageLevel = static_cast<unsigned short>(nextDamageLevel);

                if (this->mCarRenderInfo->pRideInfo != 0) {
                    CarPart *replacement =
                        *reinterpret_cast<CarPart **>(reinterpret_cast<unsigned char *>(damagePart) + 0x8 + nextDamageLevel * sizeof(CarPart *));
                    this->mCarRenderInfo->pRideInfo->SetPart(damageSlotId, replacement, false);
                }

                this->ManageGlassDamage();
            }
        }
    }
}

void VehiclePartDamageBehaviour::ApplyDamage() {
    if (this->mCarRenderInfo != 0) {
        if (this->mCarRenderInfo->pRideInfo != 0) {
            this->mCarRenderInfo->pRideInfo->UpdatePartsEnabled();
        }

        this->mCarRenderInfo->UpdateCarParts();
    }
}

bMatrix4 *VehiclePartDamageBehaviour::GetPartMatrix(unsigned int slotId) {
    if (slotId > 0x17) {
        return 0;
    }

    return reinterpret_cast<bMatrix4 *>(reinterpret_cast<unsigned char *>(this->mDamagePartList[slotId]) + 0x1C);
}

bool VehiclePartDamageBehaviour::IsPartHidden(unsigned int slotId) {
    if (slotId > 0x16) {
        return true;
    }

    return *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(this->mDamagePartList[slotId]) + 0x60) != 0;
}

void VehiclePartDamageBehaviour::HidePart(unsigned int slotId) {
    if (slotId > 0x17) {
        return;
    }

    *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(this->mDamagePartList[slotId]) + 0x60) = 1;
}

void VehiclePartDamageBehaviour::Init() {
    float *pivot;

    this->InitAnimationPivot(CARSLOTID_DAMAGE_HOOD, lbl_8040BC8C);
    this->InitAnimationPivot(CARSLOTID_DAMAGE_TRUNK, lbl_8040D09C);
    this->InitAnimationPivot(CARSLOTID_DAMAGE_LEFT_DOOR, lbl_8040D0A4);
    this->InitAnimationPivot(CARSLOTID_DAMAGE_RIGHT_DOOR, lbl_8040D0B0);
    this->InitAnimationPivot(CARSLOTID_DAMAGE_LEFT_REAR_DOOR, lbl_8040D0BC);
    this->InitAnimationPivot(CARSLOTID_DAMAGE_RIGHT_REAR_DOOR, lbl_8040D0CC);
    this->InitAnimationPivot(CARSLOTID_DAMAGE_REAR_BUMPER, lbl_8040D0DC);

    pivot = reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(this->mDamagePartList[CARSLOTID_DAMAGE_TRUNK]) + 0x10);
    pivot[0] = lbl_8040D0E8;
    pivot[1] = lbl_8040D0EC;
    pivot[2] = lbl_8040D0F0;

    pivot = reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(this->mDamagePartList[CARSLOTID_DAMAGE_HOOD]) + 0x10);
    pivot[0] = lbl_8040D0F4;
    pivot[1] = lbl_8040D0F8;
    pivot[2] = lbl_8040D0F0;
}

void VehiclePartDamageBehaviour::ManageGlassDamage() {
    int windowIx;

    for (windowIx = 0; windowIx < 5; windowIx++) {
        const BreakableWindowInfoDataType &windowInfo = mBreakableWindowInfoList[windowIx];
        VehicleDamagePart *damagePart = this->mDamagePartList[windowInfo.mPartSlotId];

        if (this->mCarRenderInfo != 0) {
            eReplacementTextureTable *replacementTexture =
                &this->mCarRenderInfo->MasterReplacementTextureTable[windowInfo.mReplaceTexId];

            if (bMin(static_cast<int>(*reinterpret_cast<unsigned short *>(damagePart)), 1) == 0) {
                if (replacementTexture->GetNewNameHash() != windowInfo.mOriginalHash) {
                    replacementTexture->SetNewNameHash(windowInfo.mOriginalHash);
                }
            } else {
                unsigned int damageHash = 0x0A155545;

                if (replacementTexture->GetNewNameHash() != damageHash) {
                    replacementTexture->SetNewNameHash(damageHash);
                }

                if (this->mCarRenderInfo->MasterReplacementTextureTable[CarRenderInfo::REPLACETEX_WINDOW_REAR_DEFOST].GetNewNameHash() != damageHash) {
                    this->mCarRenderInfo->MasterReplacementTextureTable[CarRenderInfo::REPLACETEX_WINDOW_REAR_DEFOST].SetNewNameHash(damageHash);
                }
            }
        }
    }
}

void VehiclePartDamageBehaviour::InitAnimationPivot(unsigned int slotId, const char * markerName) {
    if (this->FindPositionMarker(markerName) != 0) {
        VehicleDamagePart *damagePart = this->mDamagePartList[slotId];
        float *pivot = reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(damagePart) + 0x10);

        pivot[0] = 0.0f;
        pivot[1] = 0.0f;
        pivot[2] = 0.0f;
    }
}

void VehiclePartDamageBehaviour::Pose(bMatrix4 *worldMatrix) {
    unsigned int partIx;

    for (partIx = 0; partIx < 0x17; partIx++) {
        PSMTX44Copy(
            *reinterpret_cast<const Mtx44 *>(worldMatrix),
            *reinterpret_cast<Mtx44 *>(reinterpret_cast<unsigned char *>(this->mDamagePartList[partIx]) + 0x1C));
    }
}

void VehiclePartDamageBehaviour::DamageVehicle(const DamageZone::Info &damageInfo) {
    unsigned int zoneIx;

    for (zoneIx = 0; zoneIx < this->mDamageZoneNum; zoneIx++) {
        this->DamageZone(static_cast<int>(zoneIx), static_cast<int>(damageInfo.Get(static_cast<DamageZone::ID>(zoneIx))));
    }

    this->ApplyDamage();
}

void VehiclePartDamageBehaviour::Update(bMatrix4 *worldMatrix) {
    this->Pose(worldMatrix);

    if (*reinterpret_cast<unsigned short *>(this->mDamagePartList[CARSLOTID_DAMAGE_TRUNK]) == 1) {
        bVector3 rotation;

        rotation.x = this->CalcPartRotation(
                         worldMatrix,
                         lbl_8040D100 * lbl_8040D104,
                         lbl_8040D108,
                         lbl_8040D10C,
                         lbl_8040D110) *
                     lbl_8040D114;
        rotation.y = lbl_8040D118;
        rotation.z = lbl_8040D118;
        this->AnimatePart(CARSLOTID_DAMAGE_TRUNK, rotation, worldMatrix);
    }

    if (*reinterpret_cast<unsigned short *>(this->mDamagePartList[CARSLOTID_DAMAGE_HOOD]) == 1) {
        bVector3 rotation;

        rotation.x = -this->CalcPartRotation(
                          worldMatrix,
                          lbl_8040D100 * lbl_8040D104,
                          lbl_8040D108,
                          lbl_8040D10C,
                          lbl_8040D11C) *
                     lbl_8040D114;
        rotation.y = lbl_8040D118;
        rotation.z = lbl_8040D118;
        this->AnimatePart(CARSLOTID_DAMAGE_HOOD, rotation, worldMatrix);
    }

    if (*reinterpret_cast<unsigned short *>(this->mDamagePartList[CARSLOTID_DAMAGE_REAR_BUMPER]) == 1) {
        this->CalcPartRotation(
            worldMatrix,
            lbl_8040D100 * lbl_8040D104,
            lbl_8040D108,
            lbl_8040D10C,
            lbl_8040D110);
    }
}

float VehiclePartDamageBehaviour::CalcPartRotation(bMatrix4 *worldMatrix, float maxAngle, float spring, float damper, float inertia) {
    static float tCs;
    static float tCd;
    static float tI;
    static bool tCsInited = false;
    static bool tCdInited = false;
    static bool tIInited = false;

    bMatrix4 localInverse;
    bMatrix4 localMatrix;

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(worldMatrix), *reinterpret_cast<Mtx44 *>(&localInverse));
    bInvertMatrix(&localInverse, &localInverse);
    eMulMatrix(&localMatrix, worldMatrix, &localInverse);

    eMulVector(&this->angularVel_ch, &localMatrix, &this->mCarRenderInfo->mAngularVelocity);
    eMulVector(&this->linearVel_ch, &localMatrix, &this->mCarRenderInfo->mVelocity);
    eMulVector(&this->linearAcc_ch, &localMatrix, &this->mCarRenderInfo->mAcceleration);

    this->angularVel_ch.x *= lbl_8040D120;
    this->angularVel_ch.y *= lbl_8040D120;
    this->angularVel_ch.z *= lbl_8040D120;

    if (lbl_8040D124 < maxAngle) {
        float dT = this->mCarRenderInfo->mDeltaTime;
        float desttheta = lbl_8040D124;
        float acc;
        float absValue;

        if (worldMatrix->v2.z * lbl_8040D128 < lbl_8040D124) {
            desttheta = worldMatrix->v2.z * lbl_8040D128;
        }

        if (!tCsInited) {
            tCs = spring;
            tCsInited = true;
        }

        if (!tCdInited) {
            tCd = damper;
            tCdInited = true;
        }

        if (!tIInited) {
            tI = inertia;
            tIInited = true;
        }

        acc = tCs * (-desttheta - this->mTrunkAngle) + tCd * (-this->mTrunkVel);

        absValue = this->linearAcc_ch.x;
        if (absValue < lbl_8040D124) {
            absValue = -absValue;
        }
        if (lbl_8040D12C < absValue) {
            acc -= this->linearAcc_ch.x / tI;
        }

        absValue = this->linearVel_ch.z;
        if (absValue < lbl_8040D124) {
            absValue = -absValue;
        }
        if (lbl_8040D130 < absValue) {
            acc += (-this->linearVel_ch.z * lbl_8040D134) / tI;
        }

        absValue = -this->angularVel_ch.y;
        if (absValue < lbl_8040D124) {
            absValue = this->angularVel_ch.y;
        }
        if (lbl_8040D138 < absValue) {
            acc += -this->angularVel_ch.y * lbl_8040D13C;
        }

        this->mTrunkVel = acc * dT + this->mTrunkVel;
        this->mTrunkAngle = this->mTrunkVel * dT + this->mTrunkAngle;

        if (lbl_8040D124 <= this->mTrunkAngle) {
            if (this->mTrunkAngle > maxAngle) {
                this->mTrunkAngle = maxAngle;
                this->mTrunkVel = -this->mTrunkVel * lbl_8040D144;
            }
        } else {
            this->mTrunkAngle = lbl_8040D124;
            this->mTrunkVel = -this->mTrunkVel * lbl_8040D140;
        }
    } else {
        this->mTrunkAngle = lbl_8040D124;
    }

    return this->mTrunkAngle;
}

void VehiclePartDamageBehaviour::AnimatePart(unsigned int slotId, const bVector3 &rotation, bMatrix4 *worldMatrix) {
    if (slotId < 0x18) {
        int lod = this->mCarRenderInfo->mMinLodLevel;
        VehicleDamagePart *damagePart = this->mDamagePartList[slotId];
        bMatrix4 *partMatrix = reinterpret_cast<bMatrix4 *>(reinterpret_cast<unsigned char *>(damagePart) + 0x1C);

        while (lod <= this->mCarRenderInfo->mMaxLodLevel &&
               ((reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this->mCarRenderInfo) + 0xB78 + slotId * 0x14)[lod] & 1) == 0)) {
            if (damagePart != 0) {
                bMatrix4 localInverse;
                bVector3 pivot;
                bVector3 offset;

                PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(worldMatrix), *reinterpret_cast<Mtx44 *>(&localInverse));
                bInvertMatrix(&localInverse, &localInverse);
                eMulMatrix(partMatrix, worldMatrix, &localInverse);

                pivot.x = *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(damagePart) + 0x10);
                pivot.y = *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(damagePart) + 0x14);
                pivot.z = *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(damagePart) + 0x18);
                offset = pivot;
                eTranslate(partMatrix, partMatrix, &offset);
                eRotateX(partMatrix, partMatrix, static_cast<unsigned short>(static_cast<int>(rotation.x * lbl_8040D14C) / 0x168));
                eRotateY(partMatrix, partMatrix, static_cast<unsigned short>(static_cast<int>(rotation.y * lbl_8040D14C) / 0x168));
                eRotateZ(partMatrix, partMatrix, static_cast<unsigned short>(static_cast<int>(rotation.z * lbl_8040D14C) / 0x168));
                offset.x = -pivot.x;
                offset.y = -pivot.y;
                offset.z = -pivot.z;
                eTranslate(partMatrix, partMatrix, &offset);
                eMulMatrix(partMatrix, partMatrix, worldMatrix);
            }

            lod++;
        }
    }
}

void VehiclePartDamageBehaviour::UnitTest() {
    unitTestDelay--;

    if (unitTestDelay == 0) {
        unsigned int zoneIx;

        unitTestDelay = 0x1E;
        uniTestLevel++;
        if (uniTestLevel > 1) {
            uniTestLevel = 0;
        }

        for (zoneIx = 0; zoneIx < this->mDamageZoneNum; zoneIx++) {
            this->DamageZone(static_cast<int>(zoneIx), static_cast<int>(uniTestLevel));
        }

        this->ApplyDamage();
    }
}
