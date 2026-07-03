#include "VehiclePartDamage.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

#ifdef CLANGD_DAMNIT
#include "CarRender.cpp"
#endif

// TODO use
static const int kMaxDamagableWindows = 5;

SlotPool *VehicleDamagePartSlotPool = nullptr;
SlotPool *VehiclePartDamageZoneSlotPool = nullptr;

static const int MaximumDamageLevel = 1;

VehiclePartDamageZone::DamageZoneSlotMapDataType VehiclePartDamageBehaviour::mSlotZoneMapList[120] = {
    {0, 12}, {0, 11}, {0, 2},  {0, 10}, {0, 3},  {0, 2},  {0, 6},  {0, 7},  {0, 8},  {0, 9},  {0, 1},  {0, 17}, {0, 18}, {0, 19}, {0, 20},
    {1, 15}, {1, 16}, {1, 2},  {1, 6},  {1, 7},  {1, 8},  {1, 9},  {1, 1},  {1, 17}, {1, 18}, {1, 19}, {1, 20}, {2, 21}, {2, 22}, {2, 18},
    {2, 17}, {2, 3},  {2, 2},  {2, 6},  {2, 7},  {2, 8},  {2, 9},  {2, 1},  {2, 17}, {2, 18}, {2, 19}, {2, 20}, {3, 13}, {3, 14}, {3, 2},
    {3, 3},  {3, 6},  {3, 7},  {3, 8},  {3, 9},  {3, 1},  {3, 17}, {3, 18}, {3, 19}, {3, 20}, {4, 10}, {4, 2},  {4, 6},  {4, 7},  {4, 8},
    {4, 9},  {4, 18}, {4, 1},  {4, 17}, {4, 18}, {4, 19}, {4, 20}, {6, 15}, {6, 2},  {6, 6},  {6, 7},  {6, 8},  {6, 9},  {6, 17}, {6, 1},
    {6, 17}, {6, 18}, {6, 19}, {6, 20}, {5, 10}, {5, 2},  {5, 6},  {5, 7},  {5, 8},  {5, 9},  {5, 1},  {5, 17}, {5, 18}, {5, 19}, {5, 20},
    {7, 15}, {7, 2},  {7, 6},  {7, 7},  {7, 8},  {7, 9},  {7, 1},  {7, 17}, {7, 18}, {7, 19}, {7, 20}, {8, 11}, {8, 3},  {8, 10}, {8, 2},
    {8, 15}, {8, 13}, {8, 14}, {8, 21}, {8, 22}, {8, 6},  {8, 7},  {8, 8},  {8, 9},  {8, 1},  {8, 17}, {8, 18}, {8, 19}, {8, 20}, {65535, 65535},
};

VehiclePartDamageBehaviour::BreakableWindowInfoDataType VehiclePartDamageBehaviour::mBreakableWindowInfoList[5] = {
    {1, STRINGHASH_WINDOW_FRONT, 13, 20},        {18, STRINGHASH_WINDOW_LEFT_FRONT, 15, 22}, {17, STRINGHASH_WINDOW_LEFT_REAR, 16, 23},
    {19, STRINGHASH_WINDOW_RIGHT_FRONT, 17, 24}, {20, STRINGHASH_WINDOW_RIGHT_REAR, 18, 25},
};

VehiclePartDamageBehaviour::BreakableWindowHashDataType VehiclePartDamageBehaviour::mDamageWindowHashList[1] = {
    {STRINGHASH_WINDOW_DAMAGE0, STRINGHASH_WINDOW_DAMAGE0}};

VehiclePartDamageBehaviour::VehiclePartDamageBehaviour(CarRenderInfo *carRenderInfo)
    : mCarRenderInfo(carRenderInfo), //
      mDamageZoneNum(0),             //
      mTrunkVel(0.0f),               //
      mTrunkAngle(0.0f) {
    int partIndex = 0;
    for (unsigned int slotIx = 0; slotIx < NUM_ELEMENTS(this->mDamagePartList); slotIx++) {
        this->mDamagePartList[partIndex] = new VehicleDamagePart(carRenderInfo, slotIx);
        partIndex++;
    }

    for (int zoneId = 0; zoneId < NUM_ELEMENTS(this->mDamageZoneList); zoneId++) {
        this->mDamageZoneList[this->mDamageZoneNum++] = new VehiclePartDamageZone(zoneId, this->mSlotZoneMapList);
    }
}

