#include "./VehicleRenderConn.h"
#include "./CarLoader.hpp"
#include "./CarInfo.hpp"
#include "./WCollider.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Generated/Events/ECommitRenderAssets.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"

int CarLoader_Load(CarLoader *car_loader, RideInfo *ride_info) asm("Load__9CarLoaderP8RideInfo");
int CarLoader_IsLoaded(CarLoader *car_loader, int handle) asm("IsLoaded__9CarLoaderi");
unsigned int CameraMover_GetAnchorID() asm("GetAnchorID__11CameraMover");
const CollisionGeometry::Bounds *CollisionGeometry_Collection_GetRoot(const CollisionGeometry::Collection *collection)
    asm("GetRoot__CQ217CollisionGeometry10Collection");
extern CarTypeInfo *CarTypeInfoArray;
extern eView eViews[];

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

UTL::Collections::Listable<VehicleRenderConn, 10>::List UTL::Collections::Listable<VehicleRenderConn, 10>::_mTable;

int SkinSlotToMask(int slot) {
    return 1 << ((slot - 1U) & 0x3F);
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
    return this->IsViewAnchor(&eViews[0]) || this->IsViewAnchor(&eViews[1]);
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
    return this->CheckForRain(&eViews[0]) || this->CheckForRain(&eViews[1]);
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

void VehicleRenderConn::Update(float) {
    if (this->CanUpdate()) {
        *reinterpret_cast<int *>(&this->mHide) = 0;
        this->mState = S_Updated;
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

void VehicleRenderConn::RefreshRenderInfo() {
    this->mRenderInfo->Refresh();
}
