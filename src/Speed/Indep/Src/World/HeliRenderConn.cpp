#include "./HeliRenderConn.h"
#include "./CarInfo.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

extern CarPartDatabase CarPartDB;
extern CarType GetCarType__15CarPartDatabaseUi(CarPartDatabase *database, unsigned int model_hash)
    asm("GetCarType__15CarPartDatabaseUi");
extern float lbl_8040B0A8;
extern float lbl_8040B0AC;
extern float lbl_8040B0B0;
extern float lbl_8040B0C0;

UTL::COM::Factory<const Sim::ConnectionData &, Sim::Connection, UCrc32>::Prototype _HeliRenderConn("HeliRenderConn",
                                                                                                     HeliRenderConn::Construct);

Sim::Connection *HeliRenderConn::Construct(const Sim::ConnectionData &data) {
    RenderConn::Pkt_Heli_Open *open = reinterpret_cast<RenderConn::Pkt_Heli_Open *>(data.pkt);
    int car_type = GetCarType__15CarPartDatabaseUi(&CarPartDB, open->mModelHash);

    if (car_type == -1 || car_type > 0x53) {
        return 0;
    }

    return new HeliRenderConn(data, static_cast<CarType>(car_type), open);
}

HeliRenderConn::HeliRenderConn(const Sim::ConnectionData &data, CarType type, RenderConn::Pkt_Heli_Open *open)
    : VehicleRenderConn(data, type) {
    mLastVisibleFrame = 0;
    mDistanceToView = lbl_8040B0A8;
    mShadowScale = lbl_8040B0AC;
    mLastRenderFrame = 0;

    for (int i = 0; i <= 3; i++) {
        PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&this->mMatrices[i]));
    }

    this->Load(open->mWorldID, CarRenderUsage_AIHeli, !open->mSpoolLoad, 0);
}

HeliRenderConn::~HeliRenderConn() {}

void HeliRenderConn::Update(const RenderConn::Pkt_Heli_Service &data, float dT) {
    if (this->CanUpdate() && this->GetRenderInfo() != nullptr) {
        this->mShadowScale = data.mShadowScale;
        bVector4 offset(this->GetModelOffset());
        bVector4 rotOffset;

        this->mGeomMatrix = *this->GetBodyMatrix();
        eMulVector(&rotOffset, this->GetBodyMatrix(), &offset);
        this->mGeomMatrix.v3.x -= rotOffset.x;
        this->mGeomMatrix.v3.y -= rotOffset.y;
        this->mGeomMatrix.v3.z -= rotOffset.z;
        this->VehicleRenderConn::Update(dT);
    }
}

void HeliRenderConn::OnFetch(float dT) {
    bool inview = false;

    if (this->mLastVisibleFrame >= this->mLastRenderFrame && this->mLastRenderFrame != 0) {
        inview = true;
    }

    RenderConn::Pkt_Heli_Service pkt(inview, this->mDistanceToView);
    if (this->Service(&pkt)) {
        this->Update(pkt, dT);
    } else {
        *reinterpret_cast<int *>(&this->mHide) = 1;
    }
}

void HeliRenderConn::OnRender(eView *view, int reflection) {
    const ReferenceMirror *world_ref = reinterpret_cast<const ReferenceMirror *>(&this->mWorldRef);
    CameraMover *mover;
    CarRenderInfo *car_render_info;
    EVIEWMODE view_mode;

    if (this->mLastRenderFrame != eFrameCounter) {
        this->mDistanceToView = lbl_8040B0B0;
    }
    this->mLastRenderFrame = eFrameCounter;

    if (!this->CanRender()) {
        return;
    }

    mover = view->GetCameraMover();
    if (mover != 0) {
        if (!mover->OutsidePOV()) {
            CameraAnchor *anchor = mover->GetAnchor();

            if (anchor != 0 && anchor->GetWorldID() == world_ref->mWorldID) {
                return;
            }
        }

        if (static_cast<unsigned int>(view->GetID() - 1) < 3) {
            float distance;

            distance = mover->GetDistanceTo(reinterpret_cast<bVector3 *>(&this->mGeomMatrix.v3));
            if (this->mDistanceToView < distance) {
                distance = this->mDistanceToView;
            }
            this->mDistanceToView = distance;
        }
    }

    car_render_info = this->mRenderInfo;
    if (car_render_info != 0 && ((view_mode = eGetCurrentViewMode()), reflection == 0)) {
        bMatrix4 cbm(this->mGeomMatrix);
        bVector3 position(cbm.v3.x, cbm.v3.y, cbm.v3.z);
        CARPART_LOD lod = car_render_info->GetMinLodLevel();

        cbm.v3.x = lbl_8040B0C0;
        cbm.v3.y = lbl_8040B0C0;
        cbm.v3.z = lbl_8040B0C0;

        if (car_render_info->Render(view, &position, &cbm, this->mMatrices, this->mMatrices, this->mMatrices, 0, reflection, reflection,
                                    this->mShadowScale, lod, lod) &&
            view->GetID() < 4) {
            this->mLastVisibleFrame = eFrameCounter;
        }
    }
}
