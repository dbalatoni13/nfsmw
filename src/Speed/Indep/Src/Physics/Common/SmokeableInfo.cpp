#include "Speed/Indep/Src/Physics/SceneryModel.h"
#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Scenery.hpp"

void ResetPropTimers();

SmokeableSectionQ TheSmokeableSections;

static void SceneryModel_InitSystem() {
    SceneryModel::InitSystem();
}

static void SceneryModel_RestoreSystem() {
    SceneryModel::RestoreSystem();
}

static Sim::SubSystem _Physics_System_SceneryModel("SceneryModel", SceneryModel_InitSystem, SceneryModel_RestoreSystem);
int SceneryModel::mSceneryCount = 0;

SmokeableSection *SmokeableSectionQ::FindOrAdd(int section_id) {
    SmokeableSection *section = nullptr;
    for (int i = 0; i < mQueue.size(); i++) {
        SmokeableSection &this_section = mQueue[i];
        if (this_section.SectionID == section_id) {
            SmokeableSection new_section(this_section);
            mQueue.dequeue();
            mQueue.enqueue(new_section);
            section = &mQueue.head();
            break;
        }
    }
    if (section == nullptr) {
        SmokeableSection new_section(section_id);
        mQueue.enqueue(new_section);
        section = &mQueue.head();
    }
    return section;
}

SmokeableSection *SmokeableSectionQ::Find(int section_id) {
    for (int i = 0; i < mQueue.size(); i++) {
        SmokeableSection &this_section = mQueue[i];
        if (this_section.SectionID == section_id) {
            return &this_section;
        }
    }
    return nullptr;
}

SceneryModel::SceneryModel(SmokeableSpawner *spawner, const CollisionGeometry::Bounds *geometry,
                           const Attrib::Collection *attribs, bool hidden)
    : HeirarchyModel(spawner->GetRenderMesh(), geometry, UCrc32(0x9756DF79u), nullptr, attribs, //
                     spawner->GetRenderHeirarchy(), 0, false) //
    , ISceneryModel(this) {
    mInstanceVisible = true;
    mSpawner = spawner;
    if (!hidden) {
        InitScene();
    } else {
        StartOverride();
    }
    mSceneryCount++;
}

SceneryModel::~SceneryModel() {
    if (mSpawner != nullptr) {
        mSpawner->mSimModel = nullptr;
        ShowInstance(true);
    }
    mSceneryCount--;
}

void SceneryModel::ShowInstance(bool show) {
    if (mSpawner != nullptr && show != mInstanceVisible) {
        if (show) {
            mSpawner->ShowInstance();
        } else {
            mSpawner->HideInstance();
        }
        mInstanceVisible = show;
    }
}

void SceneryModel::OnBeginDraw() {
    HeirarchyModel::OnBeginDraw();
    StartOverride();
}

void SceneryModel::HideModel() {
    Sim::Model::HideModel();
    StartOverride();
}

void SceneryModel::EndOverride() {
    ShowInstance(true);
}

void SceneryModel::StartOverride() {
    ShowInstance(false);
}

void SceneryModel::GetTransform(UMath::Matrix4 &matrix) const {
    if (mInstanceVisible) {
        if (!GetSceneryTransform(matrix)) {
            matrix = UMath::Matrix4::kIdentity;
        }
    } else {
        HeirarchyModel::GetTransform(matrix);
    }
}

void SceneryModel::InitScene() {
    UMath::Matrix4 scenery_matrix;

    ReleaseChildModels();
    StopEffects();
    Sim::Model::EndDraw();
    Sim::Model::EndSimulation();
    Sim::Model::ReleaseSequencer();
    EndOverride();
    SetCausality(0, 0.0f);

    if (GetSceneryTransform(scenery_matrix)) {
        SetTrigger(scenery_matrix, true);
    } else {
        RemoveTrigger();
    }

    bool enable_sequencer = start_sequencer();

    if (no_trigger()) {
        SmackableTrigger *trigger = GetTrigger();
        if (trigger != nullptr) {
            trigger->Disable();
        }
        enable_sequencer = true;
    }

    if (enable_sequencer) {
        Sim::Model::StartSequencer(UCrc32(EventSequencer()));
    }

    SetCameraAvoidable(true);
}

bool SceneryModel::GetSceneryTransform(UMath::Matrix4 &matrix) const {
    const CollisionGeometry::Bounds *bounds = GetCollisionGeometry();
    if (bounds != nullptr) {
        UMath::Vector3 pivot;
        bounds->GetPivot(pivot);
        UMath::QuaternionToMatrix4(mSpawner->GetOrientation(), matrix);
        UMath::Rotate(pivot, matrix, UMath::Vector4To3(matrix.v3));
        UMath::Addxyz(matrix.v3, mSpawner->GetPosition(), matrix.v3);
        return true;
    }
    return false;
}

void SceneryModel::OnEndSimulation() {
    HeirarchyModel::OnEndSimulation();
}

