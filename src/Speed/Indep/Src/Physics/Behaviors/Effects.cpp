#include "Effects.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

Effects::Effects(const struct BehaviorParams &bp) : Behavior(bp, 0) {
    // TODO
}

Effects::~Effects() {
    Sim::Collision::RemoveListener(this);
    mScrape.Stop();
}

void Effects::Reset() {
    mScrape.Stop();
}

void Effects::OnPause() {
    mScrape.Stop();
}

void Effects::OnTaskSimulate(float dT) {
    float minspeed;

    if (mScrape.IsPaused()) {
        return;
    }
    if ((mScrapeTimeOut -= dT) >= 0.0f) {
        return;
    }
    mScrape.Pause(true);
}

void Effects::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mIRBComplex);
    }
    Behavior::OnBehaviorChange(mechanic);
}

bool Effects::DoScrape(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity, const UMath::Vector3 &normal,
                       const unsigned int actionkey, HSIMABLE hother) {
    float speed = UMath::Length(velocity);
    if (speed > 1.0f) {
        float speed = UMath::Length(velocity);
        mScrapeEffects.Init(othersurface, GetOwner()->GetAttributes(), actionkey);
        float minspeed = mScrapeEffects.GetMinSpeed();
        float maxspeed = mScrapeEffects.GetMaxSpeed();
        const Attrib::Collection *effect = mScrapeEffects.GetEffect();
        if (!effect || (speed < minspeed) || (maxspeed <= minspeed)) {
            mScrape.Pause(true);
            return false;
        }
        float intensity = (speed - minspeed) / (maxspeed - minspeed);
        UMath::Vector3 mag;
        UMath::Scale(normal, UMath::Clamp(intensity, 0.0f, 1.0f), mag);
        unsigned int actee = 0;
        if (hother) {
            ISimable *other = ISimable::FindInstance(hother);
            if (other) {
                actee = other->GetWorldID();
            }
        }
        mScrape.Set(effect, position, mag, othersurface.GetConstCollection(), actee != 0, 0);
        mScrape.Pause(false);
        mScrapeTimeOut = 0.2f;
        return true;
    } else {
        return false;
    }
}

bool Effects::DoHit(const SimSurface &othersurface, float impact, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                    const UMath::Vector3 &normal, const unsigned int actionkey, HSIMABLE hother) {
    float speed = UMath::LengthSquare(velocity);
    if (speed > 1.0f) {
        mHitEffects.Init(othersurface, GetOwner()->GetAttributes(), actionkey);
        const Attrib::Collection *effect = mHitEffects.GetEffect();
        float minimpact = mHitEffects.GetMinSpeed();
        float maximpact = mHitEffects.GetMaxSpeed();
        if (!effect) {
            return false;
        }
        float intensity = (maximpact - minimpact > FLOAT_EPSILON) ? UMath::Ramp(impact, minimpact, maximpact) : 0.0f;
        if (intensity <= 0.0f) {
            return false;
        }
        unsigned int actee = 0;
        if (hother) {
            ISimable *other = ISimable::FindInstance(hother);
            if (other) {
                actee = other->GetWorldID();
            }
        }
        UMath::Vector3 magnitude;
        UMath::Scale(normal, intensity, magnitude);
        Sim::Effect::Fire(effect, position, magnitude, GetOwner()->GetWorldID(), GetOwner()->GetAttributes().GetConstCollection(),
                          othersurface.GetConstCollection(), actee);
        return true;
    }
    return false;
}

void Effects::OnScrapeWorld(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                            const UMath::Vector3 &normal) {
    // OnScrapeWorld
    DoScrape(othersurface, position, velocity, normal, 0x7100960c, nullptr);
}

void Effects::OnHitWorld(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                         const UMath::Vector3 &normal) {
    // OnHitWorld
    DoHit(othersurface, impulse, position, velocity, normal, 0xe3167336, nullptr);
}

void Effects::OnScrapeObject(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal, HSIMABLE hother) {
    // OnScrapeObject
    DoScrape(othersurface, position, velocity, normal, 0xdacb1c11, hother);
}

