#include "./SmackableRender.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "WorldConn.h"
#include "WorldModel.hpp"
#include "dolphin/mtx/GeoTypes.h"

#include <types.h>

// UNSOLVED, i hate constructors
SmackableRenderConn::SmackableRenderConn(const Sim::ConnectionData &data /* r27 */)
    : Sim::Connection(data), mTarget(0), mModelHash((unsigned int)0), mLOD(0), mModelOffset(bVector4(0.0f, 0.0f, 0.0f, 0.0f)) {
    this->mList.AddTail(this);

    Sim::Pkt_Smackable_Open *oc = Sim::Packet::Cast<Sim::Pkt_Smackable_Open>(data.pkt);
    this->mTarget.Set(oc->mModelHash.GetValue());

    this->mHeirarchy = oc->mHeirarchy;
    this->mModelHash = oc->mModelHash;
    this->mRenderNode = oc->mRenderNode;
    this->mModelHash = oc->mModelHash;

    const CollisionGeometry::Bounds *bounds = oc->mCollisionNode;
    UMath::Vector3 pivot;
    bounds->GetPivot(pivot);
}

SmackableRenderConn::~SmackableRenderConn() {
    if (this->mModel) {
        delete this->mModel;
    }
    this->mTarget.Set(0);
    delete this;
}

SlotPool *SpaceNodeSlotPool = nullptr;
SlotPool *WorldModelSlotPool = nullptr;

// UNSOLVED, this function does some weird shit
void SmackableRenderConn::Update(float dT) {
    if (*(const bMatrix4 **)(&this->mTarget + 1)) {
        bVector4 tmp;

        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(this->mTarget.GetMatrix()), *reinterpret_cast<Mtx44 *>(&this->mRenderMatrix));
        eMulVector(&tmp, this->mTarget.GetMatrix(), &this->mModelOffset);

        this->mRenderMatrix.v3 += tmp;

        float disttoview = 0.0f;
        bool inview = false;
        if (this->mModel && this->mModel->IsEnabled()) {
            float disttoview = this->mModel->DistanceToGameView();
            if (this->mModel->GetLastVisibleFrame() >= this->mModel->GetLastRenderFrame() && this->mModel->GetLastRenderFrame() != 0) {
                inview = true;
            }
        }

        Sim::Pkt_Smackable_Service pkt = Sim::Pkt_Smackable_Service(inview, disttoview);

        if (this->Service(&pkt)) {
            if (this->mModel) {
                delete this->mModel;
            }
            return;
        }

        if (!this->mModel) {
            if (this->mHeirarchy && this->mRenderNode != 0) {
                this->mModel = new WorldModel(this->mHeirarchy, this->mRenderNode, true);
            } else {
                this->mModel = new WorldModel(this->mModelHash.GetValue(), nullptr, true);
            }
        }
        this->mModel->SetChildVisibility(0xFFFFFF);
        if (&this->mRenderMatrix) {
            this->mRenderMatrix.v2.z = 1;
            if (this->mModelOffset.y != 0) {
            }
        }
    }
}

void SmackableRenderConn::UpdateAll(float dT) {
    for (SmackableRenderConn *w = SmackableRenderConn::mList.GetHead(); w != SmackableRenderConn::mList.GetHead(); w = w->GetNext()) {
        w->Update(dT);
    }
}

void SmackableRender_Init() {}

void SmackableRender_Shutdown() {}

void SmackableRender_Service(float dT) {
    SmackableRenderConn::UpdateAll(dT);
}
