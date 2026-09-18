#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Smackable.h"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

ScenerySectionHeader *GetScenerySectionHeader(int section_number);
ModelHeirarchy *FindSceneryHeirarchyByName(unsigned int name_hash);

const Attrib::Class *TheSmackableClass = nullptr;

inline void EndianSwapVector4(UMath::Vector4 *v) {
    bPlatEndianSwap(&v->x);
    bPlatEndianSwap(&v->y);
    bPlatEndianSwap(&v->z);
    bPlatEndianSwap(&v->w);
}

SmokeableSectionQ TheSmokeableSections;

SmokeableSection *SmokeableSectionQ::FindOrAdd(int section_id) {
    SmokeableSection *section = NULL;
    for (int i = 0; i < mQueue.size(); i++) {
        SmokeableSection &this_section = mQueue[i];
        if (this_section.SectionID == section_id) {
            SmokeableSection new_section = this_section;
            mQueue.dequeue();
            mQueue.enqueue(new_section);
            section = &mQueue.head();
            break;
        }
    }

    if (section == NULL) {
        SmokeableSection new_section(section_id);
        mQueue.enqueue(new_section);
        section = &mQueue.head();
    }

    return section;
}

SmokeableSection *SmokeableSectionQ::Find(int section_id) {
    for (int i = 0; i < mQueue.size(); i++) {
        SmokeableSection *section = &mQueue[i];
        if (section->SectionID == section_id) {
            return section;
        }
    }

    return NULL;
}

void ResetPropTimers() {
    TheSmokeableSections.Reset();
}

HeirarchyModel::HeirarchyModel(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry, UCrc32 rendernode, HeirarchyModel *parent,
                               const Attrib::Collection *attribs, const ModelHeirarchy *heirarchy, unsigned int heirarchynode, bool visible)
    : Sim::Model(parent, geometry, rendernode, 6), IBody(this), ITriggerableModel(this), Attrib::Gen::smackable(attribs, 0, NULL), mTriggerAvoid(UMath::Vector4::kZero),
      mHeirarchy(heirarchy), mRenderMesh(rendermesh), mTrigger(NULL), mOffScreenTimer(0.0f), mHeirarchyNode(heirarchynode), mFlags(0),
      mChildVisibility(-1), mAvoidable(NULL) {
    Attrib::Gen::smackable attributes(attribs, 0, NULL);

    if (visible) {
        RenderConn::Pkt_Smackable_Open open(mRenderMesh, GetWorldID(), GetCollisionGeometry(), mHeirarchy, mHeirarchyNode);
        BeginDraw(UCrc32(0x804C146E), &open);
    }

    if (attributes.AI_AVOIDABLE()) {
        SetAvoidable(true);
    }

    if (attributes.CAMERA_AVOIDABLE()) {
        SetCameraAvoidable(true);
    }
}

void HeirarchyModel::SetCameraAvoidable(bool b) {
    unsigned int avoidable = mFlags & F_CAMERA_AVOIDABLE;
    if (b != avoidable) {
        if (b) {
            if (CAMERA_AVOIDABLE()) {
                CameraAI::AddAvoidable(this);
                mFlags |= F_CAMERA_AVOIDABLE;
            }
        } else {
            CameraAI::RemoveAvoidable(this);
            mFlags &= ~F_CAMERA_AVOIDABLE;
        }
    }
}

bool HeirarchyModel::OnRemoveOffScreen(float dT) {
    if (mOffScreenTimer > 0.0f) {
        if (!InView()) {
            mOffScreenTimer -= dT;
            if (mOffScreenTimer < 0.0f) {
                mOffScreenTimer = 0.0f;
                return true;
            }
        } else {
            mOffScreenTimer = KILL_OFF_SCREEN();
        }
    }

    return false;
}

void HeirarchyModel::OnProcessFrame(float dT) {
    if (OnRemoveOffScreen(dT)) {
        ReleaseModel();
    }
}

void HeirarchyModel::HidePart(const UCrc32 &nodename) {
    int index = FindHeirarchyChild(nodename);
    if (index < 0) {
        return;
    }

    mChildVisibility &= ~(1 << index);
}

void HeirarchyModel::ShowPart(const UCrc32 &nodename) {
    int index = FindHeirarchyChild(nodename);
    if (index < 0) {
        return;
    }

    mChildVisibility |= 1 << index;
}

bool HeirarchyModel::IsPartVisible(const UCrc32 &nodename) const {
    int index = FindHeirarchyChild(nodename);
    if (index >= 0) {
        bool visible = false;
        if (mChildVisibility & (1 << index)) {
            visible = true;
        }

        return visible;
    }

    return false;
}

