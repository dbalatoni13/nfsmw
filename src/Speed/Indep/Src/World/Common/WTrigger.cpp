#include "Speed/Indep/Src/World/WTrigger.h"

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/Physics/Dynamics/Collision.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x8
struct FireOnExitRec {
    FireOnExitRec(WTrigger &trigger, HSIMABLE hSimable) : mTrigger(trigger), mhSimable(hSimable) {}

    bool operator==(const FireOnExitRec &rhs) const {
        return &mTrigger == &rhs.mTrigger && mhSimable == rhs.mhSimable;
    }

    bool operator<(const FireOnExitRec &rhs) const {
        if (mhSimable < rhs.mhSimable)
            return true;
        return &mTrigger < &rhs.mTrigger;
    }

    WTrigger &mTrigger; // offset 0x0, size 0x4
    HSIMABLE mhSimable; // offset 0x4, size 0x4
};

// total size: 0x10
struct FireOnExitList : public std::set<FireOnExitRec> {
    USE_FASTALLOC(FireOnExitList);
};

WTrigger::WTrigger() {
    bMemSet(this, 0, sizeof(WTrigger));
}

// TODO bitfield access in the whole file
WTrigger::WTrigger(const UMath::Matrix4 &mat, const UMath::Vector3 &dimensions, CARP::EventList *eventList, unsigned int flags) {
#ifdef SANE_CODE
    fMatRow0Width = mat[0];
    fMatRow2Length = mat[2];
    fPosRadius = mat[3];
#else
    *reinterpret_cast<UMath::Matrix4 *>(this) = mat;
#endif

    reinterpret_cast<unsigned char *>(this)[0x10] = (reinterpret_cast<unsigned char *>(this)[0x10] & 0xF0) | 1;
    fHeight = dimensions.y + dimensions.y;
    fEvents = eventList;
    *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) =
        (*reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) & 0xFF000000) | (flags & 0x00FFFFFF);
    reinterpret_cast<unsigned char *>(this)[0x10] &= 0x0F;
    fPosRadius.x = mat[3][0];
    fPosRadius.y = mat[3][1];
    fPosRadius.z = mat[3][2];
    fPosRadius.w = UMath::Length(dimensions);
    fMatRow0Width.w = dimensions.x + dimensions.x;
    fMatRow2Length.w = dimensions.z + dimensions.z;
}

// STRIPPED
WTrigger::WTrigger(const UMath::Matrix4 &mat, float radius, float height, CARP::EventList *eventList, unsigned int flags) {}

WTrigger::~WTrigger() {
    if (!((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x12]) << 8) & 0x100) && fEvents) {
        delete fEvents;
    }
    if (WTriggerManager::Exists()) {
        WTriggerManager::Get().DeleteRefs(this);
    }
}

void WTrigger::FireEvents(HSIMABLE hSimable) {
    CARP::EventStaticData *event = fEvents->Event();
    if (fEvents != nullptr) {
        gEventDynamicData.Clear();
        gEventDynamicData.fhSimable = reinterpret_cast<uintptr_t>(hSimable);
        gEventDynamicData.fPosition = fPosRadius;
        gEventDynamicData.fPosition.w = 1.0f;
        gEventDynamicData.fTrigger = this;
        gEventDynamicData.fTriggerStimulus = WTriggerManager::Get().GetCurrentStimulus();
        EventManager::FireEventList(fEvents, false);
    }
    unsigned int hi = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x11]) << 16;
    unsigned int mid = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x12]) << 8;
    unsigned int lo = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x13]);
    unsigned int flags = lo | (mid | hi);
    if (flags & 2) {
        // Disable();
        *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) =
            (*reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) & 0xFF000000u) | (flags & 0x00FFFFFEu);
    }
}

bool WTrigger::HasEvent(unsigned int eventID, const CARP::EventStaticData **foundEvent) const {
    if (fEvents != nullptr) {
        return EventManager::ListHasEvent(fEvents, eventID, foundEvent);
    }
    return false;
}

bool WTrigger::TestDirection(const UMath::Vector3 &vec) const {
    return UMath::Dot(UMath::Vector4To3(fMatRow2Length), vec) >= 0.0f;
}

