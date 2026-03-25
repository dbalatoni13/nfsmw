#ifndef PHYSICS_HEIRARCHYMODEL_H
#define PHYSICS_HEIRARCHYMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Physics/Smackable.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"
#include "Speed/Indep/Src/Sim/SimModel.h"
#include "Speed/Indep/Src/World/ModelHeirarchy.hpp"

class HeirarchyModel : public Sim::Model, public IBody, public ITriggerableModel, public Attrib::Gen::smackable {
  public:
    void *operator new(std::size_t size) { return gFastMem.Alloc(size, nullptr); }
    void operator delete(void *mem, std::size_t size) {
        if (mem) { gFastMem.Free(mem, size, nullptr); }
    }

    HeirarchyModel(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry, UCrc32 nodename,
                   HeirarchyModel *parent, const Attrib::Collection *attribs, const ModelHeirarchy *heirarchy,
                   unsigned int child_index, bool visible);

    ~HeirarchyModel() override;

    void OnBeginDraw() override;
    void OnEndDraw() override;
    bool OnDraw(Sim::Packet *service) override;
    void OnBeginSimulation() override;
    void OnEndSimulation() override;

    void GetTransform(UMath::Matrix4 &matrix) const override;
    void GetLinearVelocity(UMath::Vector3 &velocity) const override {
        Sim::Model::GetLinearVelocity(velocity);
    }
    void GetAngularVelocity(UMath::Vector3 &velocity) const override;
    void GetDimension(UMath::Vector3 &dim) const override {
        GetCollisionGeometry()->GetHalfDimensions(dim);
    }
    const Attrib::Instance &GetAttributes() const override {
        return static_cast<const Attrib::Gen::smackable *>(this)->GetBase();
    }
    unsigned int GetWorldID() const override {
        return Sim::Model::GetWorldID();
    }

    IModel *SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) override;
    void HidePart(const UCrc32 &nodename) override;
    void ShowPart(const UCrc32 &nodename) override;
    bool IsPartVisible(const UCrc32 &nodename) const override;
    void ReleaseModel() override;

    void PlaceTrigger(const UMath::Matrix4 &mat, bool retrigger) override;

    void OnProcessFrame(float dT) override;
    virtual bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep);
    virtual bool OnRemoveOffScreen(float time);

    int FindHeirarchyChild(const UCrc32 &nodename) const;
    void DisableTrigger();
    void SetTrigger(const UMath::Matrix4 &matrix, bool enable);
    void RemoveTrigger();
    void SetCameraAvoidable(bool enable);

    SmackableTrigger *GetTrigger() {
        return mTrigger;
    }

  protected:
    UMath::Vector4 mTriggerAvoid;
    ModelHeirarchy *mHeirarchy;
    bHash32 mRenderMesh;
    SmackableTrigger *mTrigger;
    float mOffScreenTimer;
    unsigned short mHeirarchyNode;
    unsigned short mFlags;
    unsigned int mChildVisibility;
    SmackableAvoidable *mAvoidable;
};

#endif
