
#include "Speed/Indep/Src/Gameplay/GIcon.h"

#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WorldModel.hpp"

// Valores leidos de .data 0x8041D3DC. mType coincide con el indice.
GIcon::EffectInfo GIcon::kEffectInfo[] = {
    {0, 0x00000000, 0x00000000},  {1, 0x9E535570, 0xE1EB3B0F},  {2, 0x3A583CA3, 0x3CCB52C5},
    {3, 0xC8E0972E, 0xC0087531},  {4, 0x734A6A5E, 0xB8E326BF},  {5, 0xCA67B527, 0x5A4699BF},
    {6, 0x3232E2F8, 0x60BB33EC},  {7, 0x4C471D8E, 0x8A2709BE},  {8, 0x9E3223D3, 0x6D018122},
    {9, 0x85C25E95, 0xBE1EF064},  {10, 0x868C2E14, 0x6699D23D}, {11, 0xD2592F60, 0xF9F71422},
    {12, 0x00000000, 0x7546C031}, {13, 0xC1FD605B, 0x60BB33EC}, {14, 0xC1FD605B, 0x60BB33EC},
    {15, 0x00000000, 0x00000000}, {16, 0x738B1F9B, 0x00000000},
};

static int sNumSpawned = 0;

GIcon::GIcon(Type type, const UMath::Vector3 &position, float rotation)
    : mType(type),
      mFlags(0),
      mSectionID(-1),
      mCombSectionID(-1),
      mModel(NULL),
      mEmitter(NULL),
      mPosition(UMath::Vector3Make(position.x, position.y, position.z)) {
    mRotation = bDegToAng(rotation);
    FindSection();
}

GIcon::~GIcon() {
    Unspawn();

    mPosition = UMath::Vector3::kZero;
    mRotation = 0;
    mType = 0;
    mFlags = 0;
    mModel = NULL;
    mEmitter = NULL;
    mSectionID = -1;
    mCombSectionID = -1;
}

void GIcon::Spawn() {
    EffectInfo *info = &kEffectInfo[mType];

    if (mType == kType_HidingSpot) {
        SnapToGround();
    }

    if (mModel == NULL) {
        if (info->mModelHash != 0) {
            mModel = CreateGeometry(info->mModelHash);
        }
    }

    if (mEmitter == NULL) {
        if (info->mParticleHash != 0) {
            mEmitter = CreateParticleEffect(info->mParticleHash);
        }
    }

    if (IsFlagClear(0x4)) {
        sNumSpawned++;
    }

    mFlags |= 0x4;

    if (IsFlagSet(0x40)) {
        Show();
        ClearFlag(0x40);
    }
}

void GIcon::Unspawn() {
    Disable();
    ReleaseGeometry();
    ReleaseParticleEffect();

    if (IsFlagSet(0x4)) {
        sNumSpawned--;
    }

    ClearFlag(0x4);
}

void GIcon::FindSection() {
    if (mSectionID < 0 && TheVisibleSectionManager.IsLoaded()) {
        bVector2 pos2D(mPosition.x, mPosition.y);

        DrivableScenerySection *drivable = TheVisibleSectionManager.FindDrivableSection(&pos2D);

        if (drivable != NULL) {
            mSectionID = drivable->GetSectionNumber();
            mCombSectionID = TheTrackStreamer.GetCombinedSectionNumber(mSectionID);
        }
    }
}

void GIcon::SnapToGround() {
    if (!WGrid::Initialized()) {
        return;
    }

    if (IsFlagSet(0x20)) {
        return;
    }

    if (mPosition.z != 0.0f) {
        return;
    }

    UMath::Vector3 posSwiz = UMath::Vector3Make(-mPosition.y, mPosition.z, mPosition.x);

    float worldHeight = 0.0f;

    bool heightValid = WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(posSwiz, worldHeight, NULL);

    if (heightValid) {
        mFlags |= 0x20;
        mPosition.z = worldHeight;
        SetPosition();
    }
}

void GIcon::NotifyEmitterGroupDelete(void *obj, EmitterGroup *group) {
    GIcon *icon = (GIcon *) obj;

    if (icon->mEmitter == group) {
        icon->mEmitter = nullptr;
    }
}

EmitterGroup *GIcon::CreateParticleEffect(unsigned int particleHash) {
    EmitterGroup *effect = gEmitterSystem.CreateEmitterGroup(particleHash, 0x8040000);

    if (effect != NULL) {
        effect->SetAutoUpdate(true);
        effect->SubscribeToDeletion(this, NotifyEmitterGroupDelete);
        effect->Disable();

        gEmitterSystem.AddEmitterGroup(effect);
    }

    return effect;
}

void GIcon::ReleaseParticleEffect() {
    if (mEmitter) {
        delete mEmitter;
        mEmitter = nullptr;
    }
}

void GIcon::RefreshEffects() {
    bool enabled = IsFlagSet(0x8);

    Disable();
    ReleaseParticleEffect();

    if (enabled) {
        Enable();
    }
}

WorldModel *GIcon::CreateGeometry(unsigned int modelHash) {
    WorldModel *model = new WorldModel(modelHash, NULL, false);

    if (model != NULL) {
        model->SetEnabledFlag(false);
#ifndef EA_BUILD_A124
        model->SetCastsShadow(0);
#endif
    }

    return model;
}

void GIcon::ReleaseGeometry() {
    if (mModel) {
        delete mModel;
        mModel = nullptr;
    }
}

void GIcon::SetPosition() {
    if (IsFlagSet(0x4)) {
        bMatrix4 mat;
        bVector3 pos;

        bCopy(&pos, (const bVector3 *) &mPosition);
        bIdentity(&mat);
        eRotateZ(&mat, &mat, mRotation);
        bCopy(&mat.v3, &pos, 1.0f);

        if (mModel != NULL) {
            mModel->SetMatrix(&mat);

            if (IsFlagClear(0x8)) {
                mModel->SetEnabledFlag(false);
            }
        }

        if (mEmitter != NULL) {
            mEmitter->SetLocalWorld(&mat);
        }
    }
}

void GIcon::Enable() {
    if (IsFlagSet(0x4)) {
        SetPosition();

        if (mModel != NULL) {
            mModel->SetAddLighting(true);
            mModel->SetEnabledFlag(true);
        }

        if (mEmitter != NULL) {
            mEmitter->Enable();
        }

        mFlags |= 0x8;
    }
}

void GIcon::Disable() {
    if (mModel) {
        mModel->SetEnabledFlag(false);
    }

    if (mEmitter) {
        mEmitter->Disable();
    }

    ClearFlag(0x08);
}