void Effects::OnHitObject(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                          const UMath::Vector3 &normal, HSIMABLE hother) {
    // OnHitObject
    DoHit(othersurface, impulse, position, velocity, normal, 0x18915735, hother);
}

void Effects::OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal) {
    // OnScrapeGround
    DoScrape(othersurface, position, velocity, normal, 0xdb823931, nullptr);
}

void Effects::OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                          const UMath::Vector3 &normal) {
    // OnHitGround
    DoHit(othersurface, impulse, position, velocity, normal, 0xd9c6cdfd, nullptr);
}

void Effects::OnCollision(const COLLISION_INFO &cinfo) {
    // TODO annoying virtuals
}

EffectsVehicle::EffectsVehicle(const BehaviorParams &bp) : Effects(bp) {}

EffectsCar::EffectsCar(const BehaviorParams &bp) : EffectsVehicle(bp) {
    GetOwner()->QueryInterface(&mSuspension);
}

void EffectsCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
    Effects::OnBehaviorChange(mechanic);
}

void EffectsCar::OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal) {
    if (mSuspension && mSuspension->GetNumWheelsOnGround() > 2) {
        IRigidBody *body = GetOwner()->GetRigidBody();
        UMath::Vector3 vup;
        UMath::Vector3 vfwd;
        UMath::Vector3 dim;
        body->GetForwardVector(vfwd);
        body->GetUpVector(vup);
        body->GetDimension(dim);
        if (UMath::Dot(vup, normal) > VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            UMath::Vector3 pt = body->GetPosition();
            UMath::ScaleAdd(vup, -dim.y, pt, pt);
            UMath::ScaleAdd(vfwd, -dim.z * 0.75f, pt, pt);
            // OnBottomOut
            DoHit(othersurface, -UMath::Dot(velocity, normal), pt, velocity, normal, 0x31047ebc, nullptr);
            return;
        }
    }
    Effects::OnHitGround(othersurface, impulse, position, velocity, normal);
}

void EffectsCar::OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                                const UMath::Vector3 &normal) {
    if (mSuspension && mSuspension->GetNumWheelsOnGround() > 2) {
        IRigidBody *body = GetOwner()->GetRigidBody();
        UMath::Vector3 vup;
        UMath::Vector3 vfwd;
        UMath::Vector3 dim;
        body->GetForwardVector(vfwd);
        body->GetUpVector(vup);
        body->GetDimension(dim);
        if (UMath::Dot(vup, normal) > VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            UMath::Vector3 pt = body->GetPosition();
            UMath::ScaleAdd(vup, -dim.y, pt, pt);
            UMath::ScaleAdd(vfwd, -dim.z * 0.75f, pt, pt);
            // OnBottomScrape
            DoScrape(othersurface, pt, velocity, normal, 0xfc03ad6a, nullptr);
            return;
        }
    }
    Effects::OnScrapeGround(othersurface, position, velocity, normal);
}

EffectsPlayer::EffectsPlayer(const BehaviorParams &bp) : EffectsCar(bp) {}

EffectsSmackable::EffectsSmackable(const BehaviorParams &bp) : Effects(bp) {}

void EffectsSmackable::OnCollision(const COLLISION_INFO &cinfo) {
    HSIMABLE hsimable = GetOwner()->GetInstanceHandle();
    if (cinfo.objADetached && cinfo.objA == hsimable) {
        // OnDetached
        if (DoHit(SimSurface(cinfo.objBsurface), UMath::Length(cinfo.closingVel), cinfo.position, cinfo.closingVel, cinfo.normal, 0xd177bdaa,
                  cinfo.objB)) {
            return;
        }
    } else if (cinfo.objBDetached && cinfo.objB == hsimable) {
        // OnDetached
        if (DoHit(SimSurface(cinfo.objAsurface), UMath::Length(cinfo.closingVel), cinfo.position, cinfo.closingVel, cinfo.normal, 0xd177bdaa,
                  cinfo.objA)) {
            return;
        }
    }
    Effects::OnCollision(cinfo);
}

EffectsFragment::EffectsFragment(const BehaviorParams &bp) : Effects(bp) {}
