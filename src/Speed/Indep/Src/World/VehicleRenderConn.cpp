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

static void HandleEmitterGroupDelete(void *subscriber, EmitterGroup *) {
    VehicleRenderConn::Effect *effect = static_cast<VehicleRenderConn::Effect *>(subscriber);
    effect->mEmitterGroup = 0;
}

UTL::Collections::Listable<VehicleRenderConn, 10>::List UTL::Collections::Listable<VehicleRenderConn, 10>::_mTable;

int SkinSlotToMask(int slot) {
    return 1 << ((slot - 1U) & 0x3F);
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
    this->mAttributes.Change(Attrib::FindCollectionWithDefault(
        Attrib::Gen::ecar::ClassKey(), Attrib::StringToLowerCaseKey(CarTypeInfoArray[type].BaseModelName)));
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

    if (this->mRenderInfo != 0) {
        return this->mRenderInfo->FindCarPart(slot);
    }

    return 0;
}

unsigned int VehicleRenderConn::HidePart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    if (this->mRenderInfo != 0) {
        return this->mRenderInfo->HideCarPart(slot, true);
    }

    return 0;
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
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &loader_list = VehicleRenderConn::GetList();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator it = loader_list.begin();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator end = loader_list.end();

    for (; it != end; ++it) {
        (*it)->OnFetch(dT);
    }
}

void VehicleRenderConn::UpdateLoading() {
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &loader_list = VehicleRenderConn::GetList();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator it = loader_list.begin();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator end = loader_list.end();

    for (; it != end; ++it) {
        VehicleRenderConn *vehicle_render_conn = *it;
        RideInfoLoaderMirror *ride_info = reinterpret_cast<RideInfoLoaderMirror *>(vehicle_render_conn->mRideInfo);

        if (vehicle_render_conn->mState == S_Loading && CarLoader_IsLoaded(&TheCarLoader, ride_info->mMyCarLoaderHandle)) {
            vehicle_render_conn->OnLoaded(new CarRenderInfo(vehicle_render_conn->mRideInfo));
        }
    }
}