VehiclePartDamageBehaviour::~VehiclePartDamageBehaviour() {
    for (unsigned int slotIx = 0; slotIx < NUM_ELEMENTS(this->mDamagePartList); slotIx++) {
        if (this->mDamagePartList[slotIx] != nullptr) {
            delete this->mDamagePartList[slotIx];
            this->mDamagePartList[slotIx] = nullptr;
        }
    }

    for (unsigned int ix = 0; ix < this->mDamageZoneNum; ix++) {
        if (this->mDamageZoneList[ix] != nullptr) {
            delete this->mDamageZoneList[ix];
            this->mDamageZoneList[ix] = nullptr;
        }
    }
}

void VehiclePartDamageBehaviour::Init() {
    InitAnimationPivot(CARSLOTID_DAMAGE_HOOD, "HOOD");
    InitAnimationPivot(CARSLOTID_DAMAGE_TRUNK, "TRUNK");
    InitAnimationPivot(CARSLOTID_DAMAGE_LEFT_DOOR, "LEFT_DOOR");
    InitAnimationPivot(CARSLOTID_DAMAGE_RIGHT_DOOR, "RIGHT_DOOR");
    InitAnimationPivot(CARSLOTID_DAMAGE_LEFT_REAR_DOOR, "LEFT_REAR_DOOR");
    InitAnimationPivot(CARSLOTID_DAMAGE_RIGHT_REAR_DOOR, "RIGHT_REAR_DOOR");
    InitAnimationPivot(CARSLOTID_DAMAGE_REAR_BUMPER, "REAR_BUMPER");

    VehicleDamagePart *damagePart = this->mDamagePartList[CARSLOTID_DAMAGE_TRUNK];
    damagePart->SetPivot(1.73f, -0.96f, 0.0f);

    damagePart = this->mDamagePartList[CARSLOTID_DAMAGE_HOOD];
    damagePart->SetPivot(-1.13f, -0.86f, 0.0f);
}

void VehiclePartDamageBehaviour::InitAnimationPivot(unsigned int slotId, const char *markerName) {
    ePositionMarker *positionMarker = this->FindPositionMarker(markerName);
    if (positionMarker != nullptr) {
        VehicleDamagePart *damagePart = this->mDamagePartList[slotId];
        damagePart->SetPivot(positionMarker->Matrix.v3.x, positionMarker->Matrix.v3.y, positionMarker->Matrix.v3.z);
        damagePart->SetPivot(0.0f, 0.0f, 0.0f);
    }
}

void VehiclePartDamageBehaviour::Reset() {
    for (unsigned int slotIx = 0; slotIx < NUM_ELEMENTS(this->mDamagePartList); slotIx++) {
        VehicleDamagePart *damagePart = this->mDamagePartList[slotIx];
        damagePart->Reset();
    }

    for (unsigned int zoneIx = 0; zoneIx < this->mDamageZoneNum; zoneIx++) {
        VehiclePartDamageZone *damageZone = this->mDamageZoneList[zoneIx];
        damageZone->Reset();
        this->DamageZone(static_cast<int>(zoneIx), 0);
    }

    this->ApplyDamage();
}

ePositionMarker *VehiclePartDamageBehaviour::FindPositionMarker(const char *findPivotName) {
    CarPartModel *carPart;
    unsigned int findNameHash;
    this->mCarRenderInfo->GetMinLodLevel();
    eModel *model = carPart->GetModel();
    ePositionMarker *positionMarker;

    return nullptr;
}

void VehiclePartDamageBehaviour::DamageZone(int zone, int damageLevel) {
    VehiclePartDamageZone *damageZone = this->mDamageZoneList[zone];

    if (damageZone != nullptr) {
        damageZone->SetDamageLevel(static_cast<unsigned short>(damageLevel));

        for (int slotIx = 0; slotIx < damageZone->GetSlotNum(); slotIx++) {
            int slotID = damageZone->GetSlotID(slotIx);
            int partIx;
            VehicleDamagePart *damagePart = this->mDamagePartList[slotID];

            if (damagePart != nullptr) {
                int partDamageLevel = UMath::Min(MaximumDamageLevel, UMath::Max(damageLevel, static_cast<int>(damagePart->GetDamageLevel())));
                damagePart->SetDamageLevel(static_cast<unsigned short>(partDamageLevel));

                CarPart *newCarPart = damagePart->GetDamagePart(partDamageLevel);
                RideInfo *rideInfo = this->mCarRenderInfo->GetRideInfo();
                if (rideInfo != nullptr) {
                    rideInfo->SetPart(slotID, newCarPart, false);
                }

                this->ManageGlassDamage();
            }
        }
    }
}