int HeirarchyModel::FindHeirarchyChild(const UCrc32 &nodename) const {
    if (mHeirarchy == NULL) {
        return -1;
    }

    const ModelHeirarchy::Node *node = &mHeirarchy->GetNodes()[mHeirarchyNode];
    int found = -1;
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        if (mHeirarchy->GetNodes()[node->mChildIndex + i].mNodeName == nodename) {
            found = node->mChildIndex + i;
            break;
        }
    }

    return found;
}

IModel *HeirarchyModel::SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) {
    if (mHeirarchy == NULL) {
        return NULL;
    }

    if (IsDirty()) {
        return NULL;
    }

    if (UTL::Collections::Listable<IModel, 434>::Count() > 434u) {
        return NULL;
    }

    int index = FindHeirarchyChild(rendernode);
    if (index < 0) {
        return NULL;
    }

    const CollisionGeometry::Bounds *bounds = GetCollisionGeometry()->GetChild(collisionnode);
    if (bounds == NULL) {
        return NULL;
    }

    const Attrib::Collection *collection = SmokeableSpawner::FindAttributes(attributes);
    if (collection == NULL) {
        return NULL;
    }

    eModel *model = mHeirarchy->GetNodes()[index].mModel;
    if (model == NULL) {
        return NULL;
    }

    return new HeirarchyModel(model->GetNameHash(), bounds, rendernode, this, collection, mHeirarchy, index, true);
}

HeirarchyModel::~HeirarchyModel() {
    EndSimulation();

    RemoveTrigger();

    ReleaseChildModels();

    if (mAvoidable) {
        delete mAvoidable;
        mAvoidable = NULL;
    }

    SetCameraAvoidable(false);
}

void HeirarchyModel::OnBeginDraw() {
    StartSequencer(EventSequencer());

    mOffScreenTimer = KILL_OFF_SCREEN();
}

void HeirarchyModel::OnEndDraw() {
    mOffScreenTimer = 0.0f;

    if (mFlags & F_CAMERA_AVOIDABLE) {
        CameraAI::RemoveAvoidable(this);
        mFlags &= ~F_CAMERA_AVOIDABLE;
    }
}

void HeirarchyModel::GetTransform(UMath::Matrix4 &matrix) const {
    if (GetSimable()) {
        GetSimable()->GetTransform(matrix);
    } else if (mTrigger) {
        mTrigger->GetObjectMatrix(matrix);
    } else {
        matrix = UMath::Matrix4::kIdentity;
    }
}

