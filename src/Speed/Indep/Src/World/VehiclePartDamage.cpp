#include "Speed/Indep/Src/World/Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

typedef float Mtx44[4][4];

extern SlotPool *VehicleDamagePartSlotPool;
extern SlotPool *VehiclePartDamageZoneSlotPool;

struct VehiclePartDamageZone {
    struct DamageZoneSlotMapDataType {
        int ZoneId;
        int SlotId;
    };

    int mZoneId;
    unsigned short mDamageLevel;
    unsigned short pad;
    int *mSlotIdsStart;
    int *mSlotIdsEnd;
    int *mSlotIdsReserved;
    int *mSlotIdsCapacityEnd;

    static void operator delete(void *ptr) {
        bFree(VehiclePartDamageZoneSlotPool, ptr);
    }

    VehiclePartDamageZone(int zoneId, DamageZoneSlotMapDataType *zoneSlotMappingDataList);
    ~VehiclePartDamageZone();
    void Reset();
    int GetSlotNum() const;
    int GetSlotID(int index) const;
    void SetDamageLevel(unsigned short damageLevel);
};

struct CarPart;
struct CarPartDatabase;

struct VehicleDamagePart {
    unsigned short mDamageLevel;
    unsigned short pad;
    int mSlotId;
    CarPart *mReplacementParts[2];
    float mAnimationPivot[3];
    bMatrix4 mMatrix;
    int mAttached;
    bool mHidden;

    static void operator delete(void *ptr) {
        bFree(VehicleDamagePartSlotPool, ptr);
    }

    inline void SetPivot(float x, float y, float z) {
        mAnimationPivot[0] = x;
        mAnimationPivot[1] = y;
        mAnimationPivot[2] = z;
    }

    inline bool IsHidden() const {
        return mHidden;
    }

    inline bMatrix4 *GetMatrix() {
        return &mMatrix;
    }

    inline unsigned short GetDamageLevel() const {
        return mDamageLevel;
    }

    inline void SetDamageLevel(unsigned short damageLevel) {
        mDamageLevel = damageLevel;
    }

    inline CarPart *GetDamagePart(int ix) {
        return mReplacementParts[ix];
    }

    inline int GetSlotID() const {
        return mSlotId;
    }

    VehicleDamagePart(CarRenderInfo *carRenderInfo, int slotId);
    ~VehicleDamagePart();
    void Reset();
};

extern CarPartDatabase CarPartDB;
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
extern const char lbl_8040D154[] asm("lbl_8040D154");
extern const char lbl_8040D170[] asm("lbl_8040D170");
extern CarPart *CarPartDatabase_NewGetNextCarPart(
    CarPartDatabase *carPartDB,
    CarPart *car_part,
    CarType car_type,
    int car_slot_id,
    unsigned int car_part_namehash,
    int upgrade_level)
    asm("NewGetNextCarPart__15CarPartDatabaseP7CarPart7CarTypeiUii");

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

IVehiclePartDamageBehaviour::~IVehiclePartDamageBehaviour() {}

VehiclePartDamageZone::VehiclePartDamageZone(int zoneId, DamageZoneSlotMapDataType *zoneSlotMappingDataList) {
    int zoneSlotIndex;
    int currentZoneId;
    int slotId;

    mZoneId = zoneId;
    mDamageLevel = 0;
    mSlotIdsStart = 0;
    mSlotIdsEnd = 0;
    mSlotIdsCapacityEnd = 0;
    reinterpret_cast<UTL::Std::vector<int, _type_vector> *>(&mSlotIdsStart)->reserve(0x14);

    if (zoneSlotMappingDataList != 0) {
        zoneSlotIndex = 0;
        do {
            currentZoneId = zoneSlotMappingDataList[zoneSlotIndex].ZoneId;
            if (currentZoneId == mZoneId) {
                slotId = zoneSlotMappingDataList[zoneSlotIndex].SlotId;
                reinterpret_cast<UTL::Std::vector<int, _type_vector> *>(&mSlotIdsStart)->push_back(slotId);
            }
            zoneSlotIndex++;
        } while (currentZoneId != 0xFFFF);
    }
}

VehiclePartDamageZone::~VehiclePartDamageZone() {
    reinterpret_cast<UTL::Std::vector<int, _type_vector> *>(&mSlotIdsStart)->clear();
    reinterpret_cast<UTL::Std::vector<int, _type_vector> *>(&mSlotIdsStart)->~vector();
}

void VehiclePartDamageZone::Reset() {
    mDamageLevel = 0;
}

int VehiclePartDamageZone::GetSlotNum() const {
    return mSlotIdsEnd - mSlotIdsStart;
}

