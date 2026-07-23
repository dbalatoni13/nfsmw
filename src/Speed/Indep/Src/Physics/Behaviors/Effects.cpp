#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

// total size: 0x20
class EffectLookup {
  public:
    EffectLookup()
        : mEffect(),       //
          mSourceKey(0),   //
          mDataKey(0),     //
          mSurfaceKey(0),  //
          mMinSpeed(0.0f), //
          mMaxSpeed(0.0f) {}

    float GetMinSpeed() {
        return mMinSpeed;
    }

    float GetMaxSpeed() {
        return mMaxSpeed;
    }

    void Init(const SimSurface &surface, const Attrib::Instance &source, Attrib::Key datakey) {
        if ((source.GetCollection() != this->mSourceKey) || (this->mDataKey != datakey) || (surface.GetCollection() != this->mSurfaceKey)) {
            this->mSourceKey = source.GetCollection();
            this->mSurfaceKey = surface.GetCollection();
            this->mDataKey = datakey;
            this->mEffect.Clean();
            this->mEffect = Attrib::RefSpec();
            this->mMinSpeed = 0.0f;
            this->mMaxSpeed = 0.0f;
            Attrib::Attribute list = source.Get(datakey);

            if (!list.IsValid()) {
                return;
            }
            SimSurface s((surface));
            const EffectLinkageRecord *record = this->Find(Attrib::key_default, list);
            if (record != nullptr) {
                this->mEffect = record->mEffect;
                this->mMinSpeed = record->mMinSpeed;
                this->mMaxSpeed = record->mMaxSpeed;
            }
            while (s != SimSurface::kNull) {
                record = this->Find(s.GetCollection(), list);
                if (record != nullptr) {
                    this->mEffect = record->mEffect;
                    this->mMinSpeed = record->mMinSpeed;
                    this->mMaxSpeed = record->mMaxSpeed;
                    break;
                }
                s = s.GetParentSurface();
            }
        }
    }

    const Attrib::Collection *GetEffect() {
        return this->mEffect.GetCollection();
    }

  private:
    const EffectLinkageRecord *Find(unsigned int surfacekey, const Attrib::Attribute &list) const {
        for (unsigned int i = 0; i < list.GetLength(); i++) {
            const EffectLinkageRecord &record = list.Get<EffectLinkageRecord>(i);
            if (record.mSurface.GetCollectionKey() == surfacekey) {
                return &record;
            }
        }
        return nullptr;
    }

    Attrib::RefSpec mEffect; // offset 0x0, size 0xC
    Attrib::Key mSourceKey;  // offset 0xC, size 0x4
    Attrib::Key mDataKey;    // offset 0x10, size 0x4
    Attrib::Key mSurfaceKey; // offset 0x14, size 0x4
    float mMinSpeed;         // offset 0x18, size 0x4
    float mMaxSpeed;         // offset 0x1C, size 0x4
};

// total size: 0xF4
class Effects : public Behavior, public Sim::Collision::IListener {
  private:
    ICollisionBody *mIRBComplex; // offset 0x50, size 0x4
    EffectLookup mHitEffects;    // offset 0x54, size 0x20
    EffectLookup mScrapeEffects; // offset 0x74, size 0x20
    Sim::Effect mScrape;         // offset 0x94, size 0x5C
    float mScrapeTimeOut;        // offset 0xF0, size 0x4

  public:
    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

  protected:
    Effects(const BehaviorParams &bp);

    // IUnknown
    ~Effects() override;

    bool DoHit(const SimSurface &othersurface, float impact, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
               const UMath::Vector3 &normal, const unsigned int actionkey, HSIMABLE hother);
    bool DoScrape(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity, const UMath::Vector3 &normal,
                  const unsigned int actionkey, HSIMABLE hother);

    virtual void OnHitWorld(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                            const UMath::Vector3 &normal);
    virtual void OnScrapeWorld(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                               const UMath::Vector3 &normal);
    virtual void OnHitObject(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal, HSIMABLE hother);
    virtual void OnScrapeObject(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                                const UMath::Vector3 &normal, HSIMABLE hother);
    virtual void OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal);
    virtual void OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                                const UMath::Vector3 &normal);

    // Behavior
    void OnPause() override;
};

Effects::Effects(const struct BehaviorParams &bp)
    : Behavior(bp, 0),      //
      mIRBComplex(nullptr), //
      mScrapeTimeOut(0.0f), //
      mScrape(static_cast<ISimable *>(bp.fowner)->GetWorldID(), static_cast<ISimable *>(bp.fowner)->GetAttributes().GetConstCollection()) {
    Sim::Collision::AddListener(this, GetOwner(), "Effects");
}

Effects::~Effects() {
    Sim::Collision::RemoveListener(this);
}

void Effects::Reset() {
    this->mScrape.Stop();
}

void Effects::OnPause() {
    this->mScrape.Stop();
}