void HeirarchyModel::GetAngularVelocity(UMath::Vector3 &velocity) const {
    if (GetSimable()) {
        velocity = GetSimable()->GetRigidBody()->GetAngularVelocity();
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void HeirarchyModel::RemoveTrigger() {
    if (mTrigger) {
        delete mTrigger;
        mTrigger = NULL;
    }
}

void HeirarchyModel::DisableTrigger() {
    if (mTrigger) {
        mTrigger->Disable();
    }
}

void HeirarchyModel::SetTrigger(const UMath::Matrix4 &matrix, bool virgin) {
    UMath::Vector3 dim;

    GetCollisionGeometry()->GetHalfDimensions(dim);

    if (mTrigger == NULL) {
        mTrigger = new SmackableTrigger(GetInstanceHandle(), virgin, matrix, dim, 0);
    } else {
        mTrigger->Move(matrix, dim, virgin);
        mTrigger->Enable();
    }

    mTriggerAvoid = matrix.v3;
    float x_len = dim.x * matrix.v0.x + dim.y * matrix.v1.x + dim.z * matrix.v2.x;
    float z_len = dim.x * matrix.v0.z + dim.y * matrix.v1.z + dim.z * matrix.v2.z;
    mTriggerAvoid.w = UMath::Sqrt(x_len * x_len + z_len * z_len);
}

bool HeirarchyModel::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    if (mTrigger && mTrigger->IsEnabled()) {
        if (mTriggerAvoid.w > 0.0f) {
            pos = *(const UMath::Vector3 *) &mTriggerAvoid;
            sweep = mTriggerAvoid.w;
            return true;
        }
    } else if (GetSimable()) {
        IRigidBody *irb = GetSimable()->GetRigidBody();
        if (irb) {
            sweep = irb->GetRadius();
            pos = irb->GetPosition();
            return true;
        }
    }

    return false;
}

void HeirarchyModel::PlaceTrigger(const UMath::Matrix4 &matrix, bool enable) {
    SetTrigger(matrix, false);

    if (!enable) {
        DisableTrigger();
    }
}

void HeirarchyModel::OnEndSimulation() {
    if (mAvoidable) {
        mAvoidable->SetRefrence((IBody *) this);
    }
}

void HeirarchyModel::OnBeginSimulation() {
    DisableTrigger();

    if (mAvoidable) {
        mAvoidable->SetRefrence(GetSimable());
    }

    RenderConn::Pkt_Smackable_Open open(mRenderMesh, GetWorldID(), GetCollisionGeometry(), mHeirarchy, mHeirarchyNode);
    BeginDraw(UCrc32(0x804C146E), &open);
}

bool HeirarchyModel::OnDraw(Sim::Packet *service) {
    RenderConn::Pkt_Smackable_Service *draw = static_cast<RenderConn::Pkt_Smackable_Service *>(service);

    UpdateVisibility(draw->IsVisible(), draw->GetDistanceToView());

    draw->SetChildVisibility(mChildVisibility);

    return true;
}

PlaceableScenery::PlaceableScenery(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry, const Attrib::Collection *attribs,
                                   const ModelHeirarchy *heirarchy)
    : HeirarchyModel(rendermesh, geometry, UCrc32(0x9756DF79u), NULL, attribs, heirarchy, 0, false), IPlaceableScenery(this) {
}

void PlaceableScenery::ReleaseModel() {
    PickUp();
}

PlaceableScenery *PlaceableScenery::Construct(const char *name, unsigned int attributes) {
    if (UTL::Collections::Listable<IModel, 434>::Count() > 434u) {
        return NULL;
    }

    if (UTL::Collections::Countable<IPlaceableScenery>::Count() > 12u) {
        return NULL;
    }

    bHash32 render_name(bStringHashUpper(name));
    UCrc32 collision_name(name);
    bHash32 heirarchy_name(render_name.GetValue());

    const ModelHeirarchy *heirarchy = FindSceneryHeirarchyByName(heirarchy_name.GetValue());

    const CollisionGeometry::Collection *collection = CollisionGeometry::Lookup(collision_name);
    if (collection == NULL) {
        return NULL;
    }

    const CollisionGeometry::Bounds *bounds = collection->GetRoot();
    if (bounds == NULL) {
        return NULL;
    }

    eModel model(render_name.GetValue());
    if (model.GetSolid() == NULL) {
        render_name = bHash32(0x0C7395A8);
    }

    Attrib::Gen::smackable attribs(attributes, 0, NULL);

    return new PlaceableScenery(render_name, bounds, attribs.GetConstCollection(), heirarchy);
}

void PlaceableScenery::PickUp() {
    ReleaseChildModels();
    StopEffects();
    EndDraw();
    EndSimulation();
    ReleaseSequencer();
    DisableTrigger();
}

bool PlaceableScenery::Place(const UMath::Matrix4 &transform, bool snap_to_ground) {
    PickUp();

    UMath::Matrix4 mat;
    UMath::Copy(transform, mat);

    if (snap_to_ground) {
        float worldHeight = 0.0f;

        if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(UMath::Vector4To3(mat.v3), worldHeight, NULL)) {
            UMath::Vector3 dim;
            GetDimension(dim);

            mat.v3.y = worldHeight + dim.y;
        } else {
            return false;
        }
    }

    PlaceTrigger(mat, false);

    ISimable *physics = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance(UCrc32("Smackable"), SmackableParams(mat, true, this, false));

    if (physics == NULL) {
        PickUp();
        return false;
    }

    return true;
}

SmackableAvoidable::SmackableAvoidable(HeirarchyModel *model) : AIAvoidable(static_cast<IModel *>(mModel)), mModel(model) {
    SetRefrence(static_cast<IBody *>(model));
}

bool SmackableAvoidable::OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) {
    return mModel->OnUpdateAvoidable(pos, sweep);
}

SceneryModel::SceneryModel(SmokeableSpawner *spawner, const CollisionGeometry::Bounds *geometry, const Attrib::Collection *attribs, bool hidden)
    : HeirarchyModel(spawner->GetRenderMesh(), geometry, UCrc32(0x9756DF79u), NULL, attribs, spawner->GetRenderHeirarchy(), 0, false),
      ISceneryModel(this), mInstanceVisible(true), mSpawner(spawner) {
    if (!hidden) {
        InitScene();
    } else {
        StartOverride();
    }

    mSceneryCount++;
}

