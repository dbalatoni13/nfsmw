#include "./SmackableRender.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Sim/SimServer.H"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "WorldConn.h"
#include "WorldModel.hpp"
#include "dolphin/mtx/GeoTypes.h"
#include "dolphin/types.h"

// UNSOLVED, i hate constructors
SmackableRenderConn::SmackableRenderConn(const Sim::ConnectionData &data /* r27 */)
    : Sim::Connection(data), mTarget(0), mModelHash((unsigned int)0), mLOD(0),
    mModelOffset(bVector4(0.0f, 0.0f, 0.0f, 0.0f))
{
    this->mList.AddTail((bNode *)this);

    Sim::Pkt_Smackable_Open *oc = data.pkt->Cast();
    this->mTarget.Set(oc->mModelHash.GetValue());

    this->mHeirarchy = oc->mHeirarchy;
    this->mModelHash = oc->mModelHash;
    this->mRenderNode = oc->mRenderNode;

    const CollisionGeometry::Bounds *bounds = oc->mCollisionNode;
}

// UNSOLVED, this function does some weird shit
void SmackableRenderConn::Update(float dT) {
    if (*(const bMatrix4 **)(&this->mTarget + 1)) {
        bVector4 tmp;

        PSMTX44Copy(
            *reinterpret_cast<const Mtx44 *>(this->mTarget.GetMatrix()),
            *reinterpret_cast<Mtx44 *>(&this->mRenderMatrix)
        );
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
    }
}

void SmackableRenderConn::UpdateAll(float dT) {
    for (SmackableRenderConn *w = SmackableRenderConn::mList.GetHead(); w != SmackableRenderConn::mList.GetHead(); w = w->GetNext()) {
        w->Update(dT);
    }
}

void SmackableRender_Init() {

}

void SmackableRender_Shutdown() {
    
}

void SmackableRender_Service(float dT) {
    SmackableRenderConn::UpdateAll(dT);
}
