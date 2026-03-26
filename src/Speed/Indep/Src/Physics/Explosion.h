#ifndef PHYSICS_EXPLOSION_H
#define PHYSICS_EXPLOSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

struct ExplosionParams : public Sim::Param {
    const UMath::Vector3 &fPosition;
    float fExpansionSpeed;
    float fRadius;
    float fStartRadius;
    HMODEL fSource;
    bool fEffectSource;
    bool fDamage;
    unsigned int fTargets;

    static UCrc32 TypeName() {
        static UCrc32 value("ExplosionParams");
        return value;
    }
};

class Explosion : public PhysicsObject, public IExplosion {
  public:
    static ISimable *Construct(Sim::Param params);
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    virtual const UMath::Vector3 &GetOrigin() const override {
        return PhysicsObject::GetPosition();
    }

    virtual float GetExpansionSpeed() const override {
        return mExpansionSpeed;
    }

    virtual float GetMaximumRadius() const override {
        return mExpansionRadius;
    }

    virtual float GetRadius() const override;

    virtual HCAUSE GetCausality() const override {
        return mCausality;
    }

    virtual float GetCausalityTime() const override {
        return mCauseTime;
    }

    virtual bool HasDamage() const override {
        return mDamages;
    }

    virtual unsigned int GetTargets() const override {
        return mTargets;
    }

    virtual HMODEL GetSource() const override {
        return mSource;
    }

    virtual void SetCausality(HCAUSE from, float time) override {
        mCausality = from;
        mCauseTime = time;
    }

    virtual IModel *GetModel() override {
        return nullptr;
    }

    virtual const IModel *GetModel() const override {
        return nullptr;
    }

  protected:
    Explosion(const ExplosionParams &params, Sim::Param sp);
    virtual ~Explosion();
    virtual void OnTaskSimulate(float dT) override;
    virtual void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    void TestCollisions(float dT);
    void OnCollide(IRigidBody *other, float dT, float radius, const Dynamics::Collision::Geometry &explosion_sphere);

    const float mExpansionSpeed;
    const float mExpansionRadius;
    const HMODEL mSource;
    ISimpleBody *mIRBSimple;
    bool mEffectSource;
    HCAUSE mCausality;
    float mCauseTime;
    const bool mDamages;
    const unsigned int mTargets;
};

#endif
