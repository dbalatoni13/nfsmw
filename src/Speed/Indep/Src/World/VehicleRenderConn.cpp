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
#include "Speed/Indep/Src/World/Rain.hpp"

#ifdef CLANGD_DAMNIT
#include "CarRender.cpp"
#endif

IMPLEMENT_LISTABLE(VehicleRenderConn, 10);

VehicleRenderConn::Effect::Effect(const bMatrix4 *matrix) {
    mLocalMatrix = *matrix;
    this->mEmitterGroup = nullptr;
    this->mKey = 0;
}

VehicleRenderConn::Effect::~Effect() {
    if (this->mEmitterGroup != nullptr) {
        this->mEmitterGroup->UnSubscribe();
        if (this->mEmitterGroup != nullptr) {
            delete this->mEmitterGroup;
        }
        this->mEmitterGroup = nullptr;
    }
}

void VehicleRenderConn::Effect::Stop() {
    if (this->mEmitterGroup != nullptr) {
        this->mEmitterGroup->Disable();
    }
}

void HandleEmitterGroupDelete(void *effect, EmitterGroup *grp) {
    VehicleRenderConn::Effect *car_fx = static_cast<VehicleRenderConn::Effect *>(effect);
    car_fx->ResetEmitterGroup();
}

void VehicleRenderConn::Effect::Fire(const bMatrix4 *worldmatrix, unsigned int emitterkey, float emitter_intensity,
                                     const bVector3 *inherit_velocity) const {
    Attrib::Gen::emittergroup group(emitterkey, 0, nullptr);

    if (!group.IsValid()) {
        return;
    }
    EmitterGroup *emitter_group = gEmitterSystem.CreateEmitterGroup(group.GetConstCollection(), 0x10c00000);

    if (emitter_group == nullptr) {
        return;
    }

    emitter_group->SetIntensity(emitter_intensity);
    emitter_group->MakeOneShot(true);
    emitter_group->SetAutoUpdate(true);

    if (worldmatrix != nullptr) {
        bMatrix4 matrix;
        eMulMatrix(&matrix, const_cast<bMatrix4 *>(&this->mLocalMatrix), const_cast<bMatrix4 *>(worldmatrix));
        emitter_group->SetLocalWorld(&matrix);
    } else {
        emitter_group->SetLocalWorld(&this->mLocalMatrix);
    }

    if (inherit_velocity != nullptr) {
        emitter_group->SetInheritVelocity(inherit_velocity);
    }
}

void VehicleRenderConn::Effect::Update(const bMatrix4 *worldmatrix, unsigned int emitterkey, float dT, float emitter_intensity,
                                       const bVector3 *inherit_velocity) {
    if (emitterkey != this->mKey) {
        if (this->mEmitterGroup != nullptr) {
            this->mEmitterGroup->UnSubscribe();
            if (this->mEmitterGroup != nullptr) {
                delete this->mEmitterGroup;
            }
            this->mEmitterGroup = nullptr;
        }
        this->mKey = emitterkey;
        if (emitterkey == 0) {
            return;
        }
        Attrib::Gen::emittergroup group(emitterkey, 0, nullptr);
        if (!group.IsValid()) {
            return;
        }
        this->mEmitterGroup = gEmitterSystem.CreateEmitterGroup(group.GetConstCollection(), 0x10800000);
        if (this->mEmitterGroup != nullptr) {
            this->mEmitterGroup->SubscribeToDeletion(this, HandleEmitterGroupDelete);
        }
    }
    if (this->mEmitterGroup != nullptr) {
        if (worldmatrix != nullptr) {
            bMatrix4 matrix;
            eMulMatrix(&matrix, const_cast<bMatrix4 *>(&this->mLocalMatrix), const_cast<bMatrix4 *>(worldmatrix));
            this->mEmitterGroup->SetLocalWorld(&matrix);
        } else {
            this->mEmitterGroup->SetLocalWorld(&this->mLocalMatrix);
        }
        this->mEmitterGroup->Enable();
        this->mEmitterGroup->SetIntensity(emitter_intensity);
        this->mEmitterGroup->MakeOneShot(false);
        if (inherit_velocity != nullptr) {
            this->mEmitterGroup->SetInheritVelocity(inherit_velocity);
        }
        this->mEmitterGroup->Update(dT);
    }
}

