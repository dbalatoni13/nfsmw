#include "Speed/Indep/Src/Gameplay/GTrigger.h"

#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MTriggerEnter.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerExit.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerInside.h"
#include "Speed/Indep/Src/Gameplay/GIcon.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

#include <algorithm>

GTrigger::GTrigger(const Attrib::Key &triggerKey)
    : GRuntimeInstance(triggerKey, kGameplayObjType_Trigger), //
      mWorldTrigger() {
    const UMath::Vector3 &pos = Position(0);
    UMath::Vector3 dim;
    const UMath::Vector3 *dimPtr = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x6D9E21AD, 0));
    bool hasDimensions = dimPtr != nullptr;
    UMath::Vector3 posSwizzled = UMath::Vector3Make(-pos.y, pos.z, pos.x);
    UMath::Vector3 dimSwizzled;
    UMath::Matrix4 mat;
    float radius;
    EventStaticData *pTriggerData = &mEventStaticData;
    bool showIconBasedOnBin = true;
    GIcon::Type iconType = GIcon::kType_Invalid;

    if (!dimPtr) {
        dimPtr = reinterpret_cast<const UMath::Vector3 *>(Attrib::DefaultDataArea(sizeof(UMath::Vector3)));
    }

    dim = *dimPtr;
    dimSwizzled = UMath::Vector3Make(dim.x, dim.z, dim.y);
    mTriggerEnabled = 0;
    mIcon = nullptr;
    mEnabled = false;
    mActivationReferences = 0;
    bMemSet(mParticleEffect, 0, 8);
    mSimObjInside.reserve(8);

    {
        UMath::Matrix4 rotMat = UMath::Matrix4::kIdentity;
        UMath::Vector3 initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);

        MATRIX4_multyrot(&rotMat, -Rotation(0) * 0.00069444446f, &rotMat);
        VU0_MATRIX3x4_vect3mult(initialVec, rotMat, mDirection);
        mat = rotMat;
    }

    UMath::Set(mat, 3, UMath::Vector4Make(posSwizzled, 1.0f));

    if (hasDimensions) {
        new (&mWorldTrigger) WTrigger(mat, dimSwizzled, &mEventList, OneShot(0) ? 2 : 0);
    } else {
        const float *radiusPtr = reinterpret_cast<const float *>(GetAttributePointer(0x39BF8002, 0));

        if (radiusPtr) {
            radius = *radiusPtr;
        } else {
            float width = Width(0);

            radius = UMath::Sqrt(width * width + 1.0f);
        }

        new (&mWorldTrigger) WTrigger(mat, radius, radius * 2.0f, &mEventList, OneShot(0) ? 2 : 0);
    }

    mEventList.fNumEvents = 1;
    mEventList.fPad[0] = 0;
    mEventList.fPad[1] = 0;
    mEventList.fPad[2] = 0;
    pTriggerData->fEventID = 0xC34649C0u;
    pTriggerData->fEventSize = 8;
    pTriggerData->fDataOffset = 0x10;
    pTriggerData->fPad = 0;
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
        mIcon->Show();
        mIcon->ShowOnMap();
    }
}

GTrigger::~GTrigger() {
    ClearParticleEffects();
    if (mIcon) {
        GManager::Get().FreeIcon(mIcon);
        mIcon = nullptr;
    }
}

GActivity *GTrigger::GetTargetActivity() {
    unsigned int targetActivityKey = 0x277566F3;
    return reinterpret_cast<GActivity *>(GetConnectedInstance(targetActivityKey, 0));
}

void GTrigger::AddActivationReference() {
    mActivationReferences++;
    if (!mEnabled) {
        Enable(true);
    }
}

void GTrigger::RemoveActivationReference() {
    if (mActivationReferences > 0) {
        mActivationReferences--;
    }
    if (mActivationReferences <= 0) {
        Enable(false);
    }
}

EmitterGroup *GTrigger::CreateParticleEffect(const char *effectName, UMath::Vector3 &pos) {
    Attrib::StringKey effectKey(effectName);
    EmitterGroup *group = gEmitterSystem.CreateEmitterGroup(effectKey, 0x8040000);

    if (group) {
        bMatrix4 localWorld;
        bVector3 translation(pos.z, pos.y, -pos.x);

        eCreateTranslationMatrix(&localWorld, translation);
        group->SetLocalWorld(&localWorld);
        group->SetAutoUpdate(true);
        group->SubscribeToDeletion(this, NotifyEmitterGroupDelete);
        group->Disable();
        gEmitterSystem.AddEmitterGroup(group);
    }

    return group;
}

