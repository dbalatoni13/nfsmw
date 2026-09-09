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
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifndef __AIGOAL_H
#define __AIGOAL_H 1 // Decl: 13

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// total size: 0x18
// Decl: 27
class AIGoal : public UTL::COM::Factory<ISimable *, AIGoal, UCrc32> {
  public:
    USE_FASTALLOC(AIGoal);

    virtual ~AIGoal();
    virtual void ChooseAction(float dT);
    virtual void Update(float dT);
    virtual void OnBehaviorChange(const UCrc32 &mechanic);

    UCrc32 GetActionName() {
        if (this->mCurrentAction != nullptr) {
            return this->mCurrentAction->GetActionName();
        } else {
            return UCrc32(static_cast<const char *>(nullptr));
        }
    }

    bool IsCurrentAction(const UCrc32 name) {
        if (this->mCurrentAction != nullptr) {
            return name == this->mCurrentAction->GetActionName();
        } else {
            return false;
        }
    }

  protected:
    AIGoal(ISimable *isimable);

    ISimable *GetOwner() const {
        return this->mOwner;
    }

    void AddAction(const char *name);

    const AIAction::List &GetActions() const {
        return this->mActions;
    }

    AIAction *mCurrentAction; // offset 0x4, size 0x4, Decl: 54
  private:
    AIAction::List mActions; // offset 0x8, size 0x8, Decl: 57

    ISimable *mOwner; // offset 0x10, size 0x4, Decl: 59
};

#define BIND_AIGOAL_FACTORY(_TYPE_) AIGoal::Prototype _##_TYPE_(UCrc32(#_TYPE_), _TYPE_::Construct); // Decl: 65

#endif
