#include "./HeliRenderConn.h"
#include "./CarInfo.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

BIND_SIM_CONN(HeliRenderConn);

Sim::Connection *HeliRenderConn::Construct(const Sim::ConnectionData &data) {
    RenderConn::Pkt_Heli_Open *oc = reinterpret_cast<RenderConn::Pkt_Heli_Open *>(data.pkt);
    CarType ct = CarPartDB.GetCarType(oc->mModelHash);

    if (ct == CARTYPE_NONE || ct >= NUM_CARTYPES) {
        return nullptr;
    }

    return new HeliRenderConn(data, ct, oc);
}

HeliRenderConn::HeliRenderConn(const Sim::ConnectionData &data, CarType ct, RenderConn::Pkt_Heli_Open *oc)
    : VehicleRenderConn(data, ct), mLastVisibleFrame(0), //
      mDistanceToView(0.0f),                             //
      mShadowScale(1.0f),                                //
      mLastRenderFrame(0) {
    for (int i = 0; i < NUM_ELEMENTS(this->mMatrices); i++) {
        eIdentity(&this->mMatrices[i]);
    }

    this->Load(oc->mWorldID, CarRenderUsage_AIHeli, !oc->mSpoolLoad, 0);
}

HeliRenderConn::~HeliRenderConn() {}

void HeliRenderConn::Update(const RenderConn::Pkt_Heli_Service &data, float dT) {
    CarRenderInfo *car_render_info;

    if (!this->CanUpdate()) {
        return;
    }

    car_render_info = this->GetRenderInfo();
    if (car_render_info == nullptr) {
        return;
    }

    this->mShadowScale = data.mShadowScale;

    bVector4 offset(this->GetModelOffset());
    this->mGeomMatrix = *this->GetBodyMatrix();

    bVector4 rotOffset;
    eMulVector(&rotOffset, this->GetBodyMatrix(), &offset);
    this->mGeomMatrix.v3.x -= rotOffset.x;
    this->mGeomMatrix.v3.y -= rotOffset.y;
    this->mGeomMatrix.v3.z -= rotOffset.z;
    this->VehicleRenderConn::Update(dT);
}

void HeliRenderConn::OnFetch(float dT) {
    bool wasdrawn = false;

    if (this->mLastVisibleFrame >= this->mLastRenderFrame && this->mLastRenderFrame != 0) {
        wasdrawn = true;
    }

    RenderConn::Pkt_Heli_Service data(wasdrawn, this->mDistanceToView);
    if (this->Service(&data)) {
        this->Update(data, dT);
    } else {
        this->Hide();
    }
}

void HeliRenderConn::OnRender(eView *view, int reflection) {
    if (this->mLastRenderFrame != eGetFrameCounter()) {
        this->mDistanceToView = 999999.0f;
    }
    this->mLastRenderFrame = eGetFrameCounter();

    if (!this->CanRender()) {
        return;
    }

    CameraMover *mover = view->GetCameraMover();
    if (mover != nullptr) {
        if (!mover->OutsidePOV()) {
            CameraAnchor *anchor = mover->GetAnchor();
            if (anchor != nullptr && anchor->GetWorldID() == this->GetWorldID()) {
                return;
            }
        }

        if (mover != nullptr && eIsGameViewID(view->GetID())) {
            this->mDistanceToView =
                UMath::Min(this->mDistanceToView, mover->GetDistanceTo(reinterpret_cast<const bVector3 *>(&this->mGeomMatrix.v3)));
        }
    }

    CarRenderInfo *car_render_info = this->GetRenderInfo();
    if (car_render_info == nullptr) {
        return;
    }

    EVIEWMODE view_mode = eGetCurrentViewMode();
    if (reflection == 0) {
        bMatrix4 cbm(this->mGeomMatrix);
        int render_flags = 0;
        bVector3 Position(cbm.v3.x, cbm.v3.y, cbm.v3.z);

        cbm.v3.x = 0.0f;
        cbm.v3.y = 0.0f;
        cbm.v3.z = 0.0f;

        if (car_render_info->Render(view, &Position, &cbm, this->mMatrices, this->mMatrices, this->mMatrices, render_flags, reflection, reflection,
                                    this->mShadowScale, car_render_info->GetMinLodLevel(), car_render_info->GetMinLodLevel()) &&
            view->GetID() < 4) {
            this->mLastVisibleFrame = eGetFrameCounter();
        }
    }
}