// UNSOLVED, hacky...
void WTrigger::UpdateBox(const UMath::Matrix4 &mat, const UMath::Vector3 &dimension) {
    UMath::Vector4 oldPosRad = fPosRadius;
    unsigned int b11 = reinterpret_cast<const unsigned char *>(this)[0x11] << 16;
    unsigned int flags = reinterpret_cast<const unsigned char *>(this)[0x13] | (reinterpret_cast<const unsigned char *>(this)[0x12] << 8 | b11);
    CARP::EventList *eventList = fEvents;

#ifdef SANE_CODE
    fMatRow0Width = mat[0];
    fMatRow2Length = mat[2];
    fPosRadius = mat[3];
#else
    *reinterpret_cast<UMath::Matrix4 *>(this) = mat;
#endif

    reinterpret_cast<unsigned char *>(this)[0x10] = (reinterpret_cast<unsigned char *>(this)[0x10] & 0xF0) | 1;

    if (mat.v1.y < 0.999f) {
        flags |= 0x1000;
    } else {
        flags &= ~0x1000;
    }

    fHeight = dimension.y + dimension.y;
    fEvents = eventList;
    *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) =
        (*reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) & 0xFF000000) | (flags & 0x00FFFFFF);
    reinterpret_cast<unsigned char *>(this)[0x10] &= 0x0F;

    fPosRadius.x = mat[3][0];
    fPosRadius.y = mat[3][1];
    fPosRadius.z = mat[3][2];
    fPosRadius.w = UMath::Length(dimension);

    fMatRow0Width.w = dimension.x + dimension.x;
    fMatRow2Length.w = dimension.z + dimension.z;

    WGridManagedDynamicElem::AddElem(&oldPosRad, &fPosRadius, WGrid_kTrigger, reinterpret_cast<unsigned int>(this));
}

bool WTrigger::UpdatePos(const UMath::Vector3 &newPos, uintptr_t triggerInd) {
    UMath::Vector4 oldPosRad = fPosRadius;
    fPosRadius.x = newPos.x;
    fPosRadius.y = newPos.y;
    fPosRadius.z = newPos.z;
    WGridManagedDynamicElem::AddElem(&oldPosRad, &fPosRadius, WGrid_kTrigger, triggerInd);
    return true;
}

WTriggerManager *WTriggerManager::fgTriggerManager = nullptr;

WTriggerManager::WTriggerManager()
    : fSilencableEnabled(true), //
      fIterCount(0),            //
      fgFireOnExitList(nullptr) {
    fgFireOnExitList = new FireOnExitList();
}

WTriggerManager::~WTriggerManager() {
    delete fgFireOnExitList;
}

void WTriggerManager::Init() {
    fgTriggerManager = new WTriggerManager();
    Restart();
    for (unsigned int i = 0; i < WCollisionAssets::Get().NumTriggers(); i++) {
        WTrigger &trig = WCollisionAssets::Get().Trigger(i);
        if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x12]) << 8) & 0x200) {
            WTrigger &trig2 = WCollisionAssets::Get().Trigger(i);
            unsigned int hi = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig2)[0x11]) << 16;
            unsigned int mid = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig2)[0x12]) << 8;
            unsigned int lo = static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig2)[0x13]);
            unsigned int flags = lo | (mid | hi);
            *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(&trig2) + 0x10) =
                (*reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(&trig2) + 0x10) & 0xFF000000) | (flags & ~0x400);
        }
    }
}

void WTriggerManager::Restart() {
    fgTriggerManager->fgFireOnExitList->clear();
    Get().EnableSilencables();
}

void WTriggerManager::SubmitForFire(WTrigger &trig, HSIMABLE hSimable) {
    if (reinterpret_cast<const unsigned char *>(&trig)[0x12] >> 7) {
        ISimable *iSimable = ISimable::FindInstance(hSimable);
        if (iSimable != nullptr) {
            FireOnExitRec rec(trig, hSimable);
            fgFireOnExitList->insert(rec);
        } else {
            trig.FireEvents(hSimable);
        }
    }
    if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x11]) << 16 & 0x40000) != 0) {
        trig.FireEvents(hSimable);
    }
    unsigned int b11 = reinterpret_cast<const unsigned char *>(&trig)[0x11] << 16;
    if (((reinterpret_cast<const unsigned char *>(&trig)[0x12] << 8 | b11) & 0x48000) == 0) {
        trig.FireEvents(hSimable);
    }
}

