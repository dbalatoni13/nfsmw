#include "./VehicleRenderConn.h"
#include "./CarLoader.hpp"
#include "./CarInfo.hpp"
#include "./WCollider.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emittergroup.h"
#include "Speed/Indep/Src/Generated/Events/ECommitRenderAssets.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"

int CarLoader_Load(CarLoader *car_loader, RideInfo *ride_info) asm("Load__9CarLoaderP8RideInfo");
int CarLoader_IsLoaded(CarLoader *car_loader, int handle) asm("IsLoaded__9CarLoaderi");
void CarLoader_Unload(CarLoader *car_loader, int handle) asm("Unload__9CarLoaderi");
void FECustomizationRecord_WriteRecordIntoRide(const FECustomizationRecord *customizations, RideInfo *ride_info)
    asm("WriteRecordIntoRide__C21FECustomizationRecordP8RideInfo");
unsigned int CameraMover_GetAnchorID() asm("GetAnchorID__11CameraMover");
const CollisionGeometry::Bounds *CollisionGeometry_Collection_GetRoot(const CollisionGeometry::Collection *collection)
    asm("GetRoot__CQ217CollisionGeometry10Collection");
extern CarTypeInfo *CarTypeInfoArray;
extern eView eViews[];
extern EmitterSystem gEmitterSystem;
extern int UsePrecompositeVinyls;
extern int FlareDiv;
bool CarInfo_IsSkinned(CarType car_type);

struct RideInfoLoaderMirror {
    CarType Type;
    char InstanceIndex;
    char HasDash;
    char CanBeVertexDamaged;
    char SkinType;
    char pad[0x2FC];
    int mMyCarLoaderHandle;
};

struct ReferenceMirror {
    unsigned int mWorldID;
    const bMatrix4 *mMatrix;
    const bVector3 *mVelocity;
    const bVector3 *mAcceleration;
};

void HandleEmitterGroupDelete(void *effect, EmitterGroup *grp) {
    VehicleRenderConn::Effect *car_fx = static_cast<VehicleRenderConn::Effect *>(effect);
    car_fx->ResetEmitterGroup();
}

UTL::Collections::Listable<VehicleRenderConn, 10>::List UTL::Collections::Listable<VehicleRenderConn, 10>::_mTable;

int SkinSlotToMask(int slot) {
    return 1 << (slot - 1);
}

VehicleRenderConn::Effect::Effect(const bMatrix4 *matrix) {
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(matrix), *reinterpret_cast<Mtx44 *>(&this->mLocalMatrix));
    this->mEmitterGroup = 0;
    this->mKey = 0;
}

VehicleRenderConn::Effect::~Effect() {
    if (this->mEmitterGroup != 0) {
        this->mEmitterGroup->UnSubscribe();
        if (this->mEmitterGroup != 0) {
            delete this->mEmitterGroup;
        }
        this->mEmitterGroup = 0;
    }
}

void VehicleRenderConn::Effect::Stop() {
    if (this->mEmitterGroup != 0) {
        this->mEmitterGroup->Disable();
    }
}

void VehicleRenderConn::Effect::Fire(const bMatrix4 *worldmatrix, unsigned int emitterkey, float emitter_intensity,
                                     const bVector3 *inherit_velocity) const {
    Attrib::Gen::emittergroup emitter_group_spec(emitterkey, 0, nullptr);

    if (emitter_group_spec.IsValid()) {
        EmitterGroup *emitter_group = gEmitterSystem.CreateEmitterGroup(emitter_group_spec.GetConstCollection(), 0x10c00000);
        if (emitter_group != 0) {
            emitter_group->SetIntensity(emitter_intensity);
            emitter_group->MakeOneShot(true);
            if (worldmatrix == 0) {
                emitter_group->SetLocalWorld(&this->mLocalMatrix);
            } else {
                bMatrix4 local_world;
                eMulMatrix(&local_world, const_cast<bMatrix4 *>(&this->mLocalMatrix), const_cast<bMatrix4 *>(worldmatrix));
                emitter_group->SetLocalWorld(&local_world);
            }
            if (inherit_velocity != 0) {
                emitter_group->SetInheritVelocity(inherit_velocity);
            }
        }
    }
}

