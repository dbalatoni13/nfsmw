#include "Speed/Indep/Src/World/WCollider.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// TODO move
extern bool Tweak_colliderDraws;

UTL::Std::map<unsigned int, WCollider *, _type_map> WCollider::fWuidMap;
UTL::Collections::Listable<WCollider, 100>::List UTL::Collections::Listable<WCollider, 100>::_mTable;

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
    this->ReserveLists(typeMask);
}

WCollider::~WCollider() {
    this->Clear();
}

WCollider *WCollider::Get(unsigned int wuid) {
    if (wuid == 0) {
        return nullptr;
    }

    UTL::Std::map<unsigned int, WCollider *, _type_map>::iterator iter = fWuidMap.find(wuid);
    if (iter != fWuidMap.end()) {
        return (*iter).second;
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

void WCollider::InvalidateIntersectingColliders(const UMath::Vector4 &posRad) {
    for (List::const_iterator iter = GetList().begin(); iter != GetList().end(); ++iter) {
        WCollider &collider = **iter;

        if (UMath::Distance(collider.fPosition, UMath::Vector4To3(posRad)) < posRad.w + collider.fRadius) {
            collider.Clear();
        }
    }
}

// UNSOLVED
static void CalcNewRegionSizeFromRequested(bool useLastData, const UMath::Vector3 &reqPos, float reqRad, const UMath::Vector3 &oldPos, float oldRad,
                                           const UMath::Vector3 &lastPos, UMath::Vector3 &pos, float &rad) {
    if (!useLastData) {
        pos = reqPos;
        rad = reqRad * 1.1f;
    } else {
        float vel = UMath::Distance(reqPos, lastPos);
        if (vel > 5.0f) {
            pos = reqPos;
            rad = reqRad * 1.1f;
        } else {
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
    }
}

#ifdef EA_BUILD_A124
bool gDisableWCollider;
#endif

void WCollider::Refresh(const UMath::Vector3 &pt, float radius, bool predictiveSizing) {
    if (!WWorld::Get().IsValid()) {
        this->EmptyLists(28);
        return;
    }

#ifdef EA_BUILD_A124
    if (gDisableWCollider) {
        this->EmptyLists(28);
        return;
    }
#endif

    unsigned int updateMask = this->GetUpdateMask(pt, radius);
    if (updateMask != 0) {
        this->EmptyLists(updateMask);
        this->ReserveLists(updateMask);

        if (updateMask == this->fTypeMask) {
            this->fRequestedPosition = pt;
            this->fRequestedRadius = radius;

            if (predictiveSizing) {
                CalcNewRegionSizeFromRequested(this->fRegionInitialized, this->fRequestedPosition, radius, this->fLastRequestedPosition,
                                               this->fLastRequestedRadius, this->fLastRefreshedPosition, this->fPosition, this->fRadius);
            } else {
                this->fPosition = this->fRequestedPosition;
                this->fRadius = this->fRequestedRadius * 1.1f;
            }
        }

        this->PrepareRegion(updateMask);
    }

    if (predictiveSizing) {
        this->fLastRefreshedPosition = pt;
        this->fLastRequestedPosition = pt;
        this->fLastRequestedRadius = radius;
    }
}

void WCollider::PrepareRegion(unsigned int updateMask) {
    if (updateMask & 0xC) {
#ifdef EA_BUILD_A124
        bool cullStrips = updateMask & 8;
#endif
        WCollisionMgr(this->fExclusionFlags, 3)
            .GetInstanceList(this->fInstanceCacheList, this->fPosition, this->fRadius,
#ifdef EA_BUILD_A124
                             cullStrips,
#endif
                             this->fColliderShape == kColliderShape_Cylinder);

#ifdef EA_BUILD_A124
        if (cullStrips) {
#else
        if (updateMask & 0x8) {
#endif
            WCollisionMgr(this->fExclusionFlags, 3).GetTriList(this->fInstanceCacheList, this->fPosition, this->fRadius, this->fTriList);
        }
    }

    if (updateMask & 0x4) {
        this->fBarrierList.reserve(21);
        WCollisionMgr(this->fExclusionFlags, 3).GetBarrierList(this->fBarrierList, this->fInstanceCacheList, this->fPosition, this->fRadius);
    }

#ifndef EA_BUILD_A124
    if (updateMask & 0x10) {
        WCollisionMgr(fExclusionFlags, 3).GetObjectList(this->fObbList, this->fPosition, this->fRadius);
    }
#endif

    this->fRegionInitialized = true;
}

bool WCollider::IsEmpty() const {
    // TODO fObbList.empty()?
    return this->fInstanceCacheList.empty() && this->fBarrierList.empty();
}

void WCollider::Clear() {
    if (this->fRegionInitialized) {
        this->ClearLists(28);
        this->fRegionInitialized = false;
    }
}

void WCollider::ClearLists(unsigned int typeMask) {
    if (typeMask & 0x8) {
        this->fInstanceCacheList.clear();
        this->fTriList.clear_all();
    }

    if (typeMask & 0x4) {
        this->fBarrierList.clear();
    }

    if (typeMask & 0x10) {
        this->fObbList.clear();
    }
}

void WCollider::EmptyLists(unsigned int typeMask) {
    if (typeMask & 0x8) {
        this->fInstanceCacheList.resize(0);
        this->fTriList.clear_all();
    }

    if (typeMask & 0x4) {
        this->fBarrierList.resize(0);
    }

    if (typeMask & 0x10) {
        this->fObbList.resize(0);
    }
}

void WCollider::ReserveLists(unsigned int typeMask) {
    if (typeMask & 0x8) {
        this->fInstanceCacheList.reserve(100);
        this->fTriList.reserve_total();
    }

    if (typeMask & 0x4) {
        this->fBarrierList.reserve(25);
    }

    if (typeMask & 0x10) {
        this->fObbList.reserve(25);
    }
}

bool WCollider::Validate() const {
    if (!this->fRegionInitialized) {
        return false;
    } else {
        return true;
    }
}

unsigned int WCollider::GetUpdateMask(const UMath::Vector3 &pt, float radius) {
    unsigned int updateMask = this->fTypeMask & 0x10;
    if (!this->Validate() || !this->InRegion(pt, radius)) {
        updateMask |= this->fTypeMask & 0xC;
    }
    return updateMask;
}

bool WCollider::InRegion(const UMath::Vector3 &pt, float radius) const {
    float radDiff = fRadius - radius;
    if (radDiff < 0.0f) {
        return false;
    }
    float cpDiffSq = UMath::DistanceSquare(pt, this->fPosition);
    return cpDiffSq < radDiff * radDiff;
}

void WCollider::InvalidateAllCachedData() {
    List::const_iterator iter = GetList().begin();
    List::const_iterator end = GetList().end();
    while (iter != end) {
        (*iter)->Clear();
        (*iter)->fRegionInitialized = false;
        ++iter;
    }
}

void WCollisionObject::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    m = this->fMat;

    if (addXLate) {
        m[3][0] = this->fPosRadius.x;
        m[3][1] = this->fPosRadius.y;
        m[3][2] = this->fPosRadius.z;
        m[3][3] = 1.0f;
        return;
    }

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

float WCollisionInstance::CalcSphericalRadius() const {
    // TODO
    // float maxExtent = WWorldMath::wmax(fInvMatRow2Length.w, fInvPosRadius.w);
    // maxExtent = WWorldMath::wmax(maxExtent, fHeight);
    // return WWorldMath::wmax(maxExtent, fInvMatRow0Width.w);

    float maxExtent = (this->fInvMatRow2Length.w < this->fInvPosRadius.w) ? this->fInvPosRadius.w : this->fInvMatRow2Length.w;
    maxExtent = (this->fHeight < maxExtent) ? maxExtent : this->fHeight;
    return (this->fInvMatRow0Width.w < maxExtent) ? maxExtent : this->fInvMatRow0Width.w;
}

// STRIPPED
const char *WCollisionInstance::GetName() const {}

void WCollisionInstance::CalcPosition(UMath::Vector3 &pos) const {
    pos.x = (-this->fInvPosRadius.x * this->fInvMatRow0Width.x - this->fInvPosRadius.y * this->fInvMatRow0Width.y) -
            this->fInvPosRadius.z * this->fInvMatRow0Width.z;
    pos.z = (-this->fInvPosRadius.x * this->fInvMatRow2Length.x - this->fInvPosRadius.y * this->fInvMatRow2Length.y) -
            this->fInvPosRadius.z * fInvMatRow2Length.z;

    if (NeedsCrossProduct()) {
        UMath::Vector4 upVec;
        UMath::Crossxyz(reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow2Length),
                        reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow0Width), upVec);
        pos.y = (-this->fInvPosRadius.x * upVec.x - this->fInvPosRadius.y * upVec.y) - this->fInvPosRadius.z * upVec.z;
    } else {
        pos.y = -this->fInvPosRadius.y;
    }
}

void WCollisionInstance::MakeMatrix(UMath::Matrix4 &m, bool addXLate) const {
    m[0][0] = this->fInvMatRow0Width.x;
    m[0][1] = this->fInvMatRow0Width.y;
    m[0][2] = this->fInvMatRow0Width.z;
    m[0][3] = 0.0f;

    if (NeedsCrossProduct()) {
        UMath::Crossxyz(reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow2Length), reinterpret_cast<const UMath::Vector4 &>(this->fInvMatRow0Width),
                        m[1]);
        m[1][3] = 0.0f;
    } else {
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[1][3] = 0.0f;
    }

    m[2][0] = this->fInvMatRow2Length.x;
    m[2][1] = this->fInvMatRow2Length.y;
    m[2][2] = this->fInvMatRow2Length.z;
    m[2][3] = 0.0f;

    if (addXLate) {
        m[3][0] = this->fInvPosRadius.x;
        m[3][1] = this->fInvPosRadius.y;
        m[3][2] = this->fInvPosRadius.z;
        m[3][3] = 1.0f;
    } else {
        m[3][0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 0.0f;
        m[3][3] = 1.0f;
    }
}