SceneryModel *SceneryModel::Construct(SmokeableSpawner *data, const Attrib::Collection *attributes, bool hidden) {
    if (static_cast<unsigned int>(mSceneryCount) < 256) {
        const CollisionGeometry::Collection *col = CollisionGeometry::Lookup(data->GetCollisionName());
        if (col != nullptr) {
            const CollisionGeometry::Bounds *bounds = col->GetRoot();
            if (bounds != nullptr) {
                if (attributes == nullptr) {
                    return nullptr;
                }
                bounds = col->GetRoot();
                if (bounds != nullptr) {
                    UMath::Vector3 dimension;
                    bounds->GetHalfDimensions(dimension);
                    if (UMath::LengthSquare(dimension) > 0.0f) {
                        return new SceneryModel(data, bounds, attributes, hidden);
                    }
                }
            }
        }
    }
    return nullptr;
}

void SceneryModel::WakeUp() {
    SmackableTrigger *trigger = GetTrigger();
    if (trigger != nullptr && !IsRendering() && !IsSimulating()) {
        trigger->Fire();
        trigger->Disable();
    }
}

void SceneryModel::RestoreScene() {
    InitScene();
}

void SceneryModel::ReleaseModel() {
    Sim::Model::EndDraw();
    Sim::Model::EndSimulation();
    StopEffects();
    HeirarchyModel::DisableTrigger();
}

void SceneryModel::InitSystem() {
    ResetPropTimers();
}

void SceneryModel::RestoreSystem() {
    ResetPropTimers();
}

void ResetPropTimers() {
    TheSmokeableSections.Reset();
}

static const Attrib::Class *TheSmackableClass;

void SmokeableSpawnerPack::OnUnload() {
    SmokeableSection *section = TheSmokeableSections.FindOrAdd(static_cast<int>(ScenerySectionNumber));
    section->LastLoadTime = Sim::GetTime();

    for (int n = 0; n < NumSmokeableSpawners; n++) {
        {
            SmokeableSpawner *spawner = &SmokeableSpawners[n];
            if (static_cast<unsigned int>(n) < 256) {
                if (!spawner->IsInstanceVisible()) {
                    section->Rebuilds.Set(static_cast<unsigned int>(n));
                } else {
                    section->Rebuilds.Clear(static_cast<unsigned int>(n));
                }
            }
            spawner->OnUnload();
        }
    }
}

void SmokeableSpawnerPack::EndianSwap() {
    if (EndianSwapped) {
        return;
    }
    EndianSwapped = 1;
    bPlatEndianSwap(&ScenerySectionNumber);
    bPlatEndianSwap(&FirstSmokeableSpawnerID);
    bPlatEndianSwap(&NumSmokeableSpawners);
    for (int n = 0; n < NumSmokeableSpawners; n++) {
        SmokeableSpawners[n].EndianSwap();
    }
}

void SmokeableSpawnerPack::OnMoved() {
    for (int n = 0; n < NumSmokeableSpawners; n++) {
        SmokeableSpawners[n].OnMoved();
    }
}

void SmokeableSpawnerPack::OnLoad(unsigned int exclude_flags) {
    SmokeableSection *section = TheSmokeableSections.Find(static_cast<int>(ScenerySectionNumber));

    if (!GRaceStatus::Exists() || !GRaceStatus::Get().GetActivelyRacing()) {
        if (section != nullptr) {
            if (section->LastLoadTime + 180.0f < Sim::GetTime()) {
                section->Rebuilds.Clear();
                section = nullptr;
            }
        }
    }

    if (section == nullptr || !section->Rebuilds.Test()) {
        GManager::Get().RestorePursuitBreakerIcons(static_cast<int>(ScenerySectionNumber));
    }

    for (int n = 0; n < NumSmokeableSpawners; n++) {
        bool ignore = false;
        if (section != nullptr && static_cast<unsigned int>(n) < 256 && section->Rebuilds.Test(static_cast<unsigned int>(n))) {
            ignore = true;
        }
        SmokeableSpawners[n].OnLoad(exclude_flags, ignore);
    }
}

const Attrib::Collection *SmokeableSpawner::FindAttributes(UCrc32 name) {
    return TheSmackableClass->GetCollection(name.GetValue());
}

void SmokeableSpawner::Init() {
    TheSmackableClass = Attrib::Database::Get().GetClass(Attrib::Gen::smackable::ClassKey());
}

void SmokeableSpawner::EndianSwap() {
    bPlatEndianSwap(&mOrientation);
    bPlatEndianSwap(&mPosition);
    bPlatEndianSwap(&mModel);
    bPlatEndianSwap(&mCollisionName);
    bPlatEndianSwap(&mAttributes);
    bPlatEndianSwap(&mSceneryOverrideInfoNumber);
    bPlatEndianSwap(&mUniqueID);
    bPlatEndianSwap(&mExcludeFlags);
}

void SmokeableSpawner::OnUnload() {
    if (mSimModel != nullptr) {
        delete mSimModel;
        mSimModel = nullptr;
    }
}