void VehicleRenderConn::Effect::Update(const bMatrix4 *worldmatrix, unsigned int emitterkey, float dT, float emitter_intensity,
                                       const bVector3 *inherit_velocity) {
    if (emitterkey != this->mKey) {
        if (this->mEmitterGroup != 0) {
            this->mEmitterGroup->UnSubscribe();
            if (this->mEmitterGroup != 0) {
                delete this->mEmitterGroup;
            }
            this->mEmitterGroup = 0;
        }
        this->mKey = emitterkey;
        if (emitterkey != 0) {
            Attrib::Gen::emittergroup emitter_group_spec(emitterkey, 0, nullptr);
            if (emitter_group_spec.IsValid()) {
                this->mEmitterGroup = gEmitterSystem.CreateEmitterGroup(emitter_group_spec.GetConstCollection(), 0x10800000);
                if (this->mEmitterGroup != 0) {
                    this->mEmitterGroup->SubscribeToDeletion(this, HandleEmitterGroupDelete);
                }
            }
        }
    }
    if (this->mEmitterGroup != 0) {
        if (worldmatrix == 0) {
            this->mEmitterGroup->SetLocalWorld(&this->mLocalMatrix);
        } else {
            bMatrix4 local_world;
            eMulMatrix(&local_world, const_cast<bMatrix4 *>(&this->mLocalMatrix), const_cast<bMatrix4 *>(worldmatrix));
            this->mEmitterGroup->SetLocalWorld(&local_world);
        }
        this->mEmitterGroup->Enable();
        this->mEmitterGroup->SetIntensity(emitter_intensity);
        this->mEmitterGroup->MakeOneShot(false);
        if (inherit_velocity != 0) {
            this->mEmitterGroup->SetInheritVelocity(inherit_velocity);
        }
        this->mEmitterGroup->Update(dT);
    }
}

VehicleRenderConn::VehicleRenderConn(const Sim::ConnectionData &data, CarType type)
    : Sim::Connection(data), //
      mAttributes(static_cast<const Attrib::Collection *>(0), 0, 0), //
      mState(S_None), //
      mCarType(type), //
      mWorldRef(0)
{
    this->mSkinSlot = 0;
    this->mRideInfo = 0;
    this->mRenderInfo = 0;
    *reinterpret_cast<int *>(&this->mHide) = 0;
    this->mWCollider = 0;
    this->mModelOffset.w = 0.0f;
    this->mModelOffset.x = 0.0f;
    this->mModelOffset.y = 0.0f;
    this->mModelOffset.z = 0.0f;
    this->mAttributes.ChangeWithDefault(Attrib::StringToLowerCaseKey(CarTypeInfoArray[type].BaseModelName));
}

VehicleRenderConn::~VehicleRenderConn() {
    if (this->mRenderInfo != 0) {
        this->mRenderInfo->SetCollider(0);
    }
    if (this->mWCollider != 0) {
        WCollider::Destroy(this->mWCollider);
        this->mWCollider = 0;
    }
    this->Unload();
}

void VehicleRenderConn::OnClose() {
    delete this;
}

Sim::ConnStatus VehicleRenderConn::OnStatusCheck() {
    return this->mState > S_Loading ? Sim::CONNSTATUS_READY : Sim::CONNSTATUS_CONNECTING;
}

bool VehicleRenderConn::IsViewAnchor(eView *view) const {
    CameraMover *camera_mover;
    CameraAnchor *anchor;
    const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&this->mWorldRef);

    if (view != 0) {
        camera_mover = view->GetCameraMover();
        if (camera_mover != 0) {
            anchor = camera_mover->GetAnchor();
            if (anchor != 0 && anchor->GetWorldID() == world_ref->mWorldID) {
                return true;
            }
        }
    }

    return false;
}

