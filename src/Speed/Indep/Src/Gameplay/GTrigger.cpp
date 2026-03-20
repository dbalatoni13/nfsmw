#include "Speed/Indep/Src/Gameplay/GTrigger.h"

#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
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
    UMath::Matrix4 directionMat;
    UMath::Matrix4 mat;
    UMath::Vector3 initialVec;
    const float *rotation;
    const UMath::Vector3 *position;
    const UMath::Vector3 *dimensions;
    const float *radius;
    UMath::Vector3 posSwizzled;
    UMath::Vector3 dimSwizzled;
    float triggerRadius;
    EventStaticData *pTriggerData = &mEventStaticData;
    bool showIconBasedOnBin = true;
    const int *oneShot;
    bool hasDimensions;

    mTriggerEnabled = 0;
    mIcon = nullptr;
    mEnabled = false;
    mActivationReferences = 0;
    bMemSet(mParticleEffect, 0, 8);

    UMath::Copy(UMath::Matrix4::kIdentity, directionMat);
    initialVec.x = 0.0f;
    initialVec.y = 0.0f;
    initialVec.z = 1.0f;

    rotation = reinterpret_cast<const float *>(GetAttributePointer(0x5A6A57C6, 0));
    if (!rotation) {
        rotation = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    MATRIX4_multyrot(&directionMat, -*rotation * 0.00069444446f, &directionMat);
    VU0_MATRIX3x4_vect3mult(initialVec, directionMat, initialVec);
    mDirection = initialVec;
    mSimObjInside.reserve(8);

    position = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x9F743A0E, 0));
    if (!position) {
        position = reinterpret_cast<const UMath::Vector3 *>(Attrib::DefaultDataArea(sizeof(UMath::Vector3)));
    }

    dimensions = reinterpret_cast<const UMath::Vector3 *>(GetAttributePointer(0x6D9E21AD, 0));
    posSwizzled.x = -position->y;
    posSwizzled.y = position->z;
    posSwizzled.z = position->x;
    dimSwizzled.x = 0.0f;
    dimSwizzled.y = 0.0f;
    dimSwizzled.z = 0.0f;
    hasDimensions = false;
    triggerRadius = 0.0f;

    if (dimensions) {
        dimSwizzled.x = dimensions->y;
        dimSwizzled.y = dimensions->z;
        dimSwizzled.z = dimensions->x;
        hasDimensions = true;
    }

    UMath::Copy(UMath::Matrix4::kIdentity, mat);

    radius = reinterpret_cast<const float *>(GetAttributePointer(0x39BF8002, 0));
    if (radius) {
        triggerRadius = *radius;
        dimSwizzled.x = triggerRadius + triggerRadius;
        dimSwizzled.y = dimSwizzled.x;
        dimSwizzled.z = dimSwizzled.x;
        mWorldTrigger.fShape = 3;
    } else if (hasDimensions) {
        triggerRadius = UMath::Sqrt(dimSwizzled.x * dimSwizzled.x + dimSwizzled.z * dimSwizzled.z);
        mWorldTrigger.fShape = 1;
    } else {
        const float *width = reinterpret_cast<const float *>(GetAttributePointer(0x5816C1FC, 0));

        if (!width) {
            width = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
        }

        dimSwizzled.x = *width;
        dimSwizzled.y = 1.0f;
        dimSwizzled.z = 0.0f;
        triggerRadius = UMath::Sqrt(dimSwizzled.x * dimSwizzled.x + 1.0f);
        mWorldTrigger.fShape = 1;
    }

    rotation = reinterpret_cast<const float *>(GetAttributePointer(0x5A6A57C6, 0));
    if (!rotation) {
        rotation = reinterpret_cast<const float *>(Attrib::DefaultDataArea(sizeof(float)));
    }

    MATRIX4_multyrot(&mat, -*rotation * 0.00069444446f, &mat);
    mWorldTrigger.fType = 1;
    mWorldTrigger.fEvents = &mEventList;
    mWorldTrigger.fIterStamp = 0;
    mWorldTrigger.fFingerprint = 0;
    mWorldTrigger.fMatRow0Width.x = mat[0][0];
    mWorldTrigger.fMatRow0Width.y = mat[0][1];
    mWorldTrigger.fMatRow0Width.z = mat[0][2];
    mWorldTrigger.fMatRow0Width.w = dimSwizzled.x;
    mWorldTrigger.fHeight = dimSwizzled.y + dimSwizzled.y;
    mWorldTrigger.fMatRow2Length.x = mat[2][0];
    mWorldTrigger.fMatRow2Length.y = mat[2][1];
    mWorldTrigger.fMatRow2Length.z = mat[2][2];
    mWorldTrigger.fMatRow2Length.w = dimSwizzled.z;
    mWorldTrigger.fPosRadius.x = posSwizzled.x;
    mWorldTrigger.fPosRadius.y = posSwizzled.y;
    mWorldTrigger.fPosRadius.z = posSwizzled.z;
    mWorldTrigger.fPosRadius.w = triggerRadius;
    mWorldTrigger.fFlags = 0x4810D;

    oneShot = reinterpret_cast<const int *>(GetAttributePointer(0xCE4261AC, 0));
    if (!oneShot) {
        oneShot = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    if (*oneShot) {
        mWorldTrigger.fFlags |= 2;
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
        SetFlag(0x200);
        const GCollectionKey &targetActivityKey = TargetActivity();
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromKey(targetActivityKey.GetCollectionKey());
        if (parms) {
            GIcon::Type iconType = GIcon::kType_Invalid;

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
        mIcon = GManager::Get().AllocIcon(GIcon::kType_GateCarLot, posSwizzled, 0.0f, false);
    } else if (IsDerivedFromTemplate(0x4698966B)) {
        SetFlag(0x4000);
        mIcon = GManager::Get().AllocIcon(GIcon::kType_GateCustomShop, posSwizzled, 0.0f, false);
    } else if (IsDerivedFromTemplate(0x326427BB)) {
        SetFlag(0x8000);
        mIcon = GManager::Get().AllocIcon(GIcon::kType_GateSafehouse, posSwizzled, 0.0f, false);
    } else if (IsDerivedFromTemplate(0xB05871D3)) {
        SetFlag(0x100);
        if (OpenWorldSpeedTrap()) {
            mIcon = GManager::Get().AllocIcon(GIcon::kType_SpeedTrap, posSwizzled, 0.0f, false);
        } else {
            showIconBasedOnBin = false;
            mIcon = GManager::Get().AllocIcon(GIcon::kType_SpeedTrapInRace, posSwizzled, 0.0f, false);
        }
    } else if (IsDerivedFromTemplate(0x45E28759)) {
        SetFlag(0x400);
    } else if (IsDerivedFromTemplate(0x76720F56)) {
        SetFlag(0x800);
    } else if (IsDerivedFromTemplate(0xA3E939E9)) {
        SetFlag(0x1000);
    } else if (IsDerivedFromTemplate(0x98217DBF)) {
        mIcon = GManager::Get().AllocIcon(GIcon::kType_AreaUnlock, posSwizzled, 0.0f, false);
    }

    if (showIconBasedOnBin && mIcon) {
        int binIndex = BinIndex();

        if (binIndex > 0 && FEDatabase && FEDatabase->GetCareerSettings()->GetCurrentBin() <= BinIndex()) {
            mIcon->Show();
            mIcon->ShowOnMap();
        }
    }
}

GTrigger::~GTrigger() {
    Disable();

    for (unsigned int onEffect = 0; onEffect < 2; onEffect++) {
        if (mParticleEffect[onEffect]) {
            mParticleEffect[onEffect]->UnSubscribe();
            if (mParticleEffect[onEffect]) {
                delete mParticleEffect[onEffect];
            }
        }
    }

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
    EmitterGroup *effect = gEmitterSystem.CreateEmitterGroup(Attrib::StringKey(effectName), 0x8040000);

    if (effect) {
        bMatrix4 mat;
        bVector3 posSwizzled;

        eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pos), posSwizzled);
        eCreateTranslationMatrix(&mat, posSwizzled);
        effect->SetLocalWorld(&mat);
        effect->SetAutoUpdate(true);
        effect->SubscribeToDeletion(this, NotifyEmitterGroupDelete);
        effect->Disable();
        gEmitterSystem.AddEmitterGroup(effect);
    }

    return effect;
}