void Effects::OnTaskSimulate(float dT) {
    float minspeed;

    if (this->mScrape.IsPaused()) {
        return;
    }
    if ((this->mScrapeTimeOut -= dT) >= 0.0f) {
        return;
    }
    this->mScrape.Pause(true);
}

void Effects::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mIRBComplex);
    }
    Behavior::OnBehaviorChange(mechanic);
}

bool Effects::DoScrape(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity, const UMath::Vector3 &normal,
                       const Attrib::Key actionkey, HSIMABLE hother) {
    float speed = UMath::Length(velocity);
    if (speed > 1.0f) {
        float speed = UMath::Length(velocity);
        this->mScrapeEffects.Init(othersurface, this->GetOwner()->GetAttributes(), actionkey);
        float minspeed = this->mScrapeEffects.GetMinSpeed();
        float maxspeed = this->mScrapeEffects.GetMaxSpeed();
        const Attrib::Collection *effect = this->mScrapeEffects.GetEffect();
        if ((effect == nullptr) || (speed < minspeed) || (maxspeed <= minspeed)) {
            this->mScrape.Pause(true);
            return false;
        }
        float intensity = (speed - minspeed) / (maxspeed - minspeed);
        UMath::Vector3 mag;
        UMath::Scale(normal, UMath::Clamp(intensity, 0.1f, 1.0f), mag);
        unsigned int actee = 0;
        if (hother != nullptr) {
            ISimable *other = ISimable::FindInstance(hother);
            if (other != nullptr) {
                actee = other->GetWorldID();
            }
        }
        this->mScrape.Set(effect, position, mag, othersurface.GetConstCollection(), actee != 0, 0);
        this->mScrape.Pause(false);
        this->mScrapeTimeOut = 0.2f;
        return true;
    } else {
        return false;
    }
}

static const float Tweak_MinSpeedForImpacts = 1.0f;
bool Effects::DoHit(const SimSurface &othersurface, float impact, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                    const UMath::Vector3 &normal, const unsigned int actionkey, HSIMABLE hother) {
    float speed = UMath::LengthSquare(velocity);
    if (speed > Tweak_MinSpeedForImpacts) {
        this->mHitEffects.Init(othersurface, this->GetOwner()->GetAttributes(), actionkey);
        const Attrib::Collection *effect = this->mHitEffects.GetEffect();
        float minimpact = this->mHitEffects.GetMinSpeed();
        float maximpact = this->mHitEffects.GetMaxSpeed();
        if (effect == nullptr) {
            return false;
        }
        float intensity = UMath::Ramp(impact, minimpact, maximpact);
        if (intensity <= 0.0f) {
            return false;
        }
        unsigned int actee = 0;
        if (hother != nullptr) {
            ISimable *other = ISimable::FindInstance(hother);
            if (other != nullptr) {
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
    this->DoScrape(othersurface, position, velocity, normal, Attrib::Hash::pvehicle::OnScrapeWorld, nullptr);
}

void Effects::OnHitWorld(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                         const UMath::Vector3 &normal) {
    this->DoHit(othersurface, impulse, position, velocity, normal, Attrib::Hash::pvehicle::OnHitWorld, nullptr);
}

void Effects::OnScrapeObject(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal, HSIMABLE hother) {
    this->DoScrape(othersurface, position, velocity, normal, Attrib::Hash::pvehicle::OnScrapeObject, hother);
}

void Effects::OnHitObject(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                          const UMath::Vector3 &normal, HSIMABLE hother) {
    this->DoHit(othersurface, impulse, position, velocity, normal, Attrib::Hash::pvehicle::OnHitObject, hother);
}

void Effects::OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal) {
    this->DoScrape(othersurface, position, velocity, normal, Attrib::Hash::pvehicle::OnScrapeGround, nullptr);
}

void Effects::OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                          const UMath::Vector3 &normal) {
    this->DoHit(othersurface, impulse, position, velocity, normal, Attrib::Hash::pvehicle::OnHitGround, nullptr);
}

void Effects::OnCollision(const COLLISION_INFO &cinfo) {
    if (this->IsPaused()) {
        return;
    }

    if (cinfo.type == Sim::Collision::Info::WORLD) {
        this->OnHitWorld(SimSurface(cinfo.objBsurface), cinfo.impulseA, cinfo.position, cinfo.closingVel, cinfo.normal);

        this->OnScrapeWorld(SimSurface(cinfo.objBsurface), cinfo.position, cinfo.slidingVel, cinfo.normal);
    }

    if (cinfo.type == Sim::Collision::Info::OBJECT) {
        UMath::Vector3 normal = cinfo.normal;
        SimSurface surface(cinfo.objBsurface);
        float impulse = cinfo.impulseA;
        HSIMABLE hother = cinfo.objB;

        if (hother == this->GetOwner()->GetInstanceHandle()) {
            UMath::Scale(normal, -1.0f);
            surface = SimSurface(cinfo.objAsurface);
            impulse = cinfo.impulseB;
            hother = cinfo.objA;
        }

        this->OnHitObject(surface, impulse, cinfo.position, cinfo.closingVel, normal, hother);

        this->OnScrapeObject(surface, cinfo.position, cinfo.slidingVel, normal, hother);
    }

    if (cinfo.type == Sim::Collision::Info::GROUND) {
        this->OnHitGround(SimSurface(cinfo.objBsurface), cinfo.impulseA, cinfo.position, cinfo.closingVel, cinfo.normal);

        this->OnScrapeGround(SimSurface(cinfo.objBsurface), cinfo.position, cinfo.slidingVel, cinfo.normal);
    }
}

// total size: 0xF4
class EffectsVehicle : public Effects {
  public:
    static Behavior *Construct(const BehaviorParams &params) {
        return new EffectsVehicle(params);
    }

    EffectsVehicle(const BehaviorParams &bp);
};

BIND_BEHAVIOR_FACTORY(EffectsVehicle);

EffectsVehicle::EffectsVehicle(const BehaviorParams &bp) : Effects(bp) {}

// total size: 0xF8
class EffectsCar : public EffectsVehicle {
  public:
    typedef EffectsVehicle Base;

    static Behavior *Construct(const BehaviorParams &params) {
        return new EffectsCar(params);
    }

    EffectsCar(const BehaviorParams &bp);

    // Effects
    void OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                     const UMath::Vector3 &normal) override;
    void OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                        const UMath::Vector3 &normal) override;

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    ISuspension *mSuspension; // offset 0xF4, size 0x4
};

