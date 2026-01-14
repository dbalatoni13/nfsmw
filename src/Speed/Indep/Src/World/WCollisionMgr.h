#ifndef WORLD_WCOLLISION_MGR_H
#define WORLD_WCOLLISION_MGR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WCollision.h"
#include "WCollisionTri.h"

class WCollisionMgr {
  public:
    // total size: 0x58
    struct WorldCollisionInfo {
        UMath::Vector4 fCollidePt;        // offset 0x0, size 0x10
        UMath::Vector4 fNormal;           // offset 0x10, size 0x10
        WCollisionBarrierListEntry fBle;  // offset 0x20, size 0x28
        const WCollisionObject *fObj;     // offset 0x48, size 0x4
        float fDist;                      // offset 0x4C, size 0x4
        unsigned char fAnimated;          // offset 0x50, size 0x1
        unsigned char fType;              // offset 0x51, size 0x1
        unsigned short fPad;              // offset 0x52, size 0x2
        const WCollisionInstance *fCInst; // offset 0x54, size 0x4

        // UNSOLVED
        WorldCollisionInfo() {
            fCollidePt = UMath::Vector4::kIdentity;
            fNormal = UMath::Vector4::kIdentity;
            // fBle.fB = WCollisionBarrier();
            // TODO maybe this is in WCollisionBarrierListEntry's constructor?
            memset(&fBle.fB, 0, sizeof(fBle.fB));
            fBle.fSurfaceRef = nullptr;
            fBle.fDistanceToSq = 0.0f;
            fDist = 0.0f;
            fObj = nullptr;
            fAnimated = false;
            fType = 0;
            fPad = 0;
            fCInst = nullptr;
        }

        bool HitSomething() const {}
    };

    class ICollisionHandler {
      public:
        ICollisionHandler() {}

        virtual bool OnWCollide(const WorldCollisionInfo &cInfo, const bVector3 &cPoint, void *userdata);
    };

    typedef UTL::Vector<unsigned int, 16> NodeIndexList;
    // typedef FastVector<unsigned int, 16> FastNodeIndexList;
    // typedef FastVector<unsigned int, 16> LocalNodeIndexList;

    bool FindFaceInTriStrip(const UMath::Vector3 &pt, const struct WCollisionStripSphere *sp, const struct WCollisionStrip *strip,
                            WCollisionTri &retFace);
    bool FindFaceInTriStrip(const UMath::Matrix4 &vectorMat, const UMath::Vector3 &pt, const WCollisionStripSphere *sp, const WCollisionStrip *strip,
                            float &faceY, WCollisionTri &retFace);
    bool FindFaceInCInst(const UMath::Vector3 &pt, const WCollisionInstance &cInst, WCollisionTri &retFace, float &retDist);
    bool FindFaceInCInst(const UMath::Matrix4 &vectorMat, const UMath::Vector3 &endPt, const WCollisionInstance &cInst, WCollisionTri &retFace,
                         float &retDist);
    bool GetWorldHeightAtPoint(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal);
    bool GetWorldHeightAtPointRigorous(const UMath::Vector3 &pt, float &height, UMath::Vector3 *normal);
    int CheckHitWorld(const UMath::Vector4 *inputSeg, WorldCollisionInfo &cInfo, unsigned int primMask);
    bool GetWorldNormal(const WCollisionInstanceCacheList *instList, const WCollisionBarrierList *barrierList, const UMath::Vector4 *seg,
                        WorldCollisionInfo &cInfo);
    void ClosestCollisionInfo(const UMath::Vector4 *seg, const WorldCollisionInfo &c1, const WorldCollisionInfo &c2, WorldCollisionInfo &result);
    void GetInstanceListGuts(const NodeIndexList &nodeInds, WCollisionInstanceCacheList &instList, const UMath::Vector3 &inPt, float radius,
                             bool cylinderTest);
    void GetInstanceList(WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, float radius, bool cylinderTest);
    void GetInstanceListGuts(const NodeIndexList &nodeInds, WCollisionInstanceCacheList &instList, const UMath::Vector4 *seg);
    void GetInstanceList(WCollisionInstanceCacheList &instList, const UMath::Vector4 *seg);
    void GetObjectListGuts(const NodeIndexList &nodeInds, WCollisionObjectList &obbObjectList, const UMath::Vector3 &pt, float radius);
    void GetObjectList(WCollisionObjectList &obbObjectList, const UMath::Vector4 *seg);
    void GetObjectList(WCollisionObjectList &obbObjectList, const UMath::Vector3 &pt, float radius);
    void BuildGeomFromWorldObb(const WCollisionObject &object, float dt, Dynamics::Collision::Geometry &geom, UMath::Vector3 &vel, WSurface &surface);
    bool Collide(Dynamics::Collision::Geometry *geom, const WCollisionBarrierList *barrierList, ICollisionHandler *results, void *userdata,
                 bool force_single_sided);
    bool Collide(Dynamics::Collision::Geometry *geom, const WCollisionInstanceCacheList *instanceList, ICollisionHandler *results, void *userdata);
    bool GetClosestIntersectingBarrier(const WCollisionBarrierList &barrierList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo);
    bool GetBarrierNormal(const WCollisionInstanceCacheList &instList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo);
    void GetBarrierList(WCollisionBarrierList &barrierList, const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pos, float radius);
    void GetBarrierList(WCollisionBarrierList &barrierList, const UMath::Vector3 &pos, float radius);
    bool GetBarrierNormal(const WCollisionBarrierList &barrierList, const UMath::Vector4 *testSegment, WorldCollisionInfo &cInfo);
    void GetTriList(const WCollisionInstanceCacheList &instList, const UMath::Vector3 &pt, float radius, WCollisionTriList &triList);

    WCollisionMgr(unsigned int surfaceExclMask, unsigned int primitiveExclMask) {
        this->fSurfaceExclusionMask = surfaceExclMask;
        this->fPrimitiveMask = primitiveExclMask;
    }

    ~WCollisionMgr() {}

  private:
    static unsigned short fIterCount;

    unsigned int fSurfaceExclusionMask; // offset 0x0, size 0x4
    unsigned int fPrimitiveMask;        // offset 0x4, size 0x4
};

#endif
