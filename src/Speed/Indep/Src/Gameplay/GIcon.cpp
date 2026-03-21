#include "Speed/Indep/Src/Gameplay/GIcon.h"

#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WorldModel.hpp"

extern void *gWGrid __asm__("_5WGrid.fgGrid");

struct WGrid {
    static bool Initialized() {
        return gWGrid != nullptr;
    }
};

static int sNumSpawned;

GIcon::EffectInfo GIcon::kEffectInfo[GIcon::kType_Count] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
};

GIcon::GIcon(Type type, const UMath::Vector3 &pos, float rotDeg)
    : mType(type), //
      mFlags(0), //
      mSectionID(-1), //
      mCombSectionID(-1), //
      mModel(nullptr), //
      mEmitter(nullptr), //
      mPosition(pos), //
      mRotation(bDegToAng(rotDeg)),
      mPad(0) {
    FindSection();
}

GIcon::~GIcon() {
    Unspawn();
}

void GIcon::Spawn() {
    EffectInfo *info = &kEffectInfo[mType];

    if (mType == kType_HidingSpot) {
        SnapToGround();
    }

    if (!mModel && info->mModelHash) {
        mModel = CreateGeometry(info->mModelHash);
    }

    if (!mEmitter && info->mParticleHash) {
        mEmitter = CreateParticleEffect(info->mParticleHash);
    }

    if (IsFlagClear(kFlag_Spawned)) {
        sNumSpawned++;
    }

    SetFlag(kFlag_Spawned);
    if (IsFlagSet(kFlag_ShowWhenSpawned)) {
        Show();
        ClearFlag(kFlag_ShowWhenSpawned);
    }
}

void GIcon::Unspawn() {
    if (IsFlagSet(kFlag_Spawned)) {
        ReleaseParticleEffect();
        ReleaseGeometry();
        ClearFlag(kFlag_Spawned);
        sNumSpawned--;
    }
}

void GIcon::FindSection() {
    bVector2 pos2D(mPosition.x, mPosition.z);
    DrivableScenerySection *drivable = TheVisibleSectionManager.FindDrivableSection(&pos2D);

    if (drivable) {
        mSectionID = drivable->SectionNumber;
        mCombSectionID = drivable->SectionNumber;
    } else {
        mSectionID = -1;
        mCombSectionID = -1;
    }
}

void GIcon::SnapToGround() {
    float worldHeight;
    bool heightValid;

    if (!WGrid::Initialized()) {
        return;
    }

    if (IsFlagSet(kFlag_Snapped)) {
        return;
    }

    if (mPosition.z != 0.0f) {
        return;
    }

    UMath::Vector3 posSwiz = UMath::Vector3Make(-mPosition.y, mPosition.z, mPosition.x);
    worldHeight = 0.0f;
    heightValid = WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(posSwiz, worldHeight, nullptr);
    if (heightValid) {
        SetFlag(kFlag_Snapped);
        mPosition.z = worldHeight;
        SetPosition();
    }
}

void GIcon::NotifyEmitterGroupDelete(void *obj, EmitterGroup *group) {
    GIcon *icon = reinterpret_cast<GIcon *>(obj);

    if (icon && icon->mEmitter == group) {
        icon->mEmitter = nullptr;
    }
}

EmitterGroup *GIcon::CreateParticleEffect(unsigned int particleHash) {
    EmitterGroup *effect = gEmitterSystem.CreateEmitterGroup(static_cast<Attrib::Key>(particleHash), 0x8040000);

    if (effect) {
        effect->SetAutoUpdate(true);
        effect->SubscribeToDeletion(this, NotifyEmitterGroupDelete);
        effect->Disable();
        gEmitterSystem.AddEmitterGroup(effect);
        mEmitter = effect;
        SetPosition();
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
    bool shouldBeEnabled = GetIsEnabled() && GetVisibleInWorld();

    if (mEmitter) {
        if (shouldBeEnabled) {
            mEmitter->Enable();
        } else {
            mEmitter->Disable();
        }
    }
}

WorldModel *GIcon::CreateGeometry(unsigned int modelHash) {
    WorldModel *model = new WorldModel(modelHash, nullptr, true);

    if (model) {
        model->SetEnabledFlag(false);
        mModel = model;
        SetPosition();
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
    if (IsFlagSet(kFlag_Spawned)) {
        bMatrix4 mat;

        bIdentity(&mat);
        eRotateZ(&mat, &mat, mRotation);
        mat.v3 = bVector4(mPosition.x, mPosition.y, mPosition.z, 1.0f);

        if (mModel) {
            mModel->SetMatrix(&mat);
            if (IsFlagClear(kFlag_Enabled)) {
                mModel->SetEnabledFlag(false);
            }
        }

        if (mEmitter) {
            mEmitter->SetLocalWorld(&mat);
        }
    }
}

void GIcon::Enable() {
    if (!IsFlagSet(kFlag_Enabled)) {
        if (mModel) {
            mModel->SetEnabledFlag(GetVisibleInWorld());
        }
        RefreshEffects();
        SetFlag(kFlag_Enabled);
    }
}

void GIcon::Disable() {
    if (mModel) {
        mModel->SetEnabledFlag(false);
    }
    RefreshEffects();
    ClearFlag(kFlag_Enabled);
}
