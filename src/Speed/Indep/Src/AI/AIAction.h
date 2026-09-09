//
//
//
//
//
//
//
//
//
//
//
#ifndef __AIACTION_H
#define __AIACTION_H 1 // Decl: 13

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

// total size: 0x4
// Dcel: 21
struct AIActionParams {
    AIActionParams(ISimable *owner) : mOwner(owner) {}

    ISimable *mOwner; // offset 0x0, size 0x4
};

// total size: 0x48
// Decl: 30
class AIAction : public Sim::Object, public UTL::COM::Factory<AIActionParams *, AIAction, UCrc32> {
  public:
    class List : public UTL::Std::list<AIAction *, _type_list> {
      public:
        USE_FASTALLOC(AIAction::List);
    };

    USE_FASTALLOC(AIAction);

    // Decl: 37
    ~AIAction() override {}

    virtual bool CanBeAttempted(float dT) = 0;
    virtual bool IsFinished() = 0;
    virtual void OnBehaviorChange(const UCrc32 &mechanic) = 0;
    virtual void BeginAction(float dT) = 0;
    virtual void FinishAction(float dT) = 0;
    virtual void Update(float dT) = 0;

    float GetScore() {
        return this->mScore;
    }

    IVehicleAI *GetAI() const {
        return this->mAI;
    }

    IVehicle *GetVehicle() const {
        return this->mVehicle;
    }

    ISimable *GetOwner() const {
        return this->mActionParams.mOwner;
    }

    const AIActionParams &GetActionParams() const {
        return this->mActionParams;
    }

    void SetActionName(const char *name) {
        this->mActionNameString = name;
        this->mActionNameCrc = name;
    }

    UCrc32 GetActionName() {
        return this->mActionNameCrc;
    }

    const char *GetActionNameString() {
        return this->mActionNameString;
    }

  protected:
    AIAction(AIActionParams *params, float score);

    virtual bool ShouldRestartWhenFinished() {
        return false;
    }

  private:
    AIActionParams mActionParams;  // offset 0x30, size 0x4
    const char *mActionNameString; // offset 0x34, size 0x4
    UCrc32 mActionNameCrc;         // offset 0x38, size 0x4
    IVehicle *mVehicle;            // offset 0x3C, size 0x4
    IVehicleAI *mAI;               // offset 0x40, size 0x4
    float mScore;                  // offset 0x44, size 0x4
};

#define BIND_AIACTION_FACTORY(_TYPE_) AIAction::Prototype _##_TYPE_(UCrc32(#_TYPE_), _TYPE_::Construct); // Decl: 99

#define AIACTION_SCORE_LOW 0.0f  // Decl: 102
#define AIACTION_SCORE_HIGH 1.0f // Decl: 103

// total size: 0x4
// Decl: 106
class performance_limiter {
  public:
    void init(float speed);
    void update(float speed, float maxspeed, float maxaccel, float dt);

    float get_speed_limit() {
        return this->speed_limit;
    }

  private:
    float speed_limit; // offset 0x0, size 0x4
};

#endif