void VehiclePartDamageBehaviour::ApplyDamage() {
    if (this->mCarRenderInfo != nullptr) {
        RideInfo *rideInfo = this->mCarRenderInfo->GetRideInfo();
        if (rideInfo != nullptr) {
            rideInfo->UpdatePartsEnabled();
        }

        this->mCarRenderInfo->UpdateCarParts();
    }
}

static unsigned int testDamageLevel = 0;
static unsigned int testDelay = 10;

void VehiclePartDamageBehaviour::ManageGlassDamage() {
    for (int windowIx = 0; windowIx <= 4; windowIx++) {
        const BreakableWindowInfoDataType &windowInfo = this->mBreakableWindowInfoList[windowIx];
        VehicleDamagePart *damagePart = this->mDamagePartList[windowInfo.mPartSlotId];
        int damageLevel = UMath::Min(MaximumDamageLevel, static_cast<int>(damagePart->GetDamageLevel()));
        const BreakableWindowHashDataType &windowDamageHashInfo = VehiclePartDamageBehaviour::mDamageWindowHashList[damageLevel];

        if (this->mCarRenderInfo != nullptr) {
            if (damageLevel > 0) {
                unsigned int damageHash = STRINGHASH_WINDOW_DAMAGE0;
                this->mCarRenderInfo->MasterReplacementTextureTable[windowInfo.mReplaceTexId].SetNewNameHash(damageHash);
                this->mCarRenderInfo->MasterReplacementTextureTable[CarRenderInfo::REPLACETEX_WINDOW_REAR_DEFOST].SetNewNameHash(damageHash);
            } else {
                this->mCarRenderInfo->MasterReplacementTextureTable[windowInfo.mReplaceTexId].SetNewNameHash(windowInfo.mOriginalHash);
            }
        }
    }
}

bMatrix4 *VehiclePartDamageBehaviour::GetPartMatrix(unsigned int slotId) {
    if (slotId <= NUM_ELEMENTS(this->mDamagePartList)) {
        return this->mDamagePartList[slotId]->GetMatrix();
    }
    return nullptr;
}

bool VehiclePartDamageBehaviour::IsPartHidden(unsigned int slotId) {
    if (slotId < NUM_ELEMENTS(this->mDamagePartList)) {
        return this->mDamagePartList[slotId]->IsHidden();
    }
    return true;
}

void VehiclePartDamageBehaviour::HidePart(unsigned int slotId) {
    if (slotId > NUM_ELEMENTS(this->mDamagePartList)) {
        return;
    }

    this->mDamagePartList[slotId]->Hide();
}

void VehiclePartDamageBehaviour::Pose(bMatrix4 *worldMatrix) {
    for (unsigned int partIx = 0; partIx < NUM_ELEMENTS(this->mDamagePartList); partIx++) {
        this->mDamagePartList[partIx]->SetMatrix(worldMatrix);
    }
}

void VehiclePartDamageBehaviour::Update(bMatrix4 *worldMatrix) {
    this->Pose(worldMatrix);

    if (this->mDamagePartList[15]->GetDamageLevel() == 1) {
        float angle = this->CalcPartRotation(worldMatrix, DEG2ANGLE(10.0f), 2.4f, 0.2f, 80.0f) * 360.0f;
        const bVector3 trunkRotation(0.0f, angle, 0.0f);

        this->AnimatePart(15, trunkRotation, worldMatrix);
    }

    if (this->mDamagePartList[10]->GetDamageLevel() == 1) {
        float angle = -(this->CalcPartRotation(worldMatrix, DEG2ANGLE(10.0f), 2.4f, 0.2f, 130.0f) * 360.0f);
        const bVector3 hoodRotation(0.0f, angle, 0.0f);

        this->AnimatePart(10, hoodRotation, worldMatrix);
    }

    if (this->mDamagePartList[16]->GetDamageLevel() == 1) {
        float angle = this->CalcPartRotation(worldMatrix, DEG2ANGLE(10.0f), 2.4f, 0.2f, 80.0f) * 360.0f;
        const bVector3 trunkRotation(angle, 0.0f, 0.0f);
        // this->AnimatePart(10, trunkRotation, worldMatrix);
    }
}