void WTriggerManager::ProcessRB(IRigidBody *rBody, float dT) {
    fIterCount++;
    unsigned int activateFlag = rBody->GetTriggerFlags();
    if (activateFlag == 0) {
        return;
    }
    float radius = rBody->GetRadius();

    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(64);

    const WGrid &grid = WGrid::Get();
    grid.FindNodes(rBody->GetPosition(), radius, nodeInds);

    for (UTL::FastVector<unsigned int, 16>::iterator iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *gridNode = grid.fNodes[*iter];
        if (gridNode != nullptr) {
            const unsigned int *indPtr;
            WGridNode::iterator eIter(gridNode, WGrid_kTrigger);
            while ((indPtr = eIter.GetIndPtr())) {
                unsigned int ind = *indPtr;
                WTrigger &trig = WCollisionAssets::Get().Trigger(ind);
                if (trig.fIterStamp != fIterCount) {
                    trig.fIterStamp = fIterCount;
                    if (trig.IsEnabled(fSilencableEnabled) &&
                        (((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x11]) << 16) |
                          (static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x12]) << 8) |
                          static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x13])) &
                         activateFlag) != 0 &&
                        CheckCollideRB(rBody, &trig, dT)) {
                        SubmitForFire(trig, rBody->GetOwner()->GetInstanceHandle());
                    }
                }
            }
        }
    }
}

void WTriggerManager::ProcessSRB(IRigidBody *srBody, float dT) {
    fIterCount++;
    unsigned int activateFlag = srBody->GetTriggerFlags();
    if (activateFlag == 0) {
        return;
    }
    float radius = UMath::Max(srBody->GetRadius(), 1.5f);

    UTL::FastVector<unsigned int, 16> nodeInds;
    nodeInds.reserve(64);

    const WGrid &grid = WGrid::Get();
    grid.FindNodes(srBody->GetPosition(), radius, nodeInds);

    for (UTL::FastVector<unsigned int, 16>::iterator iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *gridNode = grid.fNodes[*iter];
        if (gridNode != nullptr) {
            const unsigned int *indPtr;
            WGridNode::iterator eIter(gridNode, WGrid_kTrigger);
            while ((indPtr = eIter.GetIndPtr())) {
                unsigned int ind = *indPtr;
                WTrigger &trig = WCollisionAssets::Get().Trigger(ind);
                if (trig.fIterStamp != fIterCount) {
                    trig.fIterStamp = fIterCount;
                    if (trig.IsEnabled(fSilencableEnabled) &&
                        (((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x11]) << 16) |
                          (static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x12]) << 8) |
                          static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x13])) &
                         activateFlag) != 0) {
                        if (srBody->GetOwner()->IsOwnedByPlayer() || !(reinterpret_cast<const unsigned char *>(&trig)[0x13] & 0x80)) {
                            if (CheckCollideSRB(srBody, &trig, dT)) {
                                SubmitForFire(trig, srBody->GetOwner()->GetInstanceHandle());
                            }
                        }
                    }
                }
            }
        }
    }
}