VehicleRenderConn::VehicleRenderConn(const Sim::ConnectionData &data, CarType type)
    : Sim::Connection(data),                                                     //
      mAttributes(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), //
      mState(S_None),                                                            //
      mCarType(type),                                                            //
      mWorldRef(0),                                                              //
      mModelOffset(0.0f, 0.0f, 0.0f, 0.0f) {
    this->mRideInfo = nullptr;
    this->mRenderInfo = nullptr;
    this->mHide = false;
    this->mWCollider = nullptr;
    this->mSkinSlot = 0;

    CarTypeInfo *car_type_info = GetCarTypeInfo(type);
    this->mAttributes.ChangeWithDefault(Attrib::StringToLowerCaseKey(car_type_info->BaseModelName));
}

void VehicleRenderConn::OnClose() {
    delete this;
}
bool VehicleRenderConn::IsViewAnchor(eView *view) const {
    if (view != nullptr) {
        CameraMover *mover = view->GetCameraMover();
        if (mover != nullptr) {
            CameraAnchor *ancor = mover->GetAnchor();
            if (ancor != nullptr && ancor->GetWorldID() == this->GetWorldID()) {
                return true;
            }
        }
    }

    return false;
}

bool VehicleRenderConn::IsViewAnchor() const {
    return this->IsViewAnchor(eGetView(1, false)) || this->IsViewAnchor(eGetView(2, false));
}

static const bool Tweak_EnableTireRain = true;
static const float Tweak_MaxDistForAheadRain = 10.0f;
static const float Tweak_MaxDistForTireRain = 60.0f;
static const float Tweak_MinRainForTireFX = 0.0f;

bool VehicleRenderConn::CheckForRain(eView *view) const {
    const bVector3 *position;
    bool rain_in_view = false;

    if (view != nullptr) {
        position = reinterpret_cast<const bVector3 *>(&this->mWorldRef.GetMatrix()->v3);
        Rain *rain = view->Precipitation;

        if (rain != nullptr && Tweak_MinRainForTireFX < rain->GetRainIntensity()) {
            CameraMover *mover = view->GetCameraMover();

            if (mover != nullptr) {
                float distance = mover->GetDistanceTo(position);

                if (distance < Tweak_MaxDistForTireRain) {
                    if (distance < Tweak_MaxDistForAheadRain) {
                        return rain->NoRain == 0;
                    }

                    if (mover->GetAnchorID() == this->GetWorldID()) {
                        return rain->NoRain == 0;
                    } else {
                        return rain->NoRainAhead == 0;
                    }
                }
            }
        }
    }

    return false;
}

bool VehicleRenderConn::CheckForRain() const {
    return this->CheckForRain(eGetView(1, false)) || this->CheckForRain(eGetView(2, false));
}

VehicleRenderConn *VehicleRenderConn::Find(unsigned int worldid) {
    for (VehicleRenderConn::List::const_iterator iter = VehicleRenderConn::GetList().begin(); iter != VehicleRenderConn::GetList().end(); ++iter) {
        VehicleRenderConn *vrc = *iter;

        if (vrc->mWorldRef.GetWorldID() == worldid) {
            return vrc;
        }
    }

    return nullptr;
}

void VehicleRenderConn::HandleEvent(EventID id) {
    if (this->CanUpdate()) {
        this->OnEvent(id);
    }
}

void VehicleRenderConn::FetchData(float dT) {
    const VehicleRenderConn::List &carlist = VehicleRenderConn::GetList();

    for (VehicleRenderConn *const *iter = carlist.begin(); iter != carlist.end(); ++iter) {
        (*iter)->OnFetch(dT);
    }
}

void VehicleRenderConn::UpdateLoading() {
    const VehicleRenderConn::List &carlist = VehicleRenderConn::GetList();
    for (VehicleRenderConn *const *iter = carlist.begin(); iter != carlist.end(); ++iter) {
        VehicleRenderConn *conn = *iter;

        if (conn->mState == S_Loading && TheCarLoader.IsLoaded(conn->mRideInfo->GetCarLoaderHandle())) {
            conn->OnLoaded(new ("CarRenderInfo", 0) CarRenderInfo(conn->mRideInfo));
        }
    }
}

