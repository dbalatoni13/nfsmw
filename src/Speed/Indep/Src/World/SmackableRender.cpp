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

#include <types.h>

// UNSOLVED, i hate constructors
SmackableRenderConn::SmackableRenderConn(const Sim::ConnectionData &data /* r27 */)
    : Sim::Connection(data), mTarget(0), mModelHash((unsigned int)0), mLOD(0), mModelOffset(bVector4(0.0f, 0.0f, 0.0f, 0.0f)) {
    this->mList.AddTail(this);

    RenderConn::Pkt_Smackable_Open *oc = Sim::Packet::Cast<RenderConn::Pkt_Smackable_Open>(data.pkt);
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

SlotPool *SpaceNodeSlotPool = nullptr; // move elsewhere
SlotPool *WorldModelSlotPool = nullptr; // move elsewhere

// UNSOLVED, this function does some weird shit
void SmackableRenderConn::Update(float dT) {
    if (this->mTarget.IsValid()) {
        bVector4 tmp;

        this->mRenderMatrix = *this->mTarget.GetMatrix();
        eMulVector(&tmp, this->mTarget.GetMatrix(), &this->mModelOffset);

        this->mRenderMatrix.v3 += tmp;
        this->mRenderMatrix.v3.w = 1.0f;

        float disttoview = 0.0f;
        bool inview = false;
        if (this->mModel && this->mModel->IsEnabled()) {
            disttoview = this->mModel->DistanceToGameView();
            
            if (this->mModel->GetLastVisibleFrame() >= this->mModel->GetLastRenderFrame() && this->mModel->GetLastRenderFrame() != 0) {
                inview = true;
            }
        }

        RenderConn::Pkt_Smackable_Service pkt = RenderConn::Pkt_Smackable_Service(inview, disttoview);

        if (!this->Service(&pkt)) {
            if (this->mModel != nullptr) {
                delete this->mModel;
                this->mModel = nullptr;
            }
        } else {
            if (!this->mModel) {
                if (this->mHeirarchy && this->mRenderNode != 0) {
                    this->mModel = new WorldModel(this->mHeirarchy, this->mRenderNode, true);
                } else {
                    this->mModel = new WorldModel(this->mModelHash.GetValue(), nullptr, true);
                }
            }

            this->mModel->SetChildVisibility(pkt.mChildVisibility);
            this->mModel->SetMatrix(&this->mRenderMatrix);
        }
    }
}

void SmackableRenderConn::UpdateAll(float dT) {
    for (SmackableRenderConn *w = mList.GetHead(); w != mList.GetHead(); w = w->GetNext()) {
        w->Update(dT);
    }
}

void SmackableRender_Init() {}

void SmackableRender_Shutdown() {}

void SmackableRender_Service(float dT) {
    SmackableRenderConn::UpdateAll(dT);
}

bTList<SmackableRenderConn> SmackableRenderConn::mList;
// Prototype _SmackableRenderConn;
