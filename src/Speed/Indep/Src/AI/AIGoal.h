#ifndef AI_AIGOAL_H
#define AI_AIGOAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// total size: 0x18
class AIGoal : public UTL::COM::Factory<ISimable *, AIGoal, UCrc32> {
  public:
    virtual ~AIGoal();
    virtual void OnBehaviorChange(const UCrc32 &mechanic);
    virtual void ChooseAction(float dT);
    virtual void Update(float dT);

    // void *operator new(size_t size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    // void *operator new(size_t size) {}

    // void operator delete(void *mem, size_t size) {}

    // void *operator new(size_t size, const char *name) {}

    // void operator delete(void *mem, const char *name) {}

    // void operator delete(void *mem, size_t size, const char *name) {}

    // UCrc32 GetActionName() {}

    // bool IsCurrentAction(const UCrc32 name) {}

  protected:
    AIGoal(ISimable *isimable);

    ISimable *GetOwner() const {
        return mOwner;
    }

    void AddAction(const char *name);

    const AIAction::List &GetActions() const {
        return mActions;
    }

    AIAction *mCurrentAction; // offset 0x4, size 0x4
  private:
    AIAction::List mActions; // offset 0x8, size 0x8
    ISimable *mOwner;        // offset 0x10, size 0x4
};

#endif
