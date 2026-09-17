#ifndef __EXPLOSION_H
#define __EXPLOSION_H

#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

// total size: 0x30
struct ExplosionParams : public Sim::Param {
    // TODO macro
    static UCrc32 TypeName() {
        static UCrc32 value = "ExplosionParams";
        return value;
    }

    ExplosionParams(float expansion_speed, float start_radius, float radius, const UMath::Vector3 &position, HMODEL source, bool effect_source,
                    bool damage, unsigned int targets)
        : fPosition(position) {
        // TODO
    }

    const UMath::Vector3 &fPosition; // offset 0x10, size 0x4
    float fExpansionSpeed;           // offset 0x14, size 0x4
    float fRadius;                   // offset 0x18, size 0x4
    float fStartRadius;              // offset 0x1C, size 0x4
    HMODEL fSource;                  // offset 0x20, size 0x4
    bool fEffectSource;              // offset 0x24, size 0x1
    bool fDamage;                    // offset 0x28, size 0x1
    unsigned int fTargets;           // offset 0x2C, size 0x4
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
