#include "Speed/Indep/Src/World/Interfaces/IVehicleDamageBehaviour.h"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *VehicleDamagePartSlotPool;
extern SlotPool *VehiclePartDamageZoneSlotPool;

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