bool VehicleRenderConn::IsViewAnchor() const {
    eView *views = eViews;
    return this->IsViewAnchor(&views[1]) || this->IsViewAnchor(&views[2]);
}

bool VehicleRenderConn::CanRender() const {
    return this->mHide == false && this->mState == S_Updated;
}

VehicleRenderConn *VehicleRenderConn::Find(unsigned int worldid) {
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &list =
        UTL::Collections::Listable<VehicleRenderConn, 10>::GetList();

    for (UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
        VehicleRenderConn *vehicle_render_conn = *iter;

        if (vehicle_render_conn->mWorldRef.GetWorldID() == worldid) {
            return vehicle_render_conn;
        }
    }

    return 0;
}

unsigned int VehicleRenderConn::FindPart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    unsigned int modelid = 0;

    if (this->mRenderInfo != 0) {
        modelid = this->mRenderInfo->FindCarPart(slot);
    }

    return modelid;
}

unsigned int VehicleRenderConn::HidePart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    unsigned int modelid = 0;

    if (this->mRenderInfo != 0) {
        modelid = this->mRenderInfo->HideCarPart(slot, true);
    }

    return modelid;
}

void VehicleRenderConn::ShowPart(CAR_PART_ID slot) {
    if (this->mRenderInfo != 0) {
        this->mRenderInfo->HideCarPart(slot, false);
    }
}

bool VehicleRenderConn::CheckForRain(eView *view) const {
    const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&this->mWorldRef);
    Rain *precipitation;
    CameraMover *camera_mover;
    bVector3 *camera_position;
    float dx;
    float dy;
    float dz;
    float distance_squared;
    float distance;

    if (view != 0) {
        precipitation = view->Precipitation;
        if (precipitation != 0 && 0.0f < precipitation->GetRainIntensity()) {
            camera_mover = view->GetCameraMover();
            if (camera_mover != 0) {
                camera_position = camera_mover->GetPosition();
                dx = camera_position->y - world_ref->mMatrix->v3.y;
                dy = camera_position->x - world_ref->mMatrix->v3.x;
                dz = camera_position->z - world_ref->mMatrix->v3.z;
                distance_squared = dz * dz + dy * dy + dx * dx;
                distance = 0.0f;
                if (0.0f < distance_squared) {
                    distance = bSqrt(distance_squared);
                }
                if (distance < 60.0f) {
                    if (10.0f <= distance && CameraMover_GetAnchorID() != world_ref->mWorldID) {
                        return precipitation->NoRainAhead == 0;
                    }
                    return precipitation->NoRain == 0;
                }
            }
        }
    }

    return false;
}

bool VehicleRenderConn::CheckForRain() const {
    eView *views = eViews;
    return this->CheckForRain(&views[1]) || this->CheckForRain(&views[2]);
}

bool VehicleRenderConn::CanUpdate() const {
    return this->mState > S_Loading;
}

void VehicleRenderConn::HandleEvent(EventID id) {
    if (this->CanUpdate()) {
        this->OnEvent(id);
    }
}

void VehicleRenderConn::FetchData(float dT) {
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &carlist = VehicleRenderConn::GetList();
    {
        VehicleRenderConn *const *iter = carlist.begin();

        while (iter != carlist.end()) {
            (*iter)->OnFetch(dT);
            ++iter;
        }
    }
}

void VehicleRenderConn::UpdateLoading() {
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &carlist = VehicleRenderConn::GetList();
    {
        VehicleRenderConn *const *iter = carlist.begin();

        while (iter != carlist.end()) {
            VehicleRenderConn *conn = *iter;

            if (conn->mState == S_Loading && CarLoader_IsLoaded(&TheCarLoader, conn->mRideInfo->GetCarLoaderHandle())) {
                conn->OnLoaded(new (__FILE__, __LINE__) CarRenderInfo(conn->mRideInfo));
            }

            ++iter;
        }
    }
}