// STRIPPED
void VehiclePartDamageBehaviour::ManageHoodAnimation() {}

float VehiclePartDamageBehaviour::CalcPartRotation(bMatrix4 *worldMatrix, float maxAngle, float spring, float damper, float inertia) {
    bMatrix4 localInverse(*worldMatrix);
    bMatrix4 localMatrix;

    bInvertMatrix(&localInverse, &localInverse);
    eMulMatrix(&localMatrix, worldMatrix, &localInverse);

    this->angularVel_ch = this->mCarRenderInfo->mAngularVelocity;
    this->linearVel_ch = this->mCarRenderInfo->mVelocity;
    this->linearAcc_ch = this->mCarRenderInfo->mAcceleration;

    eMulVector(&this->angularVel_ch, &localMatrix, &this->mCarRenderInfo->mAngularVelocity);
    eMulVector(&this->linearVel_ch, &localMatrix, &this->mCarRenderInfo->mVelocity);
    eMulVector(&this->linearAcc_ch, &localMatrix, &this->mCarRenderInfo->mAcceleration);

    this->angularVel_ch.y /= (float)M_TWOPI;
    this->angularVel_ch.x /= (float)M_TWOPI;
    this->angularVel_ch.z /= (float)M_TWOPI;

    if (maxAngle <= 0.0f) {
        this->mTrunkAngle = 0.0f;
    } else {
        float dT = this->mCarRenderInfo->GetDeltaTime();
        float desttheta = -UMath::Min(worldMatrix->v2.z * 0.25f, 0.0f);
        static float tCs = spring;
        static float tCd = damper;
        static float tI = inertia;
        float acc = tCs * (desttheta - this->mTrunkAngle) + tCd * -this->mTrunkVel;

        if (UMath::Abs(this->linearAcc_ch.x) > 0.1f) {
            acc -= this->linearAcc_ch.x / tI;
        }

        if (UMath::Abs(this->linearVel_ch.z) > 0.35f) {
            acc += (-this->linearVel_ch.z * 3.0f) / tI;
        }

        if (UMath::Abs(-this->angularVel_ch.y) > 0.01f) {
            acc += -this->angularVel_ch.y * 36.0f;
        }

        this->mTrunkVel = acc * dT + this->mTrunkVel;
        this->mTrunkAngle = this->mTrunkVel * dT + this->mTrunkAngle;

        if (this->mTrunkAngle < 0.0f) {
            this->mTrunkAngle = 0.0f;
            this->mTrunkVel = -this->mTrunkVel * 0.2f;
        } else if (this->mTrunkAngle > maxAngle) {
            this->mTrunkAngle = maxAngle;
            this->mTrunkVel = -this->mTrunkVel * 0.5f;
        }
    }

    return this->mTrunkAngle;
}

// STRIPPED
float VehiclePartDamageBehaviour::ManageTrunkAnimation(bMatrix4 *worldMatrix) {}

// STRIPPED
void VehiclePartDamageBehaviour::ManageDoorAnimation() {}

void VehiclePartDamageBehaviour::AnimatePart(unsigned int slotId, const bVector3 &rotation, bMatrix4 *worldMatrix) {
    this->Init();

    if (slotId <= NUM_ELEMENTS(this->mDamagePartList)) {
        VehicleDamagePart *damagePart = this->mDamagePartList[slotId];
        for (int lodIx = this->mCarRenderInfo->GetMinLodLevel(); lodIx <= this->mCarRenderInfo->GetMaxLodLevel(); lodIx++) {
            CarPartModel *carPartModel = &this->mCarRenderInfo->mCarPartModels[slotId][0][lodIx];

            if (carPartModel->IsHidden()) {
                break;
            }

            if (damagePart != nullptr) {
                bMatrix4 *partMatrix = damagePart->GetMatrix();
                bMatrix4 localInverse(*worldMatrix);
                bInvertMatrix(&localInverse, &localInverse);
                eMulMatrix(partMatrix, worldMatrix, &localInverse);

                const UMath::Vector3 pivot(damagePart->GetPivot());
                bVector3 model_pivot_translate(pivot.x, pivot.y, pivot.z);
                eTranslate(partMatrix, partMatrix, &model_pivot_translate);
                eRotateX(partMatrix, partMatrix, bDegToAng(rotation.x));
                eRotateY(partMatrix, partMatrix, bDegToAng(rotation.y));
                eRotateZ(partMatrix, partMatrix, bDegToAng(rotation.z));

                bVector3 pviot_model_translate(-pivot.x, -pivot.y, -pivot.z);
                eTranslate(partMatrix, partMatrix, &pviot_model_translate);
                eMulMatrix(partMatrix, partMatrix, worldMatrix);
            }
        }
    }
}