void GTrigger::CreateAllParticleEffects() {
    const char *effectName = ParticleEffect(0);

    if (effectName && *effectName) {
        UMath::Vector3 pos;
        float flareSpacing;

        GetPosition(pos);
        flareSpacing = FlareSpacing(0);
        if (flareSpacing > 0.0f) {
            const UMath::Vector3 upVec = {0.0f, 1.0f, 0.0f};
            UMath::Vector3 lateralVec;
            UMath::Vector3 posLeft;
            UMath::Vector3 posRight;

            bCross(reinterpret_cast<bVector3 *>(&lateralVec), reinterpret_cast<const bVector3 *>(&upVec), reinterpret_cast<const bVector3 *>(&mDirection));
            bScale(reinterpret_cast<bVector3 *>(&lateralVec), reinterpret_cast<const bVector3 *>(&lateralVec), flareSpacing);
            bScaleAdd(reinterpret_cast<bVector3 *>(&posLeft), reinterpret_cast<const bVector3 *>(&pos), reinterpret_cast<const bVector3 *>(&lateralVec), -1.0f);
            bScaleAdd(reinterpret_cast<bVector3 *>(&posRight), reinterpret_cast<const bVector3 *>(&pos), reinterpret_cast<const bVector3 *>(&lateralVec), 1.0f);
            mParticleEffect[0] = CreateParticleEffect(effectName, posLeft);
            mParticleEffect[1] = CreateParticleEffect(effectName, posRight);
        } else {
            mParticleEffect[0] = CreateParticleEffect(effectName, pos);
        }
    }
}