bool VehicleRenderConn::Load(unsigned int worldID, CarRenderUsage usage, bool commit, const FECustomizationRecord *customizations) {
    if (this->mRenderInfo != 0 || this->mCarType == CARTYPE_NONE) {
        return false;
    }

    this->mWorldRef.Set(worldID);
    this->mRideInfo = new (__FILE__, __LINE__) RideInfo;
    this->mRideInfo->Init(this->mCarType, usage, 0, 0);

    if (CarInfo_IsSkinned(this->mCarType)) {
        bool precomposite = false;

        if (0) {
            customizations->IsPreset();
        }

        if (UsePrecompositeVinyls != 0) {
            precomposite = true;
        }

        if (!precomposite) {
            for (int i = 1; i < 5; i++) {
                int mask = SkinSlotToMask(i);
                if (mOpenSkinSlots & mask) {
                    mOpenSkinSlots &= ~mask;
                    this->mSkinSlot = i;
                    break;
                }
            }
        } else {
            for (int i = 5; i < 13; i++) {
                int mask = SkinSlotToMask(i);
                if (mOpenSkinSlots & mask) {
                    mOpenSkinSlots &= ~mask;
                    this->mSkinSlot = i;
                    break;
                }
            }
        }

        if (this->mSkinSlot != 0) {
            this->mRideInfo->SetCompositeNameHash(this->mSkinSlot);
        }
    }

    this->mRideInfo->SetStockParts();
    if (customizations != 0) {
        FECustomizationRecord_WriteRecordIntoRide(customizations, this->mRideInfo);
    }
    this->SetupLoading(commit);
    return true;
}

void VehicleRenderConn::Unload() {
    if (this->mState != S_None) {
        if (this->mRideInfo != 0) {
            RideInfoLoaderMirror *ride_info = reinterpret_cast<RideInfoLoaderMirror *>(this->mRideInfo);

            CarLoader_Unload(&TheCarLoader, ride_info->mMyCarLoaderHandle);
            delete this->mRideInfo;
            this->mRideInfo = 0;
            if (this->mSkinSlot != 0) {
                mOpenSkinSlots |= SkinSlotToMask(this->mSkinSlot);
                this->mSkinSlot = 0;
            }
        }
        this->mWorldRef.Unlock();
        if (this->mRenderInfo != 0) {
            delete this->mRenderInfo;
            this->mRenderInfo = 0;
        }
        this->mState = S_None;
    }
}

void VehicleRenderConn::Update(float) {
    if (this->CanUpdate()) {
        this->mState = S_Updated;
        *reinterpret_cast<int *>(&this->mHide) = 0;
    }
}

void VehicleRenderConn::SetupLoading(bool commit) {
    int car_loader_handle = CarLoader_Load(&TheCarLoader, this->mRideInfo);
    this->mRideInfo->SetCarLoaderHandle(car_loader_handle);
    reinterpret_cast<RideInfoLoaderMirror *>(this->mRideInfo)->InstanceIndex = 0;

    if (commit) {
        new ECommitRenderAssets();
    }

    this->mState = S_Loading;
}

void VehicleRenderConn::OnEvent(EventID) {}

void VehicleRenderConn::OnLoaded(CarRenderInfo *render_info) {
    const CollisionGeometry::Collection *col;

    this->mState = S_Loaded;
    if (render_info != 0) {
        if (this->mWCollider == 0) {
            const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&this->mWorldRef);
            this->mWCollider = WCollider::Create(world_ref->mWorldID, WCollider::kColliderShape_Cylinder, 0x1C, 0);
        }
        render_info->SetCollider(this->mWCollider);
    }
    this->mRenderInfo = render_info;
    render_info->Init();

    col = reinterpret_cast<const CollisionGeometry::Collection *>(
        CollisionGeometry::Lookup(UCrc32(stringhash32(CarTypeInfoArray[this->mCarType].CarTypeName))));
    if (col) {
        const CollisionGeometry::Bounds *root = CollisionGeometry_Collection_GetRoot(col);
        if (root) {
            UMath::Vector3 pivot;
            root->GetPivot(pivot);
            this->mModelOffset.x = pivot.z;
            this->mModelOffset.y = -pivot.x;
            this->mModelOffset.z = pivot.y;
            this->mRenderInfo->SetRadius(root->fRadius);
        }
    } else {
        this->mModelOffset = bVector4(this->mRenderInfo->ModelOffset.x, this->mRenderInfo->ModelOffset.y, this->mRenderInfo->ModelOffset.z, 0.0f);
    }
}