int VehiclePartDamageZone::GetSlotID(int index) const {
    int offset = index * sizeof(int);
    unsigned char *slot_ids = reinterpret_cast<unsigned char *>(mSlotIdsStart);
    return *reinterpret_cast<int *>(slot_ids + offset);
}

void VehiclePartDamageZone::SetDamageLevel(unsigned short damageLevel) {
    mDamageLevel = damageLevel;
}

VehicleDamagePart::VehicleDamagePart(CarRenderInfo *carRenderInfo, int slotId) {
    int replacementIndex;
    RideInfo *rideInfo;

    mDamageLevel = 0;
    mSlotId = slotId;
    mAttached = 1;
    mHidden = 0;
    mAnimationPivot[0] = 0.0f;
    mAnimationPivot[1] = 0.0f;
    mAnimationPivot[2] = 0.0f;
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&mMatrix));

    if (carRenderInfo != 0 && (rideInfo = carRenderInfo->pRideInfo) != 0) {
        replacementIndex = 1;
        mReplacementParts[0] = rideInfo->GetPart(mSlotId);
        do {
            if (mReplacementParts[replacementIndex - 1] != 0) {
                mReplacementParts[replacementIndex] =
                    CarPartDatabase_NewGetNextCarPart(&CarPartDB, mReplacementParts[replacementIndex - 1], rideInfo->Type, mSlotId, 0, -1);
            }
            replacementIndex++;
        } while (replacementIndex < 2);
    }
}

VehicleDamagePart::~VehicleDamagePart() {
    int replacementIndex;

    replacementIndex = 0;
    do {
        mReplacementParts[replacementIndex] = 0;
        replacementIndex++;
    } while (replacementIndex < 2);
}

void VehicleDamagePart::Reset() {
    mDamageLevel = 0;
    mHidden = 0;
    mAttached = 1;
}

void InitVehicleDamage() {
    VehicleDamagePartSlotPool = bNewSlotPool(0x68, 0xF0, lbl_8040D154, 0);
    VehiclePartDamageZoneSlotPool = bNewSlotPool(0x18, 100, lbl_8040D170, 0);
}

ePositionMarker *VehiclePartDamageBehaviour::FindPositionMarker(const char *findPivotName) {
    return 0;
}

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

    if (damageZone) {
        damageZone->SetDamageLevel(static_cast<unsigned short>(damageLevel));

        for (int slotIx = 0; slotIx < damageZone->GetSlotNum(); slotIx++) {
            int slotID = damageZone->GetSlotID(slotIx);
            VehicleDamagePart *damagePart = this->mDamagePartList[slotID];

            if (damagePart) {
                int partDamageLevel = UMath::Max(damageLevel, static_cast<int>(damagePart->GetDamageLevel()));
                partDamageLevel = UMath::Min(partDamageLevel, 1);
                damagePart->SetDamageLevel(static_cast<unsigned short>(partDamageLevel));

                if (this->mCarRenderInfo->pRideInfo) {
                    CarPart *newCarPart = damagePart->GetDamagePart(partDamageLevel);
                    this->mCarRenderInfo->pRideInfo->SetPart(slotID, newCarPart, false);
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
    if (slotId <= 0x17) {
        return mDamagePartList[slotId]->GetMatrix();
    }
    return nullptr;
}

bool VehiclePartDamageBehaviour::IsPartHidden(unsigned int slotId) {
    if (slotId <= 0x16) {
        return mDamagePartList[slotId]->IsHidden();
    }
    return true;
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

    for (windowIx = 0; windowIx <= 4; windowIx++) {
        const BreakableWindowInfoDataType &windowInfo = mBreakableWindowInfoList[windowIx];
        VehicleDamagePart *damagePart = this->mDamagePartList[windowInfo.mPartSlotId];
        int damageState = bMin(1, static_cast<int>(*reinterpret_cast<unsigned short *>(damagePart)));

        if (this->mCarRenderInfo != 0) {
            if (damageState > 0) {
                unsigned int damageHash = 0x0A155545;
                this->mCarRenderInfo->MasterReplacementTextureTable[windowInfo.mReplaceTexId]
                    .SetNewNameHash(damageHash);
                this->mCarRenderInfo->MasterReplacementTextureTable[CarRenderInfo::REPLACETEX_WINDOW_REAR_DEFOST]
                    .SetNewNameHash(damageHash);
            } else {
                this->mCarRenderInfo->MasterReplacementTextureTable[windowInfo.mReplaceTexId]
                    .SetNewNameHash(windowInfo.mOriginalHash);
            }
        }
    }
}

void VehiclePartDamageBehaviour::InitAnimationPivot(unsigned int slotId, const char * markerName) {
    if (this->FindPositionMarker(markerName) != nullptr) {
        this->mDamagePartList[slotId]->SetPivot(0.0f, 0.0f, 0.0f);
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
