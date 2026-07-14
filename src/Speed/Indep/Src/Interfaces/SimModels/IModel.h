#ifndef INTERFACES_SIMMODELS_IMODEL_H
#define INTERFACES_SIMMODELS_IMODEL_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/World/WorldTypes.h"

// total size: 0x4
class IModel : public UTL::COM::IUnknown, public UTL::Collections::Instanceable<HMODEL, IModel, 434>, public UTL::Collections::Listable<IModel, 434> {
  public:
    enum { InstanceLimit = 434 };

    DECL_INTERFACE(IModel);
    struct Enumerator {
        virtual bool OnModel(IModel *model);
    };

    virtual void OnProcessFrame(float dT) = 0;
    virtual UCrc32 GetPartName() const = 0;
    virtual WUID GetWorldID() const = 0;
    virtual void SetCausality(HCAUSE from, float time) = 0;
    virtual HCAUSE GetCausality() const = 0;
    virtual float GetCausalityTime() const = 0;
    virtual EventSequencer::IEngine *GetEventSequencer() = 0;
    virtual ISimable *GetSimable() const = 0;
    virtual bool IsHidden() const = 0;
    virtual void HideModel() = 0;
    virtual bool InView() const = 0;
    virtual float DistanceToView() const = 0;
    virtual void GetLinearVelocity(UMath::Vector3 &velocity) const = 0;
    virtual void GetAngularVelocity(UMath::Vector3 &velocity) const = 0;
    virtual void GetTransform(UMath::Matrix4 &matrix) const = 0;
    virtual const CollisionGeometry::Bounds *GetCollisionGeometry() const = 0;
    virtual const Attrib::Instance &GetAttributes() const = 0;
    virtual Enumerator *EnumerateChildren(Enumerator *enumerator) const = 0;
    virtual IModel *GetChildModel(UCrc32 name) const = 0;
    virtual IModel *GetRootModel() const = 0;
    virtual bool IsRootModel() const = 0;
    virtual IModel *GetParentModel() const = 0;
    virtual IModel *SpawnModel(UCrc32 rendernode, UCrc32 collisionnode, UCrc32 attributes) = 0;
    virtual void ReleaseModel() = 0;
    virtual void ReleaseChildModels() = 0;
    virtual void HidePart(const UCrc32 &nodename) = 0;
    virtual void ShowPart(const UCrc32 &nodename) = 0;
    virtual bool IsPartVisible(const UCrc32 &nodename) const = 0;
    virtual void PlayEffect(UCrc32 identifire, const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude,
                            bool tracking) = 0;
    virtual void StopEffect(UCrc32 identifire) = 0;
    virtual void StopEffects() = 0;
};

#endif