void VehicleRenderConn::GetRenderMatrix(bMatrix4 *matrix) {
    const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&this->mWorldRef);
    bVector4 model_offset;
    bVector4 transformed_offset;

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(world_ref->mMatrix), *reinterpret_cast<Mtx44 *>(matrix));
    model_offset = this->mModelOffset;
    eMulVector(&transformed_offset, world_ref->mMatrix, &model_offset);
    matrix->v3.x -= transformed_offset.x;
    matrix->v3.y -= transformed_offset.y;
    matrix->v3.z -= transformed_offset.z;
}

void VehicleRenderConn::RenderAll(eView *view, int reflection) {
    for (VehicleRenderConn *const *iter = VehicleRenderConn::GetList().begin();
         iter != VehicleRenderConn::GetList().end(); ++iter) {
        VehicleRenderConn *conn = *iter;
        conn->OnRender(view, reflection);
    }
}

void VehicleRenderConn::RenderFlares(eView *view, int reflection, int renderFlareFlags) {
    for (VehicleRenderConn *const *iter = VehicleRenderConn::GetList().begin(); iter != VehicleRenderConn::GetList().end(); ++iter) {
        VehicleRenderConn *conn = *iter;
        CarRenderInfo *info = conn->mRenderInfo;

        if (conn->CanRender() && info != 0) {
            bMatrix4 render_matrix;
            bVector3 offset2;
            CameraMover *mover = nullptr;

            conn->GetRenderMatrix(&render_matrix);
            offset2.x = render_matrix.v3.x;
            offset2.y = render_matrix.v3.y;
            offset2.z = render_matrix.v3.z;

            if (view->CameraMoverList.GetHead() != view->CameraMoverList.EndOfList()) {
                mover = view->CameraMoverList.GetHead();
            }

            if (mover != 0 && !mover->RenderCarPOV()) {
                const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&conn->mWorldRef);
                CameraAnchor *anchor = mover->GetAnchor();

                if (anchor != 0 && anchor->GetWorldID() == world_ref->mWorldID) {
                    continue;
                }
            }

            info->RenderFlaresOnCar(view, &offset2, &render_matrix, 0, reflection, renderFlareFlags);

            if (reflection == 0) {
                if (view->GetID() == 1 || view->GetID() == 2) {
                    if (info->matrixIndex < 0) {
                        info->matrixIndex = 0;
                    }
                    info->matrixIndex++;
                    if (info->matrixIndex > 2) {
                        info->matrixIndex = 0;
                    }
                    bCopy(&info->LastFewMatrices[info->matrixIndex], &render_matrix);
                    info->LastFewPositions[info->matrixIndex] = offset2;
                }

                {
                    float NOSamount = bLength(conn->GetVelocity());

                    if (0.1f < NOSamount && info->NOSstate != 0) {
                        for (int streak = 3; streak > 1; --streak) {
                            int history_index = info->matrixIndex + streak;
                            int next_index = (history_index + 1) % 3;
                            int current_index = (history_index + 2) % 3;
                            bVector3 flare_position = info->LastFewPositions[current_index] - info->LastFewPositions[next_index];

                            for (int div = 0; div < FlareDiv; div++) {
                                float t = static_cast<float>(div + 1) / static_cast<float>(FlareDiv);
                                bVector3 *point = static_cast<bVector3 *>(eFrameMalloc(sizeof(bVector3)));

                                if (point != 0) {
                                    *point = flare_position;
                                    *point *= t;
                                    *point += info->LastFewPositions[next_index];
                                    info->RenderFlaresOnCar(view, point, &info->LastFewMatrices[next_index], 8, reflection, 2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void VehicleRenderConn::RefreshRenderInfo() {
    this->mRenderInfo->Refresh();
}
