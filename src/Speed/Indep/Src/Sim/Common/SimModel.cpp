#include "../SimModel.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

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
        Detach(mSimable);
        mSimable = nullptr;
    }
}

void Model::EndDraw() {
    if (mService) {
        CloseService(mService);
        mService = nullptr;
        OnEndDraw();
        mInView = false;
        mDistanceToView = 0.0f;
    }
}

// UNSOLVED stack size
bool Model::Attach(IUnknown *pOther) {
    if (!mAttachments) {
        mAttachments = new Attachments(this);
    }
    return mAttachments->Attach(pOther);
}

bool Model::Detach(IUnknown *pOther) {
    if (mAttachments) {
        return mAttachments->Detach(pOther);
    } else {
        return false;
    }
}

bool Model::IsAttached(const IUnknown *pOther) const {
    if (mAttachments) {
        return mAttachments->IsAttached(pOther);
    } else {
        return false;
    }
}

const UTL::Std::list<IAttachable *, _type_IAttachableList> *Model::GetAttachments() const {
    if (mAttachments) {
        return &mAttachments->GetList();
    } else {
        return nullptr;
    }
}

void Model::OnAttached(IAttachable *pOther) {
    ISimable *isimable;
    if (!mSimable && pOther->QueryInterface(&isimable)) {
        mSimable = isimable;
        OnBeginSimulation();
    }
    IModel *imodel;
    if (!pOther->QueryInterface(&imodel) || imodel->GetParentModel() != this) {
        return;
    }
    if (mGeometry && mChildren.capacity() < mGeometry->fNumChildren) {
        mChildren.reserve(mGeometry->fNumChildren);
    }

    mChildren.push_back(imodel);
}

void Model::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mSimable)) {
        OnEndSimulation();
        mSimable = nullptr;
    }

    if (UTL::COM::ComparePtr(pOther, mParent)) {
        ReleaseModel();
        mParent = nullptr;
    }

    for (Children::iterator iter = mChildren.begin(); iter != mChildren.end(); iter++) {
        IModel *imodel = *iter;
        if (UTL::COM::ComparePtr(imodel, pOther)) {
            mChildren.erase(iter);
            break;
        }
    }
}

void Model::ReleaseModel() {
    ReleaseChildModels();
    EndDraw();
    EndSimulation();
    ReleaseGC();
}

bool Model::OnService(HSIMSERVICE hCon, Packet *pkt) {
    if (hCon == mService && !IsDirty()) {
        return OnDraw(pkt);
    } else {
        return false;
    }
}

void Model::ReleaseChildModels() {
    UTL::Std::vector<IModel *, _type_SimModelChildren> children = mChildren;
    for (Children::iterator iter = children.begin(); iter != children.end(); iter++) {
        IModel *imodel = *iter;
        imodel->ReleaseModel();
    }

    mChildren.clear();
}

IModel *Model::GetChildModel(UCrc32 name) const {
    if (name == UCrc32::kNull) {
        return nullptr;
    }

    for (Children::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++) {
        IModel *imodel = *iter;
        if (imodel->GetPartName() == name) {
            return imodel;
        }
    }
    return nullptr;
}

IModel::Enumerator *Model::EnumerateChildren(IModel::Enumerator *enumerator) const {
    for (Children::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++) {
        IModel *imodel = *iter;
        if (!enumerator->OnModel(imodel)) {
            break;
        }
    }
    return enumerator;
}

bool Model::IsHidden() const {
    return mService == nullptr;
}

void Model::HideModel() {
    EndDraw();
    EndSimulation();
}

void Model::PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                       bool tracking) {
    if (identifire == UCrc32::kNull) {
        return;
    }
    Model::Effect *modeleffect = nullptr;
    for (Model::Effect *e = mEffects.GetHead(); e != mEffects.EndOfList(); e = static_cast<Model::Effect *>(e->GetNext())) {
        Model::Effect *me = e;
        if (me->Identifire == identifire) {
            modeleffect = me;
            break;
        }
    }
    if (!modeleffect) {
        modeleffect = ::new ("SimModel", 0) Model::Effect(UCrc32(identifire), GetWorldID(), GetAttributes().GetConstCollection());

        mEffects.AddTail(modeleffect);
    }
    modeleffect->Set(effect, position, magnitude, nullptr, tracking, 0);
}

void Model::StopEffects() {
    mEffects.DeleteAllElements();
}

void Model::StopEffect(UCrc32 identifire) {
    for (Model::Effect *e = mEffects.GetHead(); e != mEffects.EndOfList(); e = static_cast<Model::Effect *>(e->GetNext())) {
        Model::Effect *me = e;
        if (me->Identifire == identifire) {
            mEffects.Remove(me);
            delete me;
            break;
        }
    }
}

}; // namespace Sim
