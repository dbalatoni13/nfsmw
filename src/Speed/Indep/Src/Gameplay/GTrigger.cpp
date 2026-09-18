#include "GTrigger.h"

#include "GIcon.h"
#include "GManager.h"
#include "GRaceDatabase.h"
#include "GReflected.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay_hash.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerEnter.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerExit.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerInside.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

GTrigger::GTrigger(const unsigned int &triggerKey)
    : GRuntimeInstance(triggerKey, kGameplayObjType_Trigger), mWorldTrigger(), mTriggerEnabled(0) {
    mIcon = NULL;
    mEnabled = false;
    mActivationReferences = 0;
    bMemSet(mParticleEffect, 0, sizeof(mParticleEffect));

    {
        UMath::Matrix4 rotmat = UMath::Matrix4::kIdentity;
        UMath::Vector3 dir = {0.0f, 0.0f, 1.0f};
        UMath::Init(rotmat, 1.0f, 1.0f, 1.0f);
        UMath::MultYRot(rotmat, -Rotation() / 360.0f, rotmat);
        UMath::Rotate(dir, rotmat, dir);
        mDirection = dir;
    }

    mSimObjInside.reserve(8);

    const UMath::Vector3 &pos = Position();
    UMath::Vector3 dims;
    bool hasDimensions = Dimensions(dims);
    UMath::Vector3 center = UMath::Vector3Make(-pos.y, pos.z, pos.x);
    UMath::Vector3 size = UMath::Vector3Make(dims.y, dims.z, dims.x);
    UMath::Matrix4 boxmat = UMath::Matrix4::kIdentity;

    float radius = 0.0f;
    if (Radius(radius)) {
        mWorldTrigger.fShape = 3;
        size.x = size.y = size.z = radius + radius;
    } else if (hasDimensions) {
        mWorldTrigger.fShape = 1;
        float halfWidth = size.x * 0.25f;
        float halfLength = size.z * 0.25f;
        radius = UMath::Sqrt(size.x * halfWidth + size.z * halfLength);
    } else {
        size.x = Width();
        size.y = 50.0f;
        size.z = 1.0f;
        mWorldTrigger.fShape = 1;
        float halfWidth = size.x * 0.25f;
        radius = UMath::Sqrt(size.x * halfWidth + 0.25f);
    }

    UMath::Init(boxmat, 1.0f, 1.0f, 1.0f);
    UMath::MultYRot(boxmat, -Rotation() / 360.0f, boxmat);

    mWorldTrigger.fHeight = size.y + size.y;
    mWorldTrigger.fType = 1;
    mWorldTrigger.fFingerprint = 0;
    mWorldTrigger.fMatRow0Width = boxmat.v0;
    mWorldTrigger.fMatRow0Width.w = size.x;
    mWorldTrigger.fMatRow2Length = boxmat.v2;
    mWorldTrigger.fMatRow2Length.w = size.z;
    mWorldTrigger.fPosRadius = UMath::Vector4Make(center, radius);
    unsigned int *triggerWord = reinterpret_cast<unsigned int *>(&mWorldTrigger) + 4;
    /* Aqui habia un `register unsigned int triggerFlags asm("r8")` mas una
     * barrera `asm("" : "+r"(...))`. MEDIDO: quitando LAS DOS el objeto sale
     * byte a byte identico (99,5833%, las mismas 4 filas marcadas, y la unidad
     * entera en 137.232/141.472 B con 765 funciones). Quitar solo el registro
     * duro y dejar la barrera es PEOR (99,4969%, 13 filas), que es lo que
     * probablemente hizo pensar que el pin servia. */
    unsigned int triggerFlags = 0x4810d;
    triggerFlags |= *triggerWord & 0xff000000;
    *triggerWord = triggerFlags;
    if (OneShot()) {
        mWorldTrigger.fFlags |= 0x2;
    }

    mEventList.fNumEvents = 1;
    mEventStaticData.fEventID = 0xC34649C0;
    mEventStaticData.fEventSize = 8;
    mEventStaticData.fDataOffset = reinterpret_cast<const char *>(mTriggerEventData) - reinterpret_cast<const char *>(&mEventStaticData);
    *reinterpret_cast<unsigned int *>(&mTriggerEventData[4]) = GetCollection();
    mWorldTrigger.fEvents = &mEventList;

    bool showIconBasedOnBin = true;
    if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_engagetrigger)) {
        SetFlag(0x200);
        const GCollectionKey &targetActivityKey = TargetActivity();
        GRaceParameters *race = GRaceDatabase::Get().GetRaceFromKey(targetActivityKey.GetCollectionKey());
        if (race != NULL) {
            GIcon::Type iconType = (GIcon::Type)0;
            if (race->GetIsBossRace()) {
                iconType = GIcon::kType_RaceRival;
            } else {
                switch (race->GetRaceType()) {
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
            if (iconType != 0) {
                mIcon = GManager::Get().AllocIcon(iconType, pos, 0.0f, false);
            }
            showIconBasedOnBin = false;
        }
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_zone_carlot)) {
        SetFlag(0x2000);
        mIcon = GManager::Get().AllocIcon(GIcon::kType_GateCarLot, pos, 0.0f, false);
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_zone_customshop)) {
        SetFlag(0x4000);
        mIcon = GManager::Get().AllocIcon(GIcon::kType_GateCustomShop, pos, 0.0f, false);
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_zone_safehouse)) {
        SetFlag(0x8000);
        mIcon = GManager::Get().AllocIcon(GIcon::kType_GateSafehouse, pos, 0.0f, false);
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_speedtrap)) {
        SetFlag(0x100);
        if (OpenWorldSpeedTrap()) {
            mIcon = GManager::Get().AllocIcon(GIcon::kType_SpeedTrap, pos, 0.0f, false);
        } else {
            showIconBasedOnBin = false;
            mIcon = GManager::Get().AllocIcon(GIcon::kType_SpeedTrapInRace, pos, 0.0f, false);
        }
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_finishline)) {
        SetFlag(0x400);
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_time_bonus_checkpoint)) {
        SetFlag(0x800);
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_moneybag)) {
        SetFlag(0x1000);
    } else if (IsDerivedFromTemplate(Attrib::Hash::gameplay::key_areaunlocktrigger)) {
        mIcon = GManager::Get().AllocIcon(GIcon::kType_AreaUnlock, pos, 0.0f, false);
    }

    if (showIconBasedOnBin && mIcon != NULL) {
        if (BinIndex() > 0) {
            if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= BinIndex()) {
                mIcon->Show();
                mIcon->ShowOnMap();
            }
        }
    }
}

