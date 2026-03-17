#include "Speed/Indep/Src/Gameplay/GTrigger.h"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

GTrigger::GTrigger(const Attrib::Key &triggerKey)
    : GRuntimeInstance(triggerKey, kGameplayObjType_Trigger), //
      mWorldTrigger(), //
      mDirection(UMath::Vector3Make(0.0f, 0.0f, 0.0f)), //
      mTriggerEnabled(0), //
      mIcon(nullptr), //
      mEnabled(false), //
      mActivationReferences(0) {
    const UMath::Vector3 &pos = Position(0);
    const UMath::Vector3 *dimensions = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x6D9E21AD, 0));
    const float *radiusAttr = reinterpret_cast<const float *>(GetAttributePointer(0x39BF8002, 0));
    UMath::Vector3 initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);
    UMath::Vector3 posSwizzled = UMath::Vector3Make(-pos.y, pos.z, pos.x);
    UMath::Matrix4 rotMat;
    UMath::Matrix4 triggerMat;
    float radius = radiusAttr ? *radiusAttr : 0.0f;
    bool showIconBasedOnBin = true;
    GIcon::Type iconType = GIcon::kType_Invalid;

    mParticleEffect[0] = nullptr;
    mParticleEffect[1] = nullptr;
    mSimObjInside.reserve(8);

    UMath::MultYRot(UMath::Matrix4::kIdentity, -Rotation(0) * 0.00069444446f, rotMat);
    UMath::Rotate(initialVec, rotMat, mDirection);

    triggerMat = rotMat;
    UMath::Set(triggerMat, 3, UMath::Vector4Make(posSwizzled, 1.0f));

    if (dimensions) {
        UMath::Vector3 dimSwizzled = UMath::Vector3Make(dimensions->x, dimensions->z, dimensions->y);
        mWorldTrigger = WTrigger(triggerMat, dimSwizzled, &mEventList, OneShot(0) ? 2 : 0);
    } else {
        if (!radiusAttr) {
            float width = Width(0);
            radius = UMath::Sqrt(width * width + 1.0f);
        }
        mWorldTrigger = WTrigger(triggerMat, radius, radius * 2.0f, &mEventList, OneShot(0) ? 2 : 0);
    }

    mEventList.fNumEvents = 1;
    mEventList.fPad[0] = 0;
    mEventList.fPad[1] = 0;
    mEventList.fPad[2] = 0;
    mEventStaticData.fEventID = 0xC34649C0u;
    mEventStaticData.fEventSize = 8;
    mEventStaticData.fDataOffset = 0x10;
    mEventStaticData.fPad = 0;
    bMemSet(mTriggerEventData, 0, sizeof(mTriggerEventData));
    reinterpret_cast<unsigned int *>(mTriggerEventData)[1] = GetCollection();

    if (IsDerivedFromTemplate(0xF05931AB)) {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromKey(TargetActivity(0).GetCollectionKey());

        SetFlag(0x200);
        if (parms) {
            if (parms->GetIsBossRace()) {
                iconType = GIcon::kType_RaceRival;
            } else {
                switch (parms->GetRaceType()) {
                    case GRace::kRaceType_P2P:
                        iconType = GIcon::kType_RaceSprint;
                        break;
                    case GRace::kRaceType_Circuit:
                        iconType = GIcon::kType_RaceCircuit;
                        break;
                    case GRace::kRaceType_Drag:
                        iconType = GIcon::kType_RaceDrag;
                        break;
                    case GRace::kRaceType_Knockout:
                        iconType = GIcon::kType_RaceKnockout;
                        break;
                    case GRace::kRaceType_Tollbooth:
                        iconType = GIcon::kType_RaceTollbooth;
                        break;
                    case GRace::kRaceType_SpeedTrap:
                        iconType = GIcon::kType_RaceSpeedtrap;
                        break;
                    default:
                        break;
                }
            }

            if (iconType != GIcon::kType_Invalid) {
                mIcon = GManager::Get().AllocIcon(iconType, posSwizzled, 0.0f, false);
            }
            showIconBasedOnBin = false;
        }
    } else if (IsDerivedFromTemplate(0x73049919)) {
        SetFlag(0x2000);
        iconType = GIcon::kType_GateCarLot;
    } else if (IsDerivedFromTemplate(0x4698966B)) {
        SetFlag(0x4000);
        iconType = GIcon::kType_GateCustomShop;
    } else if (IsDerivedFromTemplate(0x326427BB)) {
        SetFlag(0x8000);
        iconType = GIcon::kType_GateSafehouse;
    } else if (IsDerivedFromTemplate(0xB05871D3)) {
        SetFlag(0x100);
        if (OpenWorldSpeedTrap(0)) {
            iconType = GIcon::kType_SpeedTrap;
        } else {
            iconType = GIcon::kType_SpeedTrapInRace;
            showIconBasedOnBin = false;
        }
    } else if (IsDerivedFromTemplate(0x45E28759)) {
        SetFlag(0x400);
    } else if (IsDerivedFromTemplate(0x76720F56)) {
        SetFlag(0x800);
    } else if (IsDerivedFromTemplate(0xA3E939E9)) {
        SetFlag(0x1000);
    } else if (IsDerivedFromTemplate(0x98217DBF)) {
        iconType = GIcon::kType_AreaUnlock;
    }

    if (!mIcon && iconType != GIcon::kType_Invalid) {
        mIcon = GManager::Get().AllocIcon(iconType, posSwizzled, 0.0f, false);
    }

    if (showIconBasedOnBin && mIcon && FEDatabase && FEDatabase->GetCareerSettings()->GetCurrentBin() <= BinIndex(0)) {
        mIcon->SetFlag(1);
        mIcon->SetFlag(2);
    }
}