BIND_BEHAVIOR_FACTORY(EffectsCar);

EffectsCar::EffectsCar(const BehaviorParams &bp) : EffectsVehicle(bp) {
    this->GetOwner()->QueryInterface(&mSuspension);
}

void EffectsCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&mSuspension);
    }
    Effects::OnBehaviorChange(mechanic);
}

void EffectsCar::OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                             const UMath::Vector3 &normal) {
    if ((this->mSuspension != nullptr) && this->mSuspension->GetNumWheelsOnGround() > 2) {
        IRigidBody *body = this->GetOwner()->GetRigidBody();
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
            this->DoHit(othersurface, -UMath::Dot(velocity, normal), pt, velocity, normal, Attrib::Hash::pvehicle::OnBottomOut, nullptr);
            return;
        }
    }
    Effects::OnHitGround(othersurface, impulse, position, velocity, normal);
}

void EffectsCar::OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                                const UMath::Vector3 &normal) {
    if ((this->mSuspension != nullptr) && this->mSuspension->GetNumWheelsOnGround() > 2) {
        IRigidBody *body = this->GetOwner()->GetRigidBody();
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
            this->DoScrape(othersurface, pt, velocity, normal, Attrib::Hash::pvehicle::OnBottomScrape, nullptr);
            return;
        }
    }
    Effects::OnScrapeGround(othersurface, position, velocity, normal);
}

// total size: 0xF8
class EffectsPlayer : public EffectsCar {
  public:
    typedef EffectsCar Base;

    static Behavior *Construct(const BehaviorParams &params) {
        return new EffectsPlayer(params);
    }

    EffectsPlayer(const BehaviorParams &bp);
};

BIND_BEHAVIOR_FACTORY(EffectsPlayer);

EffectsPlayer::EffectsPlayer(const BehaviorParams &bp) : EffectsCar(bp) {}

// total size: 0xF4
class EffectsSmackable : public Effects {
  public:
    static Behavior *Construct(const BehaviorParams &params) {
        return new EffectsSmackable(params);
    }

    EffectsSmackable(const BehaviorParams &bp);

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;
};

BIND_BEHAVIOR_FACTORY(EffectsSmackable);

EffectsSmackable::EffectsSmackable(const BehaviorParams &bp) : Effects(bp) {}

void EffectsSmackable::OnCollision(const COLLISION_INFO &cinfo) {
    HSIMABLE hsimable = GetOwner()->GetInstanceHandle();
    if (cinfo.objADetached && cinfo.objA == hsimable) {
        if (this->DoHit(SimSurface(cinfo.objBsurface), UMath::Length(cinfo.closingVel), cinfo.position, cinfo.closingVel, cinfo.normal,
                        Attrib::Hash::smackable::OnDetached, cinfo.objB)) {
            return;
        }
    } else if (cinfo.objBDetached && cinfo.objB == hsimable) {
        if (this->DoHit(SimSurface(cinfo.objAsurface), UMath::Length(cinfo.closingVel), cinfo.position, cinfo.closingVel, cinfo.normal,
                        Attrib::Hash::smackable::OnDetached, cinfo.objA)) {
            return;
        }
    }
    Effects::OnCollision(cinfo);
}

// total size: 0xF4
class EffectsFragment : public Effects {
  public:
    static Behavior *Construct(const BehaviorParams &params) {
        return new EffectsFragment(params);
    }

    EffectsFragment(const BehaviorParams &bp);
};

BIND_BEHAVIOR_FACTORY(EffectsFragment);

EffectsFragment::EffectsFragment(const BehaviorParams &bp) : Effects(bp) {}
