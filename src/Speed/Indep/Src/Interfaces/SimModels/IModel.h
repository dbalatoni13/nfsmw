#ifndef INTERFACES_SIMMODELS_IMODEL_H
#define INTERFACES_SIMMODELS_IMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Bounds.h"

struct HMODEL__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HMODEL__ *HMODEL;

class IModel : public UTL::COM::IUnknown, public UTL::Collections::Instanceable<HMODEL, IModel, 434>, public UTL::Collections::Listable<IModel, 434> {
  public:
    // total size: 0x4
    struct Enumerator {};

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IModel(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IModel() {}

    virtual void OnProcessFrame(float dT);
    virtual UCrc32 GetPartName() const;
    virtual unsigned int GetWorldID() const;
    virtual void SetCausality(HCAUSE from, float time);
    virtual HCAUSE GetCausality() const;
    virtual float GetCausalityTime() const;
    virtual EventSequencer::IEngine *GetEventSequencer();
    virtual ISimable *GetSimable() const;
    virtual bool IsHidden() const;
    virtual void HideModel();
    virtual bool InView() const;
    virtual float DistanceToView() const;
    virtual void GetLinearVelocity(UMath::Vector3 &velocity) const;
    virtual void GetAngularVelocity(UMath::Vector3 &velocity) const;
    virtual void GetTransform(UMath::Matrix4 &matrix) const;
    virtual const CollisionGeometry::Bounds *GetCollisionGeometry() const;
    virtual const Attrib::Instance &GetAttributes() const;
    virtual IModel::Enumerator *EnumerateChildren(IModel::Enumerator *enumerator) const;
    virtual IModel *GetChildModel(UCrc32 name) const;
    virtual IModel *GetRootModel() const;
    virtual bool IsRootModel() const;
    virtual IModel *GetParentModel() const;
    virtual IModel *SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes);
    virtual void ReleaseModel();
    virtual void ReleaseChildModels();
    virtual void HidePart(const UCrc32 &nodename);
    virtual void ShowPart(const UCrc32 &nodename);
    virtual bool IsPartVisible(const UCrc32 &nodename) const;
    virtual void PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                            bool tracking);
    virtual void StopEffect(UCrc32 identifire);
    virtual void StopEffects();
};

#endif