GTrigger::~GTrigger() {
    Enable(false);
    for (unsigned int i = 0; i < 2; i++) {
        if (mParticleEffect[i] != NULL) {
            mParticleEffect[i]->UnSubscribe();
            if (mParticleEffect[i] != NULL) {
                delete mParticleEffect[i];
            }
        }
    }
    if (mIcon != NULL) {
        GManager::Get().FreeIcon(mIcon);
        mIcon = NULL;
    }
}

GActivity *GTrigger::GetTargetActivity() {
    unsigned int key = 0x277566F3;
    return static_cast<GActivity *>(GetConnectedInstance(key, 0));
}

void GTrigger::AddActivationReference() {
    mActivationReferences = mActivationReferences + 1;
    if (mEnabled == 0) {
        Enable(true);
    }
}

void GTrigger::RemoveActivationReference() {
    if (mActivationReferences > 0) {
        mActivationReferences = mActivationReferences - 1;
    }
    if (mActivationReferences <= 0) {
        Enable(false);
    }
}

EmitterGroup *GTrigger::CreateParticleEffect(const char *effectName, UMath::Vector3 &position) {
    EmitterGroup *group = gEmitterSystem.CreateEmitterGroup(Attrib::StringKey(effectName), 0x8040000);
    if (group != NULL) {
        bMatrix4 matrix;
        bVector3 translation;
        bFill(&translation, position.z, -position.x, position.y);
        eCreateTranslationMatrix(&matrix, translation);
        group->SetLocalWorld(&matrix);
        group->SetAutoUpdate(true);
        group->SubscribeToDeletion(this, NotifyEmitterGroupDelete);
        group->Disable();
        gEmitterSystem.AddEmitterGroup(group);
    }
    return group;
}

