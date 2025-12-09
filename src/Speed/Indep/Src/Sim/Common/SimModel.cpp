#include "../SimModel.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

namespace Sim {

// UNSOLVED usual Sim stack problem
Model::Model(IModel *parent, const CollisionGeometry::Bounds *geometry, UCrc32 nodename, std::size_t numinterfaces)
    : Sim::Object(numinterfaces + 4), //
      IAttachable(this),              //
      IModel(this),                   //
      mParent(parent),                //
      mRoot(nullptr),                 //
      mGeometry(geometry),            //
      mNodeName(nodename),            //
      mIsRoot(parent == nullptr),     //
      mCausality(nullptr),            //
      mCauseTime(0.0f),               //
      mSimable(nullptr),              //
      mAttachments(nullptr),          //
      mDistanceToView(0.0f),          //
      mInView(false),                 //
      mSequencer(nullptr),            //
      mService(nullptr),              //
      EventSequencer::IContext(this) {
    if (parent) {
        mCausality = parent->GetCausality();
        mCauseTime = parent->GetCausalityTime();
        Attach(parent);

        for (IModel *p = parent; p != nullptr; p = p->GetParentModel()) {
            mRoot = p;
        }
    }
}

Model::~Model() {
    if (mAttachments) {
        delete mAttachments;
        mAttachments = nullptr;
    }
    if (mService) {
        CloseService(mService);
        mService = nullptr;
    }
    if (mSequencer) {
        mSequencer->Release();
        mSequencer = nullptr;
    }
    mEffects.DeleteAllElements();
}

void Model::GetLinearVelocity(UMath::Vector3 &velocity) const {
    if (mSimable) {
        mSimable->GetLinearVelocity(velocity);
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

void Model::GetAngularVelocity(UMath::Vector3 &velocity) const {
    if (mSimable) {
        mSimable->GetAngularVelocity(velocity);
    } else {
        velocity = UMath::Vector3::kZero;
    }
}

bool Model::SetDynamicData(const EventSequencer::System *system, EventDynamicData *data) {
    data->fWorldID = GetWorldID();
    data->fhModel = reinterpret_cast<uintptr_t>(GetInstanceHandle());

    UMath::Matrix4 m;
    GetTransform(m);

    data->fPosition = m.v3;
    data->fVector = m.v2;

    GetLinearVelocity(Vector4To3(data->fVelocity));
    GetAngularVelocity(Vector4To3(data->fAngularVelocity));

    if (mSimable) {
        data->fhSimable = reinterpret_cast<uintptr_t>(mSimable->GetInstanceHandle());
    }
    return true;
}

void Model::StartSequencer(UCrc32 name) {
    if (!mSequencer && name.GetValue() != 0) {
        mSequencer = Create(this, this, UCrc32(name), GetTime(), 1.0f);
    }
}

void Model::ReleaseSequencer() {
    if (mSequencer) {
        mSequencer->Release();
        mSequencer = nullptr;
    }
}

void Model::BeginDraw(UCrc32 service, Packet *what) {
    if (!mService) {
        mService = OpenService(UCrc32(service), what);
        OnBeginDraw();
    }
}

WUID Model::GetWorldID() const {
    return reinterpret_cast<WUID>(GetInstanceHandle()) | 0x4000000;
}

void Model::EndSimulation() {
    if (mSimable) {
        // OnEndSimulation();
        mSimable = nullptr;
    }
}

}; // namespace Sim