void GTrigger::CreateAllParticleEffects() {
    const char *effectName = ParticleEffect(0);

    if (effectName && *effectName) {
        UMath::Vector3 pos;
        float flareSpacing = FlareSpacing(0);

        GetPosition(pos);
        if (flareSpacing <= 0.0f) {
            mParticleEffect[0] = CreateParticleEffect(effectName, pos);
        } else {
            bVector3 triggerPos(pos.x, pos.y, pos.z);
            bVector3 up(0.0f, 1.0f, 0.0f);
            bVector3 direction(mDirection.x, mDirection.y, mDirection.z);
            bVector3 offset;
            bVector3 leftOffset;
            bVector3 rightOffset;

            bCross(&offset, &up, &direction);
            offset *= flareSpacing;
            leftOffset = bScaleAdd(triggerPos, offset, -1.0f);
            rightOffset = bScaleAdd(triggerPos, offset, 1.0f);
            UMath::Vector3 leftPos = UMath::Vector3Make(leftOffset.x, leftOffset.y, leftOffset.z);
            UMath::Vector3 rightPos = UMath::Vector3Make(rightOffset.x, rightOffset.y, rightOffset.z);
            mParticleEffect[0] = CreateParticleEffect(effectName, leftPos);
            mParticleEffect[1] = CreateParticleEffect(effectName, rightPos);
        }
    }
}

void GTrigger::ClearParticleEffects() {
    for (int i = 0; i < 2; i++) {
        if (mParticleEffect[i]) {
            delete mParticleEffect[i];
            mParticleEffect[i] = nullptr;
        }
    }
}

void GTrigger::EnableParticleEffects(bool enabled) {
    for (int i = 0; i < 2; i++) {
        if (mParticleEffect[i]) {
            if (enabled) {
                mParticleEffect[i]->Enable();
            } else {
                mParticleEffect[i]->Disable();
            }
        }
    }
}

void GTrigger::RefreshParticleEffects() {
    ClearParticleEffects();
    CreateAllParticleEffects();
    EnableParticleEffects(mEnabled);
}

void GTrigger::NotifyEmitterGroupDelete(void *obj, EmitterGroup *group) {
    GTrigger *trigger = reinterpret_cast<GTrigger *>(obj);
    EmitterGroup **particleEffect = trigger->mParticleEffect;
    int i = 0;

    do {
        if (*(particleEffect + i) == group) {
            *(particleEffect + i) = nullptr;
        }
        i++;
    } while (i <= 1);
}

void GTrigger::Enable(bool setEnabled) {
    if (!mTriggerEnabled) {
        if (setEnabled) {
            WCollisionAssets::Get().AddTrigger(&mWorldTrigger);
            mTriggerEnabled = 1;
        }
    } else if (!setEnabled) {
        WCollisionAssets::Get().RemoveTrigger(&mWorldTrigger);
        mTriggerEnabled = 0;
    }

    if (setEnabled) {
        mWorldTrigger.Enable();
        ShowIcon();
        CreateAllParticleEffects();
    } else {
        mWorldTrigger.Disable();
        HideIcon();
        ClearParticleEffects();
    }

    mEnabled = setEnabled;
}

void GTrigger::GetPosition(UMath::Vector3 &pos) {
    pos = *reinterpret_cast<const UMath::Vector3 *>(&mWorldTrigger.fPosRadius);
}

void GTrigger::NotifySimableTrigger(ISimable *isim, int triggerStimulus) {
    if (!isim) {
        return;
    }

    bool wasInside = IsInside(isim);
    if (triggerStimulus == 2) {
        MarkAsOutside(isim);
        if (FireOnExit(0)) {
            MTriggerExit msg(GCollectionKey(this), isim->GetOwnerHandle());
            msg.Post(UCrc32(0x20D60DBF));
        }
    }

    if (triggerStimulus == 1) {
        if (!wasInside) {
            MarkAsInside(isim);
            MTriggerEnter enterMsg(GCollectionKey(this), isim->GetOwnerHandle());
            enterMsg.Post(UCrc32(0x20D60DBF));
        }
        MTriggerInside insideMsg(GCollectionKey(this), isim->GetOwnerHandle());
        insideMsg.Post(UCrc32(0x20D60DBF));
    }
}

void GTrigger::Reset() {
    mSimObjInside.clear();
    mTriggerEnabled = 0;
    mActivationReferences = 0;
    Enable(false);
}

void GTrigger::ShowIcon() {
    GIcon *icon = mIcon;

    if (icon) {
        icon->SetFlag(1);
        icon->SetFlag(2);
    }
}

void GTrigger::HideIcon() {
    GIcon *icon = mIcon;

    if (icon) {
        icon->ClearFlag(1);
        icon->ClearFlag(2);
    }
}

void GTrigger::MarkAsInside(ISimable *simable) {
    if (!IsInside(simable)) {
        mSimObjInside.push_back(simable);
    }
}

void GTrigger::MarkAsOutside(ISimable *simable) {
    UTL::Std::vector<ISimable *, _type_ID_SimObjList>::iterator it = std::find(mSimObjInside.begin(), mSimObjInside.end(), simable);

    if (it != mSimObjInside.end()) {
        mSimObjInside.erase(it);
    }
}

bool GTrigger::IsInside(ISimable *simable) {
    return std::find(mSimObjInside.begin(), mSimObjInside.end(), simable) != mSimObjInside.end();
}