void VehiclePartDamageBehaviour::DamageVehicle(const DamageZone::Info &damageInfo) {
    for (unsigned int zoneIx = 0; zoneIx < this->mDamageZoneNum; zoneIx++) {
        this->DamageZone(static_cast<int>(zoneIx), static_cast<int>(damageInfo.Get(static_cast<DamageZone::ID>(zoneIx))));
    }

    this->ApplyDamage();
}

unsigned int unitTestDelay = 400;
unsigned int uniTestLevel = 0;

void VehiclePartDamageBehaviour::UnitTest() {
    unitTestDelay--;

    if (unitTestDelay == 0) {
        unitTestDelay = 30;
        uniTestLevel++;
        if (uniTestLevel > 1) {
            uniTestLevel = 0;
        }

        for (unsigned int zoneIx = 0; zoneIx < this->mDamageZoneNum; zoneIx++) {
            this->DamageZone(static_cast<int>(zoneIx), static_cast<int>(uniTestLevel));
        }

        this->ApplyDamage();
    }
}

VehiclePartDamageZone::VehiclePartDamageZone(int zoneId, DamageZoneSlotMapDataType *zoneSlotMappingDataList)
    : mZoneId(zoneId), //
      mDamageLevel(0) {
    this->mSlotIdList.reserve(20);

    if (zoneSlotMappingDataList != nullptr) {
        int dataZoneMapId;
        int zoneMapIx = 0;

        do {
            dataZoneMapId = zoneSlotMappingDataList[zoneMapIx].mZoneId;

            if (dataZoneMapId == this->mZoneId) {
                int slotMapId = zoneSlotMappingDataList[zoneMapIx].mSlotId;
                this->mSlotIdList.push_back(slotMapId);
            }

            zoneMapIx++;
        } while (dataZoneMapId != 0xffff);
    }
}

void VehiclePartDamageZone::Reset() {
    mDamageLevel = 0;
}

int VehiclePartDamageZone::GetSlotNum() const {
    return this->mSlotIdList.size();
}

int VehiclePartDamageZone::GetSlotID(int index) const {
    return this->mSlotIdList[index];
}

VehiclePartDamageZone::~VehiclePartDamageZone() {
    this->mSlotIdList.clear();
}

void VehiclePartDamageZone::SetDamageLevel(unsigned short damageLevel) {
    this->mDamageLevel = damageLevel;
}

VehicleDamagePart::VehicleDamagePart(CarRenderInfo *carRenderInfo, int slotId) {
    this->mDamageLevel = 0;
    this->mSlotId = slotId;
    this->mAnimatable = true;
    this->mHidden = false;
    this->mPivot.x = 0.0f;
    this->mPivot.y = 0.0f;
    this->mPivot.z = 0.0f;
    eIdentity(&this->mMatrix);

    if (carRenderInfo != nullptr) {
        RideInfo *rideInfo = carRenderInfo->GetRideInfo();

        if (rideInfo != nullptr) {
            this->mDamageParts[0] = rideInfo->GetPart(this->mSlotId);

            for (int ix = 1; ix < NUM_ELEMENTS(this->mDamageParts); ix++) {
                if (this->mDamageParts[ix - 1] != nullptr) {
                    this->mDamageParts[ix] = CarPartDB.NewGetNextCarPart(this->mDamageParts[ix - 1], rideInfo->Type, this->mSlotId, 0, -1);
                }
            }
        }
    }
}

VehicleDamagePart::~VehicleDamagePart() {
    for (int ix = 0; ix < NUM_ELEMENTS(this->mDamageParts); ix++) {
        this->mDamageParts[ix] = nullptr;
    }
}

void VehicleDamagePart::Reset() {
    mDamageLevel = 0;
    mHidden = false;
    mAnimatable = true;
}

void InitVehicleDamage() {
    VehicleDamagePartSlotPool = bNewSlotPool(sizeof(VehicleDamagePart), 240, "VehicleDamagePartSlotPool", 0);
    VehiclePartDamageZoneSlotPool = bNewSlotPool(sizeof(VehiclePartDamageZone), 100, "VehiclePartDamageZoneSlotPool", 0);
}

void CloseVehicleDamage() {}
