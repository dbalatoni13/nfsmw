#ifndef AI_AIACTION_H
#define AI_AIACTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

// total size: 0x4
struct AIActionParams {
    AIActionParams(ISimable *owner) : mOwner(owner) {}

    ISimable *mOwner; // offset 0x0, size 0x4
};

// total size: 0x48
class AIAction : public Sim::Object, public UTL::COM::Factory<AIActionParams *, AIAction, UCrc32> {
  public:
    struct List : public UTL::Std::list<AIAction *, _type_list> {
        // void *operator new(size_t size, void *ptr) {}

        // void operator delete(void *mem, void *ptr) {}

        // void *operator new(size_t size) {}

        void operator delete(void *mem, size_t size) {
            if (mem) {
                return gFastMem.Free(mem, size, nullptr);
            }
        }

        // void *operator new(size_t size, const char *name) {}

        // void operator delete(void *mem, const char *name) {}

        // void operator delete(void *mem, size_t size, const char *name) {}
    };

    AIAction(AIActionParams *params, float score);
    ~AIAction() override {}

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t bytes) {
        if (mem) {
            return gFastMem.Free(mem, bytes, nullptr);
        }
    }

    // Virtual functions
    virtual bool CanBeAttempted(float dT);
    virtual bool IsFinished();
    virtual void OnBehaviorChange(const UCrc32 &mechanic);
    virtual void BeginAction(float dT);
    virtual void FinishAction(float dT);
    virtual void Update(float dT);

    ISimable *GetOwner() const {
        return mActionParams.mOwner;
    }

    const AIActionParams &GetActionParams() const {
        return mActionParams;
    }

    const char *GetActionNameString() {
        return mActionNameString;
    }

    void SetActionName(const char *name) {
        mActionNameString = name;
        mActionNameCrc = name;
    }

    UCrc32 GetActionName() {
        return mActionNameCrc;
    }

    IVehicle *GetVehicle() const {
        return mVehicle;
    }

    IVehicleAI *GetAI() const {
        return mAI;
    }

    float GetScore() {
        return mScore;
    }

  protected:
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

// total size: 0x4
class performance_limiter {
  public:
    void init(float speed);
    void update(float speed, float maxspeed, float maxaccel, float dt);

    float get_speed_limit() {
        return speed_limit;
    }

  private:
    float speed_limit; // offset 0x0, size 0x4
};

#endif