// UNSOLVED
bool WTriggerManager::CheckCollideRB(const IRigidBody *rBody, const WTrigger *trig, float dT) const {
    const float rbRadius = rBody->GetRadius();
    // const float rbRadiusPlusVel;
    UMath::Vector3 rPos;
    UMath::Vector3 cp;

    float radsSq = rBody->GetSpeed() * dT + rbRadius + trig->fPosRadius.w;
    cp = UMath::Vector4To3(trig->fPosRadius);
    radsSq *= radsSq;
    UMath::Vector3 dP;
    UMath::Scale(rBody->GetLinearVelocity(), dT, dP);
    UMath::Add(rBody->GetPosition(), dP, rPos);

    if ((reinterpret_cast<const unsigned char *>(trig)[0x10] & 0xF) == 2) {
        if (UMath::DistanceSquare(cp, rPos) <= radsSq) {
            if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(trig)[0x12]) << 8) & 0x800) {
                if (!trig->TestDirection(rBody->GetLinearVelocity())) {
                    return false;
                }
            }
            return true;
        }
    } else {
        if (UMath::DistanceSquarexz(cp, rPos) <= radsSq) {
            if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(trig)[0x12]) << 8) & 0x800) {
                if (!trig->TestDirection(rBody->GetLinearVelocity())) {
                    return false;
                }
            }
            if ((reinterpret_cast<const unsigned char *>(trig)[0x10] & 0xF) == 3) {
                if ((rPos.y + rbRadius >= trig->fPosRadius.y - trig->fHeight * 0.5f) &&
                    (rPos.y - rbRadius < trig->fPosRadius.y + trig->fHeight * 0.5f)) {
                    return true;
                }
            } else if ((reinterpret_cast<const unsigned char *>(trig)[0x10] & 0xF) == 1) {
                UMath::Vector3 dim3;
                UMath::Matrix4 bodyMat;
                rBody->GetDimension(dim3);
                rBody->GetMatrix4(bodyMat);

                Dynamics::Collision::Geometry carOBB(bodyMat, rPos, dim3, Dynamics::Collision::Geometry::BOX, dP);

                UMath::Matrix4 m;
                trig->MakeMatrix(m, false, false);

                UMath::Vector4 trigPos = trig->fPosRadius;
                trigPos.w = 1.0f;

                UMath::Vector3 trigDimension;
                trigDimension.x = trig->fMatRow0Width.w * 0.5f;
                trigDimension.y = trig->fHeight * 0.5f;
                trigDimension.z = trig->fMatRow2Length.w * 0.5f;

                Dynamics::Collision::Geometry trigOBB(m, UMath::Vector4To3(trigPos), trigDimension, Dynamics::Collision::Geometry::BOX,
                                                      UMath::Vector3::kZero);
                if (Dynamics::Collision::Geometry::FindIntersection(&carOBB, &trigOBB, &carOBB)) {
                    return true;
                }
            } else {
                unsigned char shapeNum;
            }
        }
    }
    return false;
}

bool WTriggerManager::CheckCollideSRB(const IRigidBody *srBody, const WTrigger *trig, float dT) const {
    UMath::Vector3 rPos = srBody->GetPosition();
    float srRadius = srBody->GetRadius();
    float srRadiusPlusVel = srBody->GetSpeed() * dT + srRadius;
    UMath::Vector3 dVdT;
    UMath::Scale(srBody->GetLinearVelocity(), dT, dVdT);
    UMath::Add(rPos, dVdT, rPos);
    UMath::Vector3 cp = UMath::Vector4To3(trig->fPosRadius);

    srRadiusPlusVel += trig->fPosRadius.w;
    float radsSq = srRadiusPlusVel * srRadiusPlusVel;

    if ((reinterpret_cast<const unsigned char *>(trig)[0x10] & 0xF) == 1) {
        if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(trig)[0x12]) << 8) & 0x800) {
            if (!trig->TestDirection(srBody->GetLinearVelocity())) {
                return false;
            }
        }
        // TODO
        UMath::Vector3 dim3 = {srRadius, srRadius, srRadius};
        UMath::Matrix4 bodyMat;
        srBody->GetMatrix4(bodyMat);
        Dynamics::Collision::Geometry srbOBB(bodyMat, rPos, dim3, Dynamics::Collision::Geometry::SPHERE, dVdT);

        UMath::Matrix4 m;
        trig->MakeMatrix(m, false, false);

        UMath::Vector4 trigPos = trig->fPosRadius;
        trigPos.w = 1.0f;

        UMath::Vector3 trigDimension;
        trigDimension.x = trig->fMatRow0Width.w * 0.5f;
        trigDimension.y = trig->fHeight * 0.5f;
        trigDimension.z = trig->fMatRow2Length.w * 0.5f;

        Dynamics::Collision::Geometry trigOBB(m, reinterpret_cast<UMath::Vector3 &>(trigPos), trigDimension, Dynamics::Collision::Geometry::BOX,
                                              UMath::Vector3::kZero);
        if (Dynamics::Collision::Geometry::FindIntersection(&trigOBB, &srbOBB, &trigOBB)) {
            return true;
        }
    } else if ((reinterpret_cast<const unsigned char *>(trig)[0x10] & 0xF) == 2) {
        if (UMath::DistanceSquare(cp, rPos) < radsSq) {
            if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(trig)[0x12]) << 8) & 0x800) {
                if (!trig->TestDirection(srBody->GetLinearVelocity())) {
                    return false;
                }
            }
            return true;
        }
    } else if ((reinterpret_cast<const unsigned char *>(trig)[0x10] & 0xF) == 3) {
        if (UMath::DistanceSquarexz(cp, rPos) < radsSq) {
            if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(trig)[0x12]) << 8) & 0x800) {
                if (!trig->TestDirection(srBody->GetLinearVelocity())) {
                    return false;
                }
            }
            if (rPos.y + srRadius >= trig->fPosRadius.y - trig->fHeight * 0.5f && rPos.y - srRadius < trig->fPosRadius.y + trig->fHeight * 0.5f) {
                return true;
            } else {
                unsigned char shapeNum;
            }
        }
    }
    return false;
}