bool VehicleRenderConn::Load(unsigned int worldID, CarRenderUsage usage, bool commit, const FECustomizationRecord *customizations) {
    if (this->mRenderInfo != 0 || this->mCarType == CARTYPE_NONE) {
        return false;
    }

    this->mWorldRef.Set(worldID);
    this->mRideInfo = new RideInfo;
    this->mRideInfo->Init(this->mCarType, usage, 0, 0);

    if (CarInfo_IsSkinned(this->mCarType)) {
        int skin_slot = UsePrecompositeVinyls == 0 ? 1 : 5;
        int end_skin_slot = UsePrecompositeVinyls == 0 ? 5 : 13;

        while (skin_slot < end_skin_slot) {
            unsigned int skin_mask = SkinSlotToMask(skin_slot);
            if (mOpenSkinSlots & skin_mask) {
                mOpenSkinSlots &= ~skin_mask;
                this->mSkinSlot = skin_slot;
                break;
            }
            skin_slot++;
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
    RideInfoLoaderMirror *ride_info = reinterpret_cast<RideInfoLoaderMirror *>(this->mRideInfo);

    ride_info->mMyCarLoaderHandle = CarLoader_Load(&TheCarLoader, this->mRideInfo);
    ride_info->InstanceIndex = 0;

    if (commit) {
        new ECommitRenderAssets();
    }

    this->mState = S_Loading;
}

void VehicleRenderConn::OnEvent(EventID) {}

void VehicleRenderConn::OnLoaded(CarRenderInfo *render_info) {
    const CollisionGeometry::Collection *collision_geometry;
    const CollisionGeometry::Bounds *root;

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

    collision_geometry = reinterpret_cast<const CollisionGeometry::Collection *>(
        CollisionGeometry::Lookup(UCrc32(stringhash32(CarTypeInfoArray[this->mCarType].CarTypeName))));
    if (collision_geometry == 0) {
        this->mModelOffset.x = render_info->ModelOffset.x;
        this->mModelOffset.y = render_info->ModelOffset.y;
        this->mModelOffset.z = render_info->ModelOffset.z;
        this->mModelOffset.w = 0.0f;
    } else {
        root = CollisionGeometry_Collection_GetRoot(collision_geometry);
        if (root != 0) {
            this->mModelOffset.x = static_cast< float >(root->fPivot.z) * 0.001f;
            this->mModelOffset.y = -static_cast< float >(root->fPivot.x) * 0.001f;
            this->mModelOffset.z = static_cast< float >(root->fPivot.y) * 0.001f;
            render_info->SetRadius(root->fRadius);
        }
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
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &loader_list = VehicleRenderConn::GetList();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator it = loader_list.begin();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator end = loader_list.end();

    for (; it != end; ++it) {
        (*it)->OnRender(view, reflection);
    }
}

void VehicleRenderConn::RenderFlares(eView *view, int reflection, int renderFlareFlags) {
    const UTL::Collections::Listable<VehicleRenderConn, 10>::List &loader_list = VehicleRenderConn::GetList();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator it = loader_list.begin();
    UTL::Collections::Listable<VehicleRenderConn, 10>::List::const_iterator end = loader_list.end();

    for (; it != end; ++it) {
        VehicleRenderConn *vehicle_render_conn = *it;
        CarRenderInfo *render_info = vehicle_render_conn->mRenderInfo;

        if (vehicle_render_conn->CanRender() && render_info != 0) {
            const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&vehicle_render_conn->mWorldRef);
            bMatrix4 matrix;
            bVector3 position;
            CameraMover *camera_mover = view->GetCameraMover();

            vehicle_render_conn->GetRenderMatrix(&matrix);
            position.x = matrix.v3.x;
            position.y = matrix.v3.y;
            position.z = matrix.v3.z;

            if (camera_mover != 0 && !camera_mover->RenderCarPOV()) {
                CameraAnchor *anchor = camera_mover->GetAnchor();

                if (anchor != 0 && anchor->GetWorldID() == world_ref->mWorldID) {
                    continue;
                }
            }

            render_info->RenderFlaresOnCar(view, &position, &matrix, 0, reflection, renderFlareFlags);

            if (reflection == 0) {
                if (view->GetID() == 1 || view->GetID() == 2) {
                    if (render_info->matrixIndex < 0) {
                        render_info->matrixIndex = 0;
                    }
                    render_info->matrixIndex++;
                    if (render_info->matrixIndex > 2) {
                        render_info->matrixIndex = 0;
                    }
                    bCopy(&render_info->LastFewMatrices[render_info->matrixIndex], &matrix);
                    render_info->LastFewPositions[render_info->matrixIndex] = position;
                }

                {
                    const bVector3 *velocity = reinterpret_cast<const ReferenceMirror *>(&vehicle_render_conn->mWorldRef)->mVelocity;
                    float speed_sq = velocity->y * velocity->y + velocity->x * velocity->x + velocity->z * velocity->z;
                    float speed = bSqrt(speed_sq);

                    if (0.1f < speed && render_info->NOSstate != 0) {
                        for (int streak = 3; streak > 1; --streak) {
                            int history_index = render_info->matrixIndex + streak;
                            int next_index = (history_index + 1) % 3;
                            int current_index = (history_index + 2) % 3;
                            bVector3 delta = render_info->LastFewPositions[current_index] - render_info->LastFewPositions[next_index];
                            bVector3 flare_position(delta);

                            for (int div = 0; div < FlareDiv; div++) {
                                float t = static_cast<float>(div + 1) / static_cast<float>(FlareDiv);
                                bVector3 point(flare_position);

                                point *= t;
                                point += render_info->LastFewPositions[next_index];
                                render_info->RenderFlaresOnCar(view, &point, &render_info->LastFewMatrices[next_index], 8, 0, 2);
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
