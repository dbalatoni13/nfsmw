#ifndef PHYSICS_PLACEABLESCENERY_H
#define PHYSICS_PLACEABLESCENERY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Physics/HeirarchyModel.h"

struct PlaceableScenery : public HeirarchyModel, public IPlaceableScenery {
    PlaceableScenery(bHash32 rendermesh, const CollisionGeometry::Bounds *geometry,
                     const Attrib::Collection *attribs, const ModelHeirarchy *heirarchy);
    ~PlaceableScenery() override;

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    static PlaceableScenery *Construct(const char *name, unsigned int node);

    // IPlaceableScenery
    void PickUp() override;
    bool Place(const UMath::Matrix4 &transform, bool snap_to_ground) override;
    void Destroy() override {
        Sim::Model::ReleaseModel();
    }

    // IModel
    void ReleaseModel() override;
    bool OnRemoveOffScreen(float dT) override {
        return false;
    }
};

#endif