SceneryModel::~SceneryModel() {
    if (mSpawner) {
        mSpawner->SetSimModel(NULL);
        ShowInstance(true);
    }

    mSceneryCount--;
}

int SceneryModel::mSceneryCount = 0;

void SceneryModel::ShowInstance(bool show) {
    if (mSpawner == NULL) {
        return;
    }

    if (show == mInstanceVisible) {
        return;
    }

    if (show) {
        mSpawner->ShowInstance();
    } else {
        mSpawner->HideInstance();
    }

    mInstanceVisible = show;
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
    ReleaseChildModels();
    StopEffects();
    EndDraw();
    EndSimulation();
    ReleaseSequencer();
    EndOverride();

    SetCausality(NULL, 0.0f);

    UMath::Matrix4 matrix;
    if (GetSceneryTransform(matrix)) {
        SetTrigger(matrix, true);
    } else {
        RemoveTrigger();
    }

    bool start = start_sequencer();
    if (no_trigger()) {
        if (GetTrigger()) {
            GetTrigger()->Disable();
        }
        start = true;
    }

    if (start) {
        StartSequencer(EventSequencer());
    }

    SetCameraAvoidable(true);
}

bool SceneryModel::GetSceneryTransform(UMath::Matrix4 &matrix) const {
    const CollisionGeometry::Bounds *bounds = GetCollisionGeometry();

    if (bounds) {
        UMath::Vector3 pivot;
        bounds->GetPivot(pivot);

        UMath::QuaternionToMatrix4(mSpawner->GetOrientation(), matrix);

        UMath::Rotate(pivot, matrix, *(UMath::Vector3 *) &matrix.v3);
        UMath::Addxyz(matrix.v3, mSpawner->GetPosition(), matrix.v3);

        return true;
    }

    return false;
}

void SceneryModel::OnEndSimulation() {
    HeirarchyModel::OnEndSimulation();
}

SceneryModel *SceneryModel::Construct(SmokeableSpawner *data, const Attrib::Collection *attributes, bool hidden) {
    if (mSceneryCount > 255u) {
        return NULL;
    }

    const CollisionGeometry::Collection *collection = CollisionGeometry::Lookup(data->GetCollisionName());
    if (collection == NULL || collection->GetRoot() == NULL) {
        return NULL;
    }

    if (attributes == NULL) {
        return NULL;
    }

    const CollisionGeometry::Bounds *bounds = collection->GetRoot();
    if (bounds == NULL) {
        return NULL;
    }

    UMath::Vector3 dim;
    bounds->GetHalfDimensions(dim);

    if (UMath::LengthSquare(dim) <= 0.0f) {
        return NULL;
    }

    return new SceneryModel(data, bounds, attributes, hidden);
}

void SceneryModel::WakeUp() {
    SmackableTrigger *trigger = GetTrigger();
    if (trigger && !IsRendering() && !IsSimulating()) {
        trigger->Fire();
        trigger->Disable();
    }
}

void SceneryModel::RestoreScene() {
    InitScene();
}

void SceneryModel::ReleaseModel() {
    EndDraw();
    EndSimulation();
    StopEffects();
    DisableTrigger();
}

void SceneryModel::InitSystem() {
    ResetPropTimers();
}

void SceneryModel::RestoreSystem() {
    ResetPropTimers();
}

BIND_SIM_SUBSYSTEM(SceneryModel, SceneryModel::InitSystem, SceneryModel::RestoreSystem);

