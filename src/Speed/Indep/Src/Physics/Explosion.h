#ifndef __EXPLOSION_H
#define __EXPLOSION_H

#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"

// total size: 0x30
struct ExplosionParams : public Sim::Param {
    static UCrc32 TypeName() {
        static UCrc32 value = "ExplosionParams";
        return value;
    }

    ExplosionParams(float expansion_speed, float start_radius, float radius, const UMath::Vector3 &position, HMODEL source, bool effect_source,
                    bool damage, unsigned int targets)
        : Sim::Param(UCrc32(UCRC32_BASE), this), fPosition(position), fExpansionSpeed(expansion_speed), fRadius(radius),
          fStartRadius(start_radius), fSource(source), fEffectSource(effect_source), fDamage(damage), fTargets(targets) {}

    const UMath::Vector3 &fPosition; // offset 0x10, size 0x4
    float fExpansionSpeed;           // offset 0x14, size 0x4
    float fRadius;                   // offset 0x18, size 0x4
    float fStartRadius;              // offset 0x1C, size 0x4
    HMODEL fSource;                  // offset 0x20, size 0x4
    bool fEffectSource;              // offset 0x24, size 0x1
    bool fDamage;                    // offset 0x28, size 0x1
    unsigned int fTargets;           // offset 0x2C, size 0x4
};

// total size: 0xDC
class Explosion : public PhysicsObject, public IExplosion {
  public:
    USE_FASTALLOC(Explosion);

    static ISimable *Construct(Sim::Param param);

    Explosion(const ExplosionParams &params, Sim::Param sp);
    ~Explosion() override;

    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void OnCollide(IRigidBody *other, float dT, float radius, const Dynamics::Collision::Geometry &explosion_sphere);
    float GetRadius() const override;
    void TestCollisions(float dT);
    void OnTaskSimulate(float dT) override;

    // NOTE: also overrides IExplosion (interface header is missing these)
    HMODEL GetSource() const;
    float GetExpansionSpeed() const;
    float GetMaximumRadius() const;
    const UMath::Vector3 &GetOrigin() const;
    bool HasDamage() const;
    unsigned int GetTargets() const;

    // Overrides: ISimable
    void SetCausality(HCAUSE from, float time) override;
    HCAUSE GetCausality() const override;
    float GetCausalityTime() const override;
    IModel *GetModel() override;
    const IModel *GetModel() const override;

  private:
    const float mExpansionSpeed;  // offset 0xB8, size 0x4
    const float mExpansionRadius; // offset 0xBC, size 0x4
    HMODEL const mSource;         // offset 0xC0, size 0x4
    ISimpleBody *mIRBSimple;      // offset 0xC4, size 0x4
    bool mEffectSource;           // offset 0xC8, size 0x1
    HCAUSE mCausality;            // offset 0xCC, size 0x4
    float mCauseTime;             // offset 0xD0, size 0x4
    const bool mDamages;          // offset 0xD4, size 0x1
    const unsigned int mTargets;  // offset 0xD8, size 0x4
};

#endif
