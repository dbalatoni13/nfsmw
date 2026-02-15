#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"

#include <algorithm>

typedef UTL::FixedVector<ISimable *, 10, 16> ValidTargetList;

ValidTargetList TheValidTargets;
bTList<AITarget> TheAITargets;

bool AITarget::CanAquire(const ISimable *who) {
    return std::find(TheValidTargets.begin(), TheValidTargets.end(), who) != TheValidTargets.end();
}

void AITarget::Register(ISimable *who) {
    TheValidTargets.push_back(who);
}

void AITarget::UnRegister(ISimable *who) {
    ValidTargetList::iterator iter = std::find(TheValidTargets.begin(), TheValidTargets.end(), who);
    TheValidTargets.erase(iter);

    for (AITarget *target = TheAITargets.GetHead(); target != TheAITargets.EndOfList(); target = target->GetNext()) {
        if (target->mTargetSimable == who) {
            target->Clear();
        }
    }
}

void AITarget::Track(const ISimable *who) {
    for (AITarget *target = TheAITargets.GetHead(); target != TheAITargets.EndOfList(); target = target->GetNext()) {
        if (target->mTargetSimable == who || target->mOwner == who) {
            target->TrackInternal();
        }
    }
}

void AITarget::TrackAll() {
    for (AITarget *target = TheAITargets.GetHead(); target != TheAITargets.EndOfList(); target = target->GetNext()) {
        target->TrackInternal();
    }
}

AITarget::AITarget(ISimable *owner)
    : mOwner(owner),                           //
      mTargetPosition(UMath::Vector3::kZero),  //
      mTargetSimable(nullptr),                 //
      mTargetDirection(UMath::Vector3::kZero), //
      mValid(false) {
    TheAITargets.AddTail(this);
}

AITarget::~AITarget() {
    TheAITargets.Remove(this);
}

void AITarget::Clear() {
    mValid = false;
    mTargetSimable = nullptr;
    mTargetPosition = UMath::Vector3::kZero;
    mTargetDirection = UMath::Vector3::kZero;
    mDistTo = 0.0f;
    mDirTo = UMath::Vector3::kZero;
}

void AITarget::Aquire(ISimable *target) {
    if (target == mTargetSimable) {
        return;
    }
    Clear();
    if (CanAquire(target)) {
        mTargetSimable = target;
        mValid = true;
        TrackInternal();
    }
}

void AITarget::Aquire(const UMath::Vector3 &position) {
    Clear();
    mTargetSimable = nullptr;
    mTargetPosition = position;
    mTargetDirection = UMath::Vector3::kZero;
    mValid = true;
    TrackInternal();
}

void AITarget::Aquire(const UMath::Vector3 &position, const UMath::Vector3 &direction) {
    Clear();
    mTargetSimable = nullptr;
    mTargetDirection = direction;
    mTargetPosition = position;
    mValid = true;
    TrackInternal();
}

void AITarget::Aquire(const AITarget *aitarget) {
    if (aitarget != this && aitarget && aitarget->IsValid()) {
        if (aitarget->IsSimable()) {
            Aquire(aitarget->GetSimable());
        } else {
            Aquire(aitarget->mTargetPosition);
        }
    }
}

bool AITarget::IsTarget(const AITarget *aitarget) const {
    if (!aitarget || !aitarget->IsValid() || !IsValid()) {
        return false;
    }
    if (aitarget->mTargetSimable) {
        return aitarget->mTargetSimable == mTargetSimable;
    } else {
        return UMath::Distance(mTargetPosition, aitarget->mTargetPosition) < 0.1f;
    }
}

float AITarget::GetSpeed() const {
    if (mTargetSimable) {
        return mTargetSimable->GetRigidBody()->GetSpeedXZ();
    } else {
        return 0.0f;
    }
}

const UMath::Vector3 &AITarget::GetLinearVelocity() const {
    if (mTargetSimable) {
        return mTargetSimable->GetRigidBody()->GetLinearVelocity();
    } else {
        return UMath::Vector3::kZero;
    }
}

void AITarget::TrackInternal() {
    if (!mValid) {
        return;
    }
    if (mTargetSimable) {
        mTargetPosition = mTargetSimable->GetPosition();
        mTargetSimable->GetRigidBody()->GetForwardVector(mTargetDirection);
    }
    if (mOwner) {
        mDistTo = UMath::Distance(mOwner->GetPosition(), mTargetPosition);
        UMath::Sub(mTargetPosition, mOwner->GetPosition(), mDirTo);
        UMath::Unit(mDirTo, mDirTo);
    }
}