void GTrigger::ClearParticleEffects() {
    for (unsigned int onEffect = 0; onEffect < 2; onEffect++) {
        if (mParticleEffect[onEffect]) {
            mParticleEffect[onEffect]->UnSubscribe();
            if (mParticleEffect[onEffect]) {
                delete mParticleEffect[onEffect];
            }
        }
    }

    bMemSet(mParticleEffect, 0, sizeof(mParticleEffect));
}

void GTrigger::EnableParticleEffects(bool enabled) {
    for (unsigned int onEffect = 0; onEffect < 2; onEffect++) {
        if (mParticleEffect[onEffect]) {
            if (enabled) {
                mParticleEffect[onEffect]->Enable();
            } else {
                mParticleEffect[onEffect]->Disable();
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
    unsigned int i = 0;

    do {
        if (trigger->mParticleEffect[i] == group) {
            trigger->mParticleEffect[i] = nullptr;
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

    {
        WTrigger *trigger = &mWorldTrigger;
        unsigned char *triggerBytes = reinterpret_cast<unsigned char *>(trigger);
        unsigned int word = *reinterpret_cast<unsigned int *>(triggerBytes + 0x10);
        unsigned int flags;

        if (setEnabled) {
            flags = static_cast<unsigned int>(triggerBytes[0x13]) |
                    (static_cast<unsigned int>(triggerBytes[0x12]) << 8) |
                    (static_cast<unsigned int>(triggerBytes[0x11]) << 16) | 1;
        } else {
            flags = (static_cast<unsigned int>(triggerBytes[0x13]) & 0xFFFFFE) |
                    (static_cast<unsigned int>(triggerBytes[0x12]) << 8) |
                    (static_cast<unsigned int>(triggerBytes[0x11]) << 16);
        }

        *reinterpret_cast<unsigned int *>(triggerBytes + 0x10) = (word & 0xFF000000) | flags;
    }

    if (setEnabled) {
        CreateAllParticleEffects();
    } else {
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

    bool triggerExited = triggerStimulus == 2;
    bool triggerInside = triggerStimulus == 1;
    bool wasInside = IsInside(isim);

    if (triggerExited) {
        MarkAsOutside(isim);
        if (FireOnExit()) {
            UCrc32 triggerMessage(0x20D60DBF);
            HSIMABLE handle = isim->GetInstanceHandle();
            GCollectionKey sender(this);
            MTriggerExit msg(sender, handle);
            msg.Post(triggerMessage);
        }
    }

    if (triggerInside) {
        if (!wasInside) {
            MarkAsInside(isim);
            UCrc32 triggerMessage(0x20D60DBF);
            HSIMABLE handle = isim->GetInstanceHandle();
            GCollectionKey sender(this);
            MTriggerEnter enterMsg(sender, handle);
            enterMsg.Post(triggerMessage);
        }

        UCrc32 triggerMessage(0x20D60DBF);
        HSIMABLE handle = isim->GetInstanceHandle();
        GCollectionKey sender(this);
        MTriggerInside insideMsg(sender, handle);
        insideMsg.Post(triggerMessage);
    }
}

void GTrigger::Reset() {
    mSimObjInside.clear();
}

void GTrigger::ShowIcon() {
    if (mIcon) {
        mIcon->Show();
        mIcon->ShowOnMap();
    }
}

void GTrigger::HideIcon() {
    if (mIcon) {
        mIcon->Hide();
        mIcon->HideOnMap();
    }
}

void GTrigger::MarkAsInside(ISimable *simable) {
    if (std::find(mSimObjInside.begin(), mSimObjInside.end(), simable) == mSimObjInside.end()) {
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