inline float DistanceSquared_XZ(const UMath::Vector3 &a, const UMath::Vector3 &b) {
    float z = a.z - b.z;
    float x = a.x - b.x;
    return x * x + z * z;
}

void WTriggerManager::GetIntersectingTriggers(const UMath::Vector3 &pt, float radius, WTriggerList *triggerList) const {
    UTL::FastVector<unsigned int, 16> nodeInds;
    fIterCount++;
    nodeInds.reserve(0x40);
    const WGrid &grid = WGrid::Get();
    grid.FindNodes(pt, radius, nodeInds);
    for (unsigned int *iter = nodeInds.begin(); iter != nodeInds.end(); ++iter) {
        WGridNode *gridNode = grid.fNodes[*iter];
        if (gridNode != nullptr) {
            WGridNode::iterator eIter(gridNode, WGrid_kTrigger);
            while (const unsigned int *indPtr = eIter.GetIndPtr()) {
                unsigned int ind = *indPtr;
                WTrigger &trig = WCollisionAssets::Get().Trigger(ind);
                if (trig.fIterStamp != fIterCount) {
                    trig.fIterStamp = fIterCount;
                    if (DistanceSquared_XZ(pt, UMath::Vector4To3(trig.fPosRadius)) < (radius + trig.fPosRadius.w) * (radius + trig.fPosRadius.w)) {
                        triggerList->push_back(&trig);
                    }
                }
            }
        }
    }
}

void WTriggerManager::DeleteRefs(const WTrigger *trig) {
    std::set<FireOnExitRec>::const_iterator iter = fgFireOnExitList->begin();
    while (iter != fgFireOnExitList->end()) {
        const FireOnExitRec &rec = *iter;
        if (trig == &rec.mTrigger) {
            std::set<FireOnExitRec>::const_iterator newlocation = iter;
            ++newlocation;
            fgFireOnExitList->erase(iter);
            iter = newlocation;
            if (iter == fgFireOnExitList->end()) {
                return;
            }
        }
        ++iter;
    }
}

void WTriggerManager::ClearAllFireOnExit() {
    if (fgFireOnExitList != nullptr) {
        fgFireOnExitList->clear();
    }
}

void WTriggerManager::Update(float dT) {
    fProcessingStimulus = 1;
    IRigidBody *const *enditer = IRigidBody::GetList().end();
    for (IRigidBody *const *iter = IRigidBody::GetList().begin(); iter != enditer; ++iter) {
        IRigidBody *rigidBody = *iter;
        if (rigidBody->IsSimple()) {
            ProcessSRB(rigidBody, dT);
        } else {
            ProcessRB(rigidBody, dT);
        }
    }
    fProcessingStimulus = 2;
    std::set<FireOnExitRec>::const_iterator iter = fgFireOnExitList->begin();
    while (iter != fgFireOnExitList->end()) {
        const FireOnExitRec &rec = *iter;
        ISimable *iSimable = ISimable::FindInstance(rec.mhSimable);
        if (iSimable != nullptr) {
            IRigidBody *iRigidBody = iSimable->GetRigidBody();
            if (iRigidBody != nullptr) {
                bool result;
                if (iRigidBody->IsSimple()) {
                    result = CheckCollideSRB(iRigidBody, &rec.mTrigger, dT);
                } else {
                    result = CheckCollideRB(iRigidBody, &rec.mTrigger, dT);
                }
                if (result == true) {
                    ++iter;
                    continue;
                }
                rec.mTrigger.FireEvents(rec.mhSimable);
            }
        }
        std::set<FireOnExitRec>::const_iterator newlocation = iter;
        ++newlocation;
        fgFireOnExitList->erase(iter);
        iter = newlocation;
        if (iter == fgFireOnExitList->end()) {
            return;
        }
    }
}

int LoaderTrigger(bChunk *chunk) {
    return 0;
}

int UnloaderTrigger(bChunk *chunk) {
    return 0;
}