VehicleRenderConn::~VehicleRenderConn() {
    CarRenderInfo *carrender_info = this->GetRenderInfo();
    if (carrender_info != nullptr) {
        carrender_info->SetCollider(nullptr);
    }
    if (this->mWCollider != nullptr) {
        WCollider::Destroy(this->mWCollider);
        this->mWCollider = nullptr;
    }
    this->Unload();
}

bool VehicleRenderConn::CanRender() const {
    return this->mHide == false && this->mState == S_Updated;
}

unsigned int VehicleRenderConn::FindPart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    unsigned int modelid = 0;
    if (this->mRenderInfo != nullptr) {
        modelid = this->mRenderInfo->FindCarPart(slot);
    }

    return modelid;
}

unsigned int VehicleRenderConn::HidePart(CAR_PART_ID slot) {
    if (slot < 0) {
        return 0;
    }

    unsigned int modelid = 0;
    if (this->mRenderInfo != nullptr) {
        modelid = this->mRenderInfo->HideCarPart(slot, true);
    }

    return modelid;
}

void VehicleRenderConn::ShowPart(CAR_PART_ID slot) {
    if (this->mRenderInfo != nullptr) {
        this->mRenderInfo->HideCarPart(slot, false);
    }
}

bool VehicleRenderConn::CanUpdate() const {
    return this->mState >= S_Loaded;
}

void VehicleRenderConn::Update(float) {
    if (this->CanUpdate()) {
        this->mState = S_Updated;
        this->mHide = false;
    }
}

void VehicleRenderConn::SetupLoading(bool commit) {
    int car_loader_handle = TheCarLoader.Load(this->mRideInfo);
    this->mRideInfo->SetCarLoaderHandle(car_loader_handle);
    this->mRideInfo->InstanceIndex = 0;

    if (commit) {
        new ECommitRenderAssets();
    }

    this->mState = S_Loading;
}

