#include "Speed/Indep/Src/World/WCollider.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/Src/World/WWorldMath.h"

void VU0_v4crossprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &r);

UTL::Std::map<unsigned int, WCollider *, _type_map> WCollider::fWuidMap;
template <> UTL::Collections::Listable<WCollider, 100>::List UTL::Collections::Listable<WCollider, 100>::_mTable;

WCollider::WCollider(eColliderShape colliderShape, unsigned int typeMask, unsigned int exclusionMask)
    : fRequestedPosition(UMath::Vector3::kZero),     //
      fRequestedRadius(0.0f),                        //
      fLastRequestedPosition(UMath::Vector3::kZero), //
      fLastRequestedRadius(0.0f),                    //
      fPosition(UMath::Vector3::kZero),              //
      fRadius(0.0f),                                 //
      fLastRefreshedPosition(UMath::Vector3::kZero), //
      fRegionInitialized(false),                     //
      fColliderShape(colliderShape),                 //
      fTypeMask(typeMask),                           //
      fRefCount(0),                                  //
      fWorldID(0),                                   //
      fExclusionFlags(exclusionMask) {
    ReserveLists(typeMask);
}

WCollider::~WCollider() {
    Clear();
}

WCollider *WCollider::Get(unsigned int wuid) {
    if (wuid == 0) {
        return nullptr;
    }

    UTL::Std::map<unsigned int, WCollider *, _type_map>::iterator iter = fWuidMap.find(wuid);
    if (iter != fWuidMap.end()) {
        return iter->second;
    }

    return nullptr;
}

WCollider *WCollider::Create(unsigned int wuid, eColliderShape shape, unsigned int typeCheckMask, unsigned int exclusionMask) {
    WCollider *col = Get(wuid);
    if (col == nullptr) {
        col = new WCollider(shape, typeCheckMask, exclusionMask);
        col->fWorldID = wuid;
        col->AddRef();
        if (wuid != 0) {
            fWuidMap[wuid] = col;
        }
        return col;
    } else {
        col->AddRef();
        return col;
    }
}

void WCollider::Destroy(WCollider *col) {
    col->RemoveRef();
    if (col->fRefCount == 0) {
        UTL::Std::map<unsigned int, WCollider *, _type_map>::iterator iter = fWuidMap.find(col->fWorldID);
        if (iter != fWuidMap.end()) {
            fWuidMap.erase(iter);
        }
        delete col;
    }
}

static void CalcNewRegionSizeFromRequested(bool useLastData, const UMath::Vector3 &reqPos, float reqRad, const UMath::Vector3 &oldPos,
                                           float oldRad, const UMath::Vector3 &lastPos, UMath::Vector3 &pos, float &rad) {
    if (!useLastData) {
        pos = reqPos;
        rad = reqRad * 1.1f;
        return;
    }

    float vel = UMath::Distance(reqPos, lastPos);
    if (vel > 5.0f) {
        pos = reqPos;
        rad = reqRad * 1.1f;
        return;
    }

    UMath::Vector3 moveVec;
    UMath::Sub(reqPos, oldPos, moveVec);
    float lifeFactor = 2.5f;
    UMath::Unit(moveVec, moveVec);
    UMath::Scale(moveVec, vel * lifeFactor, moveVec);
    UMath::Add(reqPos, moveVec, pos);

    float moveDist = UMath::Length(moveVec);
    rad = reqRad + moveDist + 0.1f;
    if (rad > 25.0f) {
        rad = 25.0f;
    }
}

void WCollider::InvalidateIntersectingColliders(const UMath::Vector4 &posRad) {
    for (List::const_iterator iter = GetList().begin(); iter != GetList().end(); ++iter) {
        WCollider &collider = **iter;

        if (UMath::Distance(collider.fPosition, UMath::Vector4To3(posRad)) < posRad.w + collider.fRadius) {
            collider.Clear();
        }
    }
}

void WCollider::Refresh(const UMath::Vector3 &pt, float radius, bool predictiveSizing) {
    if (!WWorld::Get().IsValid()) {
        EmptyLists(0x1C);
        return;
    }

    unsigned int updateMask = GetUpdateMask(pt, radius);
    if (updateMask != 0) {
        EmptyLists(updateMask);
        ReserveLists(updateMask);

        if (updateMask == fTypeMask) {
            fRequestedPosition = pt;
            fRequestedRadius = radius;

            if (predictiveSizing) {
                CalcNewRegionSizeFromRequested(fRegionInitialized, fRequestedPosition, radius, fLastRequestedPosition, fLastRequestedRadius, fLastRefreshedPosition,
                                               fPosition, fRadius);
            } else {
                fPosition = fRequestedPosition;
                fRadius = fRequestedRadius * 1.1f;
            }
        }

        PrepareRegion(updateMask);
    }

    if (predictiveSizing) {
        fLastRefreshedPosition = pt;
        fLastRequestedPosition = pt;
        fLastRequestedRadius = radius;
    }
}

void WCollider::PrepareRegion(unsigned int updateMask) {
    if (updateMask & 0xC) {
        WCollisionMgr(fExclusionFlags, 3).GetInstanceList(fInstanceCacheList, fPosition, fRadius, fColliderShape == kColliderShape_Cylinder);
        if (updateMask & 0x8) {
            WCollisionMgr(fExclusionFlags, 3).GetTriList(fInstanceCacheList, fPosition, fRadius, fTriList);
        }
    }

    if (updateMask & 0x4) {
        fBarrierList.reserve(0x15);
        WCollisionMgr(fExclusionFlags, 3).GetBarrierList(fBarrierList, fInstanceCacheList, fPosition, fRadius);
    }

    if (updateMask & 0x10) {
        WCollisionMgr(fExclusionFlags, 3).GetObjectList(fObbList, fPosition, fRadius);
    }

    fRegionInitialized = true;
}