const ModelHeirarchy *SmokeableSpawner::GetRenderHeirarchy() const {
    ModelHeirarchy *heirarchy;
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info != nullptr) {
        ScenerySectionHeader *section_header = GetScenerySectionHeader(static_cast<int>(info->SectionNumber));
        if (section_header != nullptr) {
            SceneryInstance *scenery_instance = section_header->GetSceneryInstance(static_cast<int>(info->InstanceNumber));
            if (scenery_instance != nullptr) {
                SceneryInfo *sinfo = section_header->GetSceneryInfo(scenery_instance);
                if (sinfo != nullptr) {
                    heirarchy = sinfo->mHeirarchy;
                    return heirarchy;
                }
            }
        }
    }
    return nullptr;
}

bHash32 SmokeableSpawner::GetRenderMesh() const {
    SceneryOverrideInfo *soi = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (soi != nullptr) {
        ScenerySectionHeader *section_header = GetScenerySectionHeader(static_cast<int>(soi->SectionNumber));
        if (section_header != nullptr) {
            SceneryInstance *scenery_instance = section_header->GetSceneryInstance(static_cast<int>(soi->InstanceNumber));
            if (scenery_instance != nullptr) {
                SceneryInfo *scenery_info = section_header->GetSceneryInfo(scenery_instance);
                if (scenery_info != nullptr) {
                    for (int i = 0; i < 4; i++) {
                        if (scenery_info->NameHash[i] != 0) {
                            return bHash32(scenery_info->NameHash[i]);
                        }
                    }
                }
            }
        }
    }
    return bHash32(0x0C7395A8u);
}

void SmokeableSpawner::ShowInstance() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info != nullptr) {
        info->EnableRendering();
        info->AssignOverrides();
    }
}

bool SmokeableSpawner::IsInstanceVisible() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info != nullptr) {
        return (info->ExcludeFlags & 0x10) == 0;
    }
    return false;
}

void SmokeableSpawner::HideInstance() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info != nullptr) {
        info->DisableRendering();
        info->AssignOverrides();
    }
}

void SmokeableSpawner::OnMoved() {
    if (mSimModel != nullptr) {
        mSimModel->mSpawner = this;
    }
}

void SmokeableSpawner::OnLoad(unsigned int exclude_flags, bool hidden) {
    if ((exclude_flags & mExcludeFlags) == 0) {
        ShowInstance();
        const Attrib::Collection *collection = FindAttributes(UCrc32(mAttributes));
        if (collection != nullptr) {
            mSimModel = SceneryModel::Construct(this, collection, hidden);
        } else {
            mSimModel = nullptr;
        }
    }
    if (mSimModel == nullptr) {
        HideInstance();
    }
}

int SmokeableSpawnerPack::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x34027) {
        SmokeableSpawnerPack *spawner_pack = reinterpret_cast<SmokeableSpawnerPack *>(chunk->GetAlignedData(16));
        if (AreChunksBeingMoved()) {
            spawner_pack->OnMoved();
        } else {
            spawner_pack->EndianSwap();
            if (Sim::Exists()) {
                unsigned int exclude_flags = static_cast<unsigned int>(Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN);
                if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
                    exclude_flags |= 4;
                }
                spawner_pack->OnLoad(exclude_flags);
            }
        }
        return 1;
    }
    return 0;
}

int SmokeableSpawnerPack::Unloader(bChunk *chunk) {
    if (chunk->GetID() == 0x34027) {
        if (!AreChunksBeingMoved() && Sim::Exists()) {
            SmokeableSpawnerPack *pack = reinterpret_cast<SmokeableSpawnerPack *>(chunk->GetAlignedData(16));
            pack->OnUnload();
        }
        return 1;
    }
    return 0;
}

bChunkLoader SmokeableSpawnerPack::mLoader(0x34027, SmokeableSpawnerPack::Loader, SmokeableSpawnerPack::Unloader);

inline bool SceneryModel::IsHidden() const {
    bool visible = mInstanceVisible || !Sim::Model::IsHidden();
    return !visible;
}

inline bool SceneryModel::IsExcluded(unsigned int scenery_exclusion_flag) const {
    unsigned int my_flags = 0;
    if (mSpawner != nullptr) {
        my_flags = mSpawner->GetExcludeFlags();
    }
    return (my_flags & scenery_exclusion_flag) != 0;
}

inline unsigned int SceneryModel::GetSpawnerID() const {
    if (mSpawner != nullptr) {
        return mSpawner->GetUniqueID();
    }
    return 0xFFFFFFFF;
}

bool HeirarchyModel::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    if (mTrigger != nullptr && mTrigger->IsEnabled()) {
        if (0.0f < mTriggerAvoid.w) {
            pos = UMath::Vector4To3(mTriggerAvoid);
            sweep = mTriggerAvoid.w;
            return true;
        }
    } else {
        if (GetSimable() != nullptr) {
            IRigidBody *irb = GetSimable()->GetRigidBody();
            if (irb != nullptr) {
                sweep = irb->GetRadius();
                pos = irb->GetPosition();
                return true;
            }
        }
    }
    return false;
}