void VehicleRenderConn::OnLoaded(CarRenderInfo *render_info) {
    const CollisionGeometry::Collection *col;

    this->mState = S_Loaded;
    if (render_info != nullptr) {
        if (this->mWCollider == nullptr) {
            this->mWCollider = WCollider::Create(this->GetWorldID(), WCollider::kColliderShape_Cylinder, 0x1C, 0);
        }
        render_info->SetCollider(this->mWCollider);
    }
    this->mRenderInfo = render_info;
    render_info->Init();

    col = CollisionGeometry::Lookup(GetCarTypeInfo(this->mCarType)->GetName());
    if (col != nullptr) {
        const CollisionGeometry::Bounds *root = col->GetRoot();
        if (root != nullptr) {
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

void VehicleRenderConn::RefreshRenderInfo() {
    this->mRenderInfo->Refresh();
}

int SkinSlotToMask(int slot) {
    return 1 << (slot - 1);
}

int VehicleRenderConn::mOpenSkinSlots = 4095;

bool VehicleRenderConn::Load(unsigned int worldID, CarRenderUsage usage, bool commit, const FECustomizationRecord *customizations) {
    if (this->mRenderInfo != nullptr || this->mCarType == CARTYPE_NONE) {
        return false;
    }

    this->mWorldRef.Set(worldID);
    this->mRideInfo = new ("RideInfo", 0) RideInfo;
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
    if (customizations != nullptr) {
        customizations->WriteRecordIntoRide(this->mRideInfo);
    }
    this->SetupLoading(commit);
    return true;
}

void VehicleRenderConn::Unload() {
    if (this->mState != S_None) {
        if (this->mRideInfo != nullptr) {
            TheCarLoader.Unload(this->mRideInfo->GetCarLoaderHandle());
            delete this->mRideInfo;
            this->mRideInfo = nullptr;
            if (this->mSkinSlot != 0) {
                mOpenSkinSlots |= SkinSlotToMask(this->mSkinSlot);
                this->mSkinSlot = 0;
            }
        }
        this->mWorldRef.Unlock();
        if (this->mRenderInfo != nullptr) {
            delete this->mRenderInfo;
            this->mRenderInfo = nullptr;
        }
        this->mState = S_None;
    }
}

void VehicleRenderConn::RenderAll(eView *view, int reflection) {
    for (VehicleRenderConn *const *iter = VehicleRenderConn::GetList().begin(); iter != VehicleRenderConn::GetList().end(); ++iter) {
        VehicleRenderConn *conn = *iter;
        conn->OnRender(view, reflection);
    }
}

int FlareDiv = 8;
static const float baseStreak = 0.5f;

void VehicleRenderConn::GetRenderMatrix(bMatrix4 *matrix) {
    *matrix = *this->GetBodyMatrix();

    bVector4 offset = this->GetModelOffset();

    bVector4 rotOffset;
    eMulVector(&rotOffset, this->GetBodyMatrix(), &offset);

    matrix->v3.x -= rotOffset.x;
    matrix->v3.y -= rotOffset.y;
    matrix->v3.z -= rotOffset.z;
}

void VehicleRenderConn::RenderFlares(eView *view, int reflection, int renderFlareFlags) {
    for (VehicleRenderConn *const *iter = VehicleRenderConn::GetList().begin(); iter != VehicleRenderConn::GetList().end(); ++iter) {
        VehicleRenderConn *conn = *iter;

        if (!conn->CanRender()) {
            continue;
        }

        CarRenderInfo *car_render_info = conn->GetRenderInfo();
        if (car_render_info == nullptr) {
            continue;
        }

        bMatrix4 render_matrix;
        bVector3 offset2;
        CameraMover *mover = nullptr;

        conn->GetRenderMatrix(&render_matrix);
        offset2.x = render_matrix.v3.x;
        offset2.y = render_matrix.v3.y;
        offset2.z = render_matrix.v3.z;

        mover = view->GetCameraMover();

        if (mover != nullptr && !mover->RenderCarPOV()) {
            CameraAnchor *anchor = mover->GetAnchor();

            if (anchor != nullptr && anchor->GetWorldID() == conn->GetWorldID()) {
                continue;
            }
        }

        car_render_info->RenderFlaresOnCar(view, &offset2, &render_matrix, 0, reflection, renderFlareFlags);

        CarRenderInfo *info = conn->GetRenderInfo();
        info->pCarTypeInfo->GetCarUsageType();
        if (reflection == 0) {
            float NOSamount;
            if (view->GetID() == 1 || view->GetID() == 2) {
                if (info->matrixIndex < 0) {
                    info->matrixIndex = 0;
                }
                info->matrixIndex++;
                if (info->matrixIndex > 2) {
                    info->matrixIndex = 0;
                }
                info->LastFewMatrices[info->matrixIndex] = render_matrix;
                info->LastFewPositions[info->matrixIndex] = offset2;
            }

            if (0.2f < bLength(conn->GetVelocity()) && car_render_info->NOSstate != 0) {
                for (int i = 3; i > 0; --i) {
                    int mIndex = (i + info->matrixIndex + 1) % 3;
                    int mIndex2 = (i + info->matrixIndex + 2) % 3;
                    bVector3 delta = info->LastFewPositions[mIndex2] - info->LastFewPositions[mIndex];

                    for (int j = 0; j < FlareDiv; j++) {
                        float mul = static_cast<float>(j + 1) / static_cast<float>(FlareDiv);
                        bVector3 *d2 = static_cast<bVector3 *>(eFrameMalloc(sizeof(bVector3)));

                        if (d2 != nullptr) {
                            *d2 = delta;
                            *d2 *= mul;
                            *d2 += info->LastFewPositions[mIndex];
                            info->RenderFlaresOnCar(view, d2, &info->LastFewMatrices[mIndex], 8, reflection, 2);
                        }
                    }
                }
            }
        }
    }
}

void RefreshAllRenderInfo(CarType type) {
    for (VehicleRenderConn ::List::const_iterator it = VehicleRenderConn::GetList().begin(); it != VehicleRenderConn::GetList().end(); ++it) {
        VehicleRenderConn *conn = *it;

        if ((type == CARTYPE_NONE || conn->GetCarType() == type) && conn->mState >= VehicleRenderConn::S_Loaded) {
            conn->RefreshRenderInfo();
        }
    }

    RefreshAllFrontEndCarRenderInfos(type);
}