bool WCollider::IsEmpty() const {
    return fInstanceCacheList.empty() && fBarrierList.empty();
}

void WCollider::Clear() {
    if (fRegionInitialized) {
        ClearLists(0x1C);
        fRegionInitialized = false;
    }
}

void WCollider::ClearLists(unsigned int typeMask) {
    if (typeMask & 0x8) {
        fInstanceCacheList.clear();
        fTriList.clear_all();
    }

    if (typeMask & 0x4) {
        fBarrierList.clear();
    }

    if (typeMask & 0x10) {
        fObbList.clear();
    }
}

void WCollider::EmptyLists(unsigned int typeMask) {
    if (typeMask & 0x8) {
        fInstanceCacheList.resize(0);
        fTriList.clear_all();
    }

    if (typeMask & 0x4) {
        fBarrierList.resize(0);
    }

    if (typeMask & 0x10) {
        fObbList.resize(0);
    }
}

void WCollider::ReserveLists(unsigned int typeMask) {
    if (typeMask & 0x8) {
        fInstanceCacheList.reserve(0x64);
        fTriList.reserve(0x8);
    }

    if (typeMask & 0x4) {
        fBarrierList.reserve(0x19);
    }

    if (typeMask & 0x10) {
        fObbList.reserve(0x19);
    }
}

unsigned int WCollider::Validate() const {
    if (!*reinterpret_cast<const unsigned int *>(&fRegionInitialized)) return 0;
    return 1;
}

unsigned int WCollider::GetUpdateMask(const UMath::Vector3 &pt, float radius) {
    unsigned int updateMask = fTypeMask & 0x10;
    if (!Validate() || !InRegion(pt, radius)) {
        updateMask |= fTypeMask & 0xC;
    }
    return updateMask;
}

bool WCollider::InRegion(const UMath::Vector3 &pt, float radius) const {
    float radDiff = fRadius - radius;
    if (radDiff < 0.0f) {
        return false;
    }
    return UMath::DistanceSquare(pt, fPosition) < radDiff * radDiff;
}

void WCollider::InvalidateAllCachedData() {
    const List &list = GetList();
    List::const_iterator iter = list.begin();
    List::const_iterator end = list.end();
    const unsigned int regionInitialized = false;
    while (iter != end) {
        (*iter)->Clear();
        (*iter)->fRegionInitialized = regionInitialized;
        ++iter;
    }
}

void WCollisionObject::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    m = fMat;

    if (addXLate) {
        m.v3.x = fPosRadius.x;
        m.v3.y = fPosRadius.y;
        m.v3.z = fPosRadius.z;
        m.v3.w = 1.0f;
        return;
    }

    m.v3.x = 0.0f;
    m.v3.y = 0.0f;
    m.v3.z = 0.0f;
    m.v3.w = 1.0f;
}

float WCollisionInstance::CalcSphericalRadius() const {
    float maxExtent = fInvMatRow2Length.w;
    if (maxExtent < fInvPosRadius.w) maxExtent = fInvPosRadius.w;
    if (fHeight > maxExtent) maxExtent = fHeight;
    if (maxExtent >= fInvMatRow0Width.w) return maxExtent;
    return fInvMatRow0Width.w;
}

void WCollisionInstance::CalcPosition(UMath::Vector3 &pos) const {
    pos.x = (-fInvPosRadius.x * fInvMatRow0Width.x - fInvPosRadius.y * fInvMatRow0Width.y) - fInvPosRadius.z * fInvMatRow0Width.z;
    pos.z = (-fInvPosRadius.x * fInvMatRow2Length.x - fInvPosRadius.y * fInvMatRow2Length.y) - fInvPosRadius.z * fInvMatRow2Length.z;
    bool needsCross = NeedsCrossProduct();

    if (needsCross) {
        UMath::Vector4 upVec;
        VU0_v4crossprodxyz(reinterpret_cast<const UMath::Vector4 &>(fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(fInvMatRow0Width),
                           upVec);
        pos.y = (-fInvPosRadius.x * upVec.x - fInvPosRadius.y * upVec.y) - fInvPosRadius.z * upVec.z;
    } else {
        pos.y = -fInvPosRadius.y;
    }
}

void WCollisionInstance::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    m.v0.x = fInvMatRow0Width.x;
    m.v0.y = fInvMatRow0Width.y;
    m.v0.z = fInvMatRow0Width.z;
    m.v0.w = 0.0f;
    bool needsCross = NeedsCrossProduct();

    if (needsCross) {
        VU0_v4crossprodxyz(reinterpret_cast<const UMath::Vector4 &>(fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(fInvMatRow0Width),
                           m.v1);
        m.v1.w = 0.0f;
    } else {
        m.v1.x = 0.0f;
        m.v1.y = 1.0f;
        m.v1.z = 0.0f;
        m.v1.w = 0.0f;
    }

    m.v2.x = fInvMatRow2Length.x;
    m.v2.y = fInvMatRow2Length.y;
    m.v2.z = fInvMatRow2Length.z;
    m.v2.w = 0.0f;

    if (addXLate) {
        m.v3.x = fInvPosRadius.x;
        m.v3.y = fInvPosRadius.y;
        m.v3.z = fInvPosRadius.z;
        m.v3.w = 1.0f;
    } else {
        m.v3.x = 0.0f;
        m.v3.y = 0.0f;
        m.v3.z = 0.0f;
        m.v3.w = 1.0f;
    }
}
