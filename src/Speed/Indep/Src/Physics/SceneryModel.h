#ifndef PHYSICS_SCENERYMODEL_H
#define PHYSICS_SCENERYMODEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Physics/HeirarchyModel.h"
#include "Speed/Indep/Src/Physics/SmackableTrigger.h"

class SmokeableSpawner;

class SceneryModel : public HeirarchyModel, public ISceneryModel {
  public:
    SceneryModel(SmokeableSpawner *spawner, const CollisionGeometry::Bounds *geometry,
                 const Attrib::Collection *attribs, bool hidden);
    ~SceneryModel() override;

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) { gFastMem.Free(mem, size, nullptr); }
    }

    static SceneryModel *Construct(SmokeableSpawner *data, const Attrib::Collection *attributes, bool hidden);

    static int SceneryCount() {
        return mSceneryCount;
    }

    unsigned int GetSpawnerID() const;
    void RestoreScene();
    bool GetSceneryTransform(UMath::Matrix4 &matrix) const;
    void WakeUp();
    bool IsExcluded(unsigned int scenery_exclusion_flag) const;

    void GetTransform(UMath::Matrix4 &matrix) const override;
    void ReleaseModel() override;
    bool IsHidden() const override;
    void HideModel() override;
    void OnBeginDraw() override;
    void OnEndSimulation() override;

    static void InitSystem();
    static void RestoreSystem();

  private:
    void InitScene();
    void StartOverride();
    void EndOverride();
    void ShowInstance(bool show);

    bool mInstanceVisible;
    SmokeableSpawner *mSpawner;

    static int mSceneryCount;

    friend class SmokeableSpawner;
};

#endif