void GTrigger::CreateAllParticleEffects() {
    const char *effectName = ParticleEffect();
    if (effectName != NULL) {
        if (*effectName != 0) {
            UMath::Vector3 position;
            GetPosition(position);
            float flareSpacing = FlareSpacing();
            if (flareSpacing > 0.0f) {
                UMath::Vector3 up = {0.0f, 1.0f, 0.0f};
                bVector3 cross;
                bCross(&cross, (const bVector3 *)&up, (const bVector3 *)&mDirection);
                bScale(&cross, &cross, flareSpacing);
                bVector3 flareA;
                bVector3 flareB;
                bScaleAdd(&flareA, (const bVector3 *)&position, &cross, -1.0f);
                bScaleAdd(&flareB, (const bVector3 *)&position, &cross, 1.0f);
                mParticleEffect[0] = CreateParticleEffect(effectName, *(UMath::Vector3 *)&flareA);
                mParticleEffect[1] = CreateParticleEffect(effectName, *(UMath::Vector3 *)&flareB);
            } else {
                mParticleEffect[0] = CreateParticleEffect(effectName, position);
            }
        }
    }
}

void GTrigger::ClearParticleEffects() {
    for (unsigned int i = 0; i < 2; i++) {
        if (mParticleEffect[i] != NULL) {
            mParticleEffect[i]->UnSubscribe();
            if (mParticleEffect[i] != NULL) {
                delete mParticleEffect[i];
            }
        }
    }
    bMemSet(mParticleEffect, 0, 8);
}

void GTrigger::EnableParticleEffects(bool enable) {
    for (unsigned int i = 0; i < 2; i++) {
        if (mParticleEffect[i] != NULL) {
            if (enable) {
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

void GTrigger::NotifyEmitterGroupDelete(void *context, EmitterGroup *group) {
    GTrigger *trigger = static_cast<GTrigger *>(context);
    for (unsigned int i = 0; i < 2; i++) {
        if (trigger->mParticleEffect[i] == group) {
            trigger->mParticleEffect[i] = NULL;
        }
    }
}

void GTrigger::Enable(bool enable) {
    if (mTriggerEnabled == 0) {
        if (enable) {
            WCollisionAssets::sWCollisionAssets->AddTrigger(&mWorldTrigger);
            mTriggerEnabled = 1;
        }
    } else if (!enable) {
        WCollisionAssets::sWCollisionAssets->RemoveTrigger(&mWorldTrigger);
        mTriggerEnabled = enable;
    }
    if (enable) {
        mWorldTrigger.Enable();
    } else {
        mWorldTrigger.Disable();
    }
    if (enable) {
        CreateAllParticleEffects();
    } else {
        ClearParticleEffects();
    }
    mEnabled = enable;
}

void GTrigger::GetPosition(UMath::Vector3 &position) {
    position = Vector4To3(mWorldTrigger.fPosRadius);
}

void GTrigger::NotifySimableTrigger(ISimable *simable, int eventType) {
    if (simable == NULL) {
        return;
    }
    bool wasEnter = (eventType == 1);
    bool wasExit = (eventType == 2);
    bool inside = IsInside(simable);
    if (wasExit) {
        MarkAsOutside(simable);
        if (FireOnExit()) {
            (MTriggerExit(GCollectionKey(this), simable->GetInstanceHandle())).Post(UCrc32(0x20d60dbf));
        }
    }
    if (wasEnter) {
        if (!inside) {
            MarkAsInside(simable);
            (MTriggerEnter(GCollectionKey(this), simable->GetInstanceHandle())).Post(UCrc32(0x20d60dbf));
        }
        (MTriggerInside(GCollectionKey(this), simable->GetInstanceHandle())).Post(UCrc32(0x20d60dbf));
    }
}

void GTrigger::Reset() {
    mSimObjInside.erase(mSimObjInside.begin(), mSimObjInside.end());
}

void GTrigger::ShowIcon() {
    if (mIcon != NULL) {
        mIcon->Show();
        mIcon->ShowOnMap();
    }
}

void GTrigger::HideIcon() {
    if (mIcon != NULL) {
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
