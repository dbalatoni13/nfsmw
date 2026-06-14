#include "./SmackableRender.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WorldConn.h"
#include "WorldModel.hpp"

#include <types.h>

// total size: 0x8C
class SmackableRenderConn : public Sim::Connection, public bTNode<SmackableRenderConn> {
  public:
    USE_FASTALLOC(SmackableRenderConn);

    SmackableRenderConn(const Sim::ConnectionData &data);
    virtual ~SmackableRenderConn();

    void OnClose() override {
        delete this;
    }

    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    static Sim::Connection *Construct(const Sim::ConnectionData &data) {
        return new SmackableRenderConn(data);
    }

    static void UpdateAll(float dT);

  private:
    void Update(float dT);

    bHash32 mModelHash;               // offset 0x18, size 0x4
    const ModelHeirarchy *mHeirarchy; // offset 0x1C, size 0x4
    WorldConn::Reference mTarget;     // offset 0x20, size 0x10
    WorldModel *mModel;               // offset 0x30, size 0x4
    uint32 mRenderNode;               // offset 0x34, size 0x4
    bVector4 mModelOffset;            // offset 0x38, size 0x10
    bMatrix4 mRenderMatrix;           // offset 0x48, size 0x40
    int mLOD;                         // offset 0x88, size 0x4
    static bTList<SmackableRenderConn> mList;
};

SmackableRenderConn::SmackableRenderConn(const Sim::ConnectionData &data)
    : Sim::Connection(data), mModelHash(), mTarget((*reinterpret_cast<unsigned int *>(&mModelHash) = 0, 0)), mModel(nullptr), mLOD(0),
      mModelOffset(bVector4(0.0f, 0.0f, 0.0f, 0.0f)) {
    this->mList.AddTail(this);

    RenderConn::Pkt_Smackable_Open *oc = Sim::Packet::Cast<RenderConn::Pkt_Smackable_Open>(data.pkt);
    this->mTarget.Set(oc->mObjectWUID);

    this->mHeirarchy = oc->mHeirarchy;
    this->mModelHash = oc->mModelHash;
    const CollisionGeometry::Bounds *bounds = oc->mCollisionNode;
    this->mRenderNode = oc->mRenderNode;
    UMath::Vector3 pivot;
    bounds->GetPivot(pivot);
    this->mModelOffset.x = -pivot.z;
    this->mModelOffset.y = pivot.x;
    this->mModelOffset.z = -pivot.y;
}

SmackableRenderConn::~SmackableRenderConn() {
    this->mList.Remove(this);
    if (this->mModel != nullptr) {
        delete this->mModel;
        this->mModel = nullptr;
    }
    this->mTarget.Set(0);
}

void SmackableRenderConn::Update(float dT) {
    if (this->mTarget.IsValid()) {
        this->mRenderMatrix = *this->mTarget.GetMatrix();
        bVector4 tmp;
        eMulVector(&tmp, this->mTarget.GetMatrix(), &this->mModelOffset);

        this->mRenderMatrix.v3 += tmp;
        this->mRenderMatrix.v3.w = 1.0f;

        float disttoview = 0.0f;
        bool inview = false;
        if ((this->mModel != nullptr) && this->mModel->IsEnabled()) {
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
            if (this->mModel == nullptr) {
                if ((this->mHeirarchy != nullptr) && this->mRenderNode != 0) {
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
    for (SmackableRenderConn *w = mList.GetHead(); w != mList.EndOfList(); w = w->GetNext()) {
        w->Update(dT);
    }
}

void SmackableRender_Init() {}

void SmackableRender_Shutdown() {}

void SmackableRender_Service(float dT) {
    SmackableRenderConn::UpdateAll(dT);
}

bTList<SmackableRenderConn> SmackableRenderConn::mList;
BIND_SIM_CONN(SmackableRenderConn);
