#ifndef PHYSICS_BEHAVIORS_EFFECTS_H
#define PHYSICS_BEHAVIORS_EFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"

struct EffectLinkageRecord {
    // total size: 0x20
    Attrib::RefSpec mSurface; // offset 0x0, size 0xC
    Attrib::RefSpec mEffect;  // offset 0xC, size 0xC
    float mMinSpeed;          // offset 0x18, size 0x4
    float mMaxSpeed;          // offset 0x1C, size 0x4
};

// TODO here or in SimEffect.h?
class EffectLookup {
  public:
    const Attrib::Collection *GetEffect() {
        return mEffect.GetCollection();
    }

    void Init(const SimSurface &surface, const Attrib::Instance &source, unsigned int datakey) {
        if ((source.GetCollection() != mSourceKey) || (mDataKey != datakey) || (surface.GetCollection() != mSurfaceKey)) {
            mSourceKey = source.GetCollection();
            mSurfaceKey = surface.GetCollection();
            mDataKey = datakey;
            mEffect.Clean();
            mEffect = Attrib::RefSpec();
            mMinSpeed = 0.0f;
            mMaxSpeed = 0.0f;
            Attrib::Attribute list = source.Get(datakey);

            if (!list.IsValid()) {
                return;
            }
            SimSurface s((surface));
            const EffectLinkageRecord *record = Find(0xeec2271a, list);
            if (record) {
                mEffect = record->mEffect;
                mMinSpeed = record->mMinSpeed;
                mMaxSpeed = record->mMaxSpeed;
            }
            while (s != SimSurface::kNull) {
                record = Find(s.GetCollection(), list);
                if (record) {
                    mEffect = record->mEffect;
                    mMinSpeed = record->mMinSpeed;
                    mMaxSpeed = record->mMaxSpeed;
                    break;
                }
                s = s.GetParentSurface();
            }
        }
    }

    const EffectLinkageRecord *Find(unsigned int surfacekey, const Attrib::Attribute &list) {
        // TODO
    }

    float GetMinSpeed() {
        return mMinSpeed;
    }

    float GetMaxSpeed() {
        return mMaxSpeed;
    }

  private:
    // total size: 0x20
    Attrib::RefSpec mEffect;  // offset 0x0, size 0xC
    unsigned int mSourceKey;  // offset 0xC, size 0x4
    unsigned int mDataKey;    // offset 0x10, size 0x4
    unsigned int mSurfaceKey; // offset 0x14, size 0x4
    float mMinSpeed;          // offset 0x18, size 0x4
    float mMaxSpeed;          // offset 0x1C, size 0x4
};

// total size: 0xF4
class Effects : public Behavior, public Sim::Collision::IListener {
  public:
    Effects(const BehaviorParams &bp);
    bool DoScrape(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity, const UMath::Vector3 &normal,
                  const unsigned int actionkey, HSIMABLE hother);
    bool DoHit(const SimSurface &othersurface, float impact, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
               const UMath::Vector3 &normal, const unsigned int actionkey, HSIMABLE hother);

    // Overrides
    // IUnknown
    ~Effects() override;

    // Behavior
    void Reset() override;
    void OnPause() override;
    void OnTaskSimulate(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

    // Virtual methods
    void OnScrapeWorld(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                       const UMath::Vector3 &normal) override;
    void OnHitWorld(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                    const UMath::Vector3 &normal) override;
    void OnScrapeObject(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity, const UMath::Vector3 &normal,
                        HSIMABLE hother) override;
    void OnHitObject(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                     const UMath::Vector3 &normal, HSIMABLE hother) override;
    void OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                        const UMath::Vector3 &normal) override;
    void OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                     const UMath::Vector3 &normal) override;

  private:
    ICollisionBody *mIRBComplex; // offset 0x50, size 0x4
    EffectLookup mHitEffects;    // offset 0x54, size 0x20
    EffectLookup mScrapeEffects; // offset 0x74, size 0x20
    Sim::Effect mScrape;         // offset 0x94, size 0x5C
    float mScrapeTimeOut;        // offset 0xF0, size 0x4
};

// total size: 0xF4
class EffectsVehicle : public Effects {
  public:
    EffectsVehicle(const BehaviorParams &bp);
};

// total size: 0xF8
class EffectsCar : public EffectsVehicle {
  public:
    EffectsCar(const BehaviorParams &bp);

    // Overrides
    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    // Effects
    void OnHitGround(const SimSurface &othersurface, float impulse, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                     const UMath::Vector3 &normal) override;
    void OnScrapeGround(const SimSurface &othersurface, const UMath::Vector3 &position, const UMath::Vector3 &velocity,
                        const UMath::Vector3 &normal) override;

  private:
    ISuspension *mSuspension; // offset 0xF4, size 0x4
};

// total size: 0xF8
class EffectsPlayer : public EffectsCar {
  public:
    EffectsPlayer(const BehaviorParams &bp);
};

// total size: 0xF4
class EffectsSmackable : public Effects {
  public:
    EffectsSmackable(const BehaviorParams &bp);

    // Overrides
    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;
};

// total size: 0xF4
class EffectsFragment : public Effects {
  public:
    EffectsFragment(const BehaviorParams &bp);
};

#endif