void SmokeableSpawnerPack::OnUnload() {
    SmokeableSection *section = TheSmokeableSections.FindOrAdd(ScenerySectionNumber);

    section->LastLoadTime = Sim::GetTime();

    for (int n = 0; n < NumSmokeableSpawners; n++) {
        SmokeableSpawner *spawner = &SmokeableSpawners[n];

        if (static_cast<unsigned int>(n) <= 255) {
            if (!spawner->IsInstanceVisible()) {
                section->Rebuilds.Set(n);
            } else {
                section->Rebuilds.Clear(n);
            }
        }

        spawner->OnUnload();
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
    SmokeableSection *section = TheSmokeableSections.Find(ScenerySectionNumber);

    if (!(GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing())) {
        if (section && section->LastLoadTime + 180.0f < Sim::GetTime()) {
            section->Rebuilds.Clear();
            section = NULL;
        }
    }

    if (section == NULL || !section->Rebuilds.AnySet()) {
        GManager::Get().RestorePursuitBreakerIcons(ScenerySectionNumber);
    }

    for (int n = 0; n < NumSmokeableSpawners; n++) {
        bool ignore = false;

        if (section && static_cast<unsigned int>(n) <= 255) {
            if (section->Rebuilds.Test(n)) {
                ignore = true;
            }
        }

        SmokeableSpawners[n].OnLoad(exclude_flags, ignore);
    }
}

const Attrib::Collection *SmokeableSpawner::FindAttributes(UCrc32 name) {
    return TheSmackableClass->GetCollection(name.GetValue());
}

void SmokeableSpawner::Init() {
    const Attrib::Class *smackableClass = Attrib::Database::Get().GetClass(Attrib::Gen::smackable::ClassKey());
    TheSmackableClass = smackableClass;
}

void SmokeableSpawner::EndianSwap() {
    EndianSwapVector4(&mOrientation);
    EndianSwapVector4(&mPosition);
    bPlatEndianSwap(&mModel);
    bPlatEndianSwap(&mCollisionName);
    bPlatEndianSwap(&mAttributes);
    bPlatEndianSwap(&mSceneryOverrideInfoNumber);
    bPlatEndianSwap(&mUniqueID);
    bPlatEndianSwap(&mExcludeFlags);
}

void SmokeableSpawner::OnUnload() {
    if (mSimModel) {
        delete mSimModel;
        mSimModel = NULL;
    }
}

const ModelHeirarchy *SmokeableSpawner::GetRenderHeirarchy() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info) {
        ScenerySectionHeader *section_header = GetScenerySectionHeader(info->SectionNumber);
        if (section_header) {
            SceneryInstance *instance = section_header->GetSceneryInstance(info->InstanceNumber);
            if (instance) {
                SceneryInfo *scenery_info = section_header->GetSceneryInfo(instance->SceneryInfoNumber);
                if (scenery_info) {
                    return scenery_info->mHeirarchy;
                }
            }
        }
    }

    return NULL;
}

bHash32 SmokeableSpawner::GetRenderMesh() const {
    SceneryOverrideInfo *soi = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (soi) {
        ScenerySectionHeader *section_header = GetScenerySectionHeader(soi->SectionNumber);
        if (section_header) {
            SceneryInstance *instance = section_header->GetSceneryInstance(soi->InstanceNumber);
            if (instance) {
                SceneryInfo *scenery_info = section_header->GetSceneryInfo(instance->SceneryInfoNumber);
                if (scenery_info) {
                    for (int i = 0; i < 4; i++) {
                        if (scenery_info->NameHash[i] != 0) {
                            return bHash32(scenery_info->NameHash[i]);
                        }
                    }
                }
            }
        }
    }

    return bHash32(0x0C7395A8);
}

void SmokeableSpawner::ShowInstance() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info) {
        info->SetExcludeFlags(0xFFEF, 0);
    }
}

bool SmokeableSpawner::IsInstanceVisible() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info) {
        return (info->ExcludeFlags & 0x10) == 0;
    }

    return false;
}

void SmokeableSpawner::HideInstance() const {
    SceneryOverrideInfo *info = GetSceneryOverrideInfo(mSceneryOverrideInfoNumber);
    if (info) {
        info->SetExcludeFlags(0xFFFF, 0x10);
    }
}

void SmokeableSpawner::OnMoved() {
    if (mSimModel) {
        mSimModel->SetSpawner(this);
    }
}

void SmokeableSpawner::OnLoad(unsigned int exclude_flags, bool ignore) {
    if (!(exclude_flags & mExcludeFlags)) {
        ShowInstance();

        const Attrib::Collection *collection = FindAttributes(mAttributes);

        mSimModel = collection ? SceneryModel::Construct(this, collection, ignore) : NULL;
    }

    if (mSimModel == NULL) {
        HideInstance();
    }
}

int SmokeableSpawnerPack::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x34027) {
        SmokeableSpawnerPack *spawner_pack = (SmokeableSpawnerPack *) chunk->GetAlignedData(16);

        if (AreChunksBeingMoved()) {
            spawner_pack->OnMoved();
        } else {
            spawner_pack->EndianSwap();

            if (Sim::Exists()) {
                unsigned int exclude_flags = Sim::GetUserMode() == true;

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
            ((SmokeableSpawnerPack *) chunk->GetAlignedData(16))->OnUnload();
        }

        return 1;
    }

    return 0;
}

bChunkLoader SmokeableSpawnerPack::mLoader(0x34027, SmokeableSpawnerPack::Loader, SmokeableSpawnerPack::Unloader);

IPlaceableScenery *IPlaceableScenery::CreateInstance(const char *name, Attrib::Key attributes) {
    return PlaceableScenery::Construct(name, attributes);
}

