#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"

#include <algorithm>

typedef UTL::FixedVector<ISimable *, 10, 16> ValidTargetList;

ValidTargetList TheValidTargets;
bTList<AITarget> TheAITargets;

bool AITarget::CanAquire(const ISimable *who) {
    if (std::find(TheValidTargets.begin(), TheValidTargets.end(), who) == TheValidTargets.end()) {
        return false;
    }
    return true;
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
    this->mValid = false;
    this->mTargetSimable = nullptr;
    this->mTargetPosition = UMath::Vector3::kZero;
    this->mTargetDirection = UMath::Vector3::kZero;
    this->mDistTo = 0.0f;
    this->mDirTo = UMath::Vector3::kZero;
}

void AITarget::Aquire(ISimable *target) {
    if (target == this->mTargetSimable) {
        return;
    }
    this->Clear();
    if (CanAquire(target)) {
        this->mTargetSimable = target;
        this->mValid = true;
        this->TrackInternal();
    }
}

void AITarget::Aquire(const UMath::Vector3 &position) {
    this->Clear();
    this->mTargetSimable = nullptr;
    this->mTargetPosition = position;
    this->mTargetDirection = UMath::Vector3::kZero;
    this->mValid = true;
    this->TrackInternal();
}

void AITarget::Aquire(const UMath::Vector3 &position, const UMath::Vector3 &direction) {
    this->Clear();
    this->mTargetSimable = nullptr;
    this->mTargetDirection = direction;
    this->mTargetPosition = position;
    this->mValid = true;
    this->TrackInternal();
}

void AITarget::Aquire(const AITarget *aitarget) {
    if (aitarget != this && aitarget != nullptr && aitarget->IsValid()) {
        if (aitarget->IsSimable()) {
            this->Aquire(aitarget->GetSimable());
        } else {
            this->Aquire(aitarget->mTargetPosition);
        }
    }
}

bool AITarget::IsTarget(const AITarget *aitarget) const {
    if (aitarget == nullptr || !aitarget->IsValid() || !this->IsValid()) {
        return false;
    }
    if (aitarget->mTargetSimable != nullptr) {
        return aitarget->mTargetSimable == this->mTargetSimable;
    } else {
        return UMath::Distance(this->mTargetPosition, aitarget->mTargetPosition) < 0.1f;
    }
}

float AITarget::GetSpeed() const {
    if (this->mTargetSimable != nullptr) {
        return this->mTargetSimable->GetRigidBody()->GetSpeedXZ();
    } else {
        return 0.0f;
    }
}

const UMath::Vector3 &AITarget::GetLinearVelocity() const {
    if (this->mTargetSimable != nullptr) {
        return this->mTargetSimable->GetRigidBody()->GetLinearVelocity();
    } else {
        return UMath::Vector3::kZero;
    }
}

void AITarget::TrackInternal() {
    if (!this->mValid) {
        return;
    }
    if (this->mTargetSimable != nullptr) {
        this->mTargetPosition = this->mTargetSimable->GetPosition();
        this->mTargetSimable->GetRigidBody()->GetForwardVector(this->mTargetDirection);
    }
    if (this->mOwner != nullptr) {
        this->mDistTo = UMath::Distance(this->mOwner->GetPosition(), this->mTargetPosition);
        UMath::Sub(this->mTargetPosition, this->mOwner->GetPosition(), this->mDirTo);
        UMath::Unit(this->mDirTo, this->mDirTo);
    }
}
