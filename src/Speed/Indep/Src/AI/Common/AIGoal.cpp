#include "Speed/Indep/Src/AI/AIGoal.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

AIGoal::~AIGoal() {
    for (AIAction::List::const_iterator iter = mActions.begin(); iter != mActions.end(); ++iter) {
        delete *iter;
    }
    mActions.clear();

    mCurrentAction = nullptr;
}

void AIGoal::AddAction(const char *name) {
    AIActionParams params(mOwner);
    AIAction *action = UTL::COM::Factory<AIActionParams *, AIAction, UCrc32>::CreateInstance(name, &params);

    action->SetActionName(name);

    mActions.push_back(action);
}

void AIGoal::OnBehaviorChange(const UCrc32 &mechanic) {
    for (AIAction::List::const_iterator iter = mActions.begin(); iter != mActions.end(); ++iter) {
        (*iter)->OnBehaviorChange(mechanic);
    }
}

void AIGoal::ChooseAction(float dT) {
    bool currentDone = false;
    float currentScore = 0.0f;

    if (mCurrentAction) {
        currentDone = mCurrentAction->IsFinished();
        currentScore = mCurrentAction->GetScore();
    }

    AIAction *new_action = nullptr;
    for (AIAction::List::const_iterator iter = mActions.begin(); iter != mActions.end(); ++iter) {
        AIAction *action = *iter;
        if (action != mCurrentAction && action->CanBeAttempted(dT)) {
            if (currentDone || action->GetScore() >= currentScore) {
                new_action = action;
                currentDone = false;
                currentScore = action->GetScore();
            }
        }
    }
    if (new_action) {
        if (mCurrentAction) {
            mCurrentAction->FinishAction(dT);
        }
        mCurrentAction = new_action;
        new_action->BeginAction(dT);
    }
}

void AIGoal::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    {
        ProfileNode profile_node("TODO2", 0);
        ChooseAction(dT);
    }
    if (mCurrentAction) {
        ProfileNode profile_node("TODO3", 0);
        mCurrentAction->Update(dT);
    }
}

AIGoal::AIGoal(ISimable *isimable) {
    mOwner = isimable;
    mCurrentAction = nullptr;
}

// TODO move these to the header?

// total size: 0x18
class AIGoalNone : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalNone(ISimable *isimable);

    ~AIGoalNone() override {}
};

AIGoalNone::AIGoalNone(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionNone");
}

// total size: 0x18
class AIGoalTraffic : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalTraffic(ISimable *isimable);

    ~AIGoalTraffic() override {}
};

AIGoalTraffic::AIGoalTraffic(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionTraffic");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPatrol : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalPatrol(ISimable *isimable);

    ~AIGoalPatrol() override {}
};

AIGoalPatrol::AIGoalPatrol(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionTraffic");
    AddAction("AIActionTooDamaged");
    ChooseAction(0.0f);
}

// total size: 0x1C
class AIGoalPursuit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalPursuit(ISimable *isimable);

    // Overrides: AIGoal
    void Update(float dT) override;

    // Overrides: AIGoal
    ~AIGoalPursuit() override;

  private:
    float mFwdCG; // offset 0x18, size 0x4
};

AIGoalPursuit::AIGoalPursuit(ISimable *isimable)
    : AIGoal(isimable), //
      mFwdCG(0.0f) {
    AddAction("AIActionPursuitOffRoad");
    AddAction("AIActionRace");

    if (!GetOwner()->IsPlayer()) {
        AddAction("AIActionTraffic");
    }

    AddAction("AIActionTooDamaged");
    AddAction("AIActionGetUnstuck");
    AddAction("AIActionAirborne");

    ChooseAction(0.0f);
}

void AIGoalPursuit::Update(float dT) {
    ProfileNode profile_node("TODO", 0);
    AIGoal::Update(dT);
}

AIGoalPursuit::~AIGoalPursuit() {}

// total size: 0x18
class AIGoalStopShort : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalStopShort(ISimable *isimable);

    ~AIGoalStopShort() override {}
};

AIGoalStopShort::AIGoalStopShort(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionStopShort");
    AddAction("AIActionTooDamaged");
    AddAction("AIActionGetUnstuck");
    AddAction("AIActionAirborne");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalRam : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalRam(ISimable *isimable);

    ~AIGoalRam() override {}
};

AIGoalRam::AIGoalRam(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionRam");
    AddAction("AIActionPursuitOffRoad");
    AddAction("AIActionRace");
    AddAction("AIActionTooDamaged");
    AddAction("AIActionGetUnstuck");
    AddAction("AIActionAirborne");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalPit(ISimable *isimable);

    ~AIGoalPit() override {}
};

AIGoalPit::AIGoalPit(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionRam");
    AddAction("AIActionPursuitOffRoad");
    AddAction("AIActionRace");
    AddAction("AIActionTooDamaged");
    AddAction("AIActionGetUnstuck");
    AddAction("AIActionAirborne");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPullOver : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalPullOver(ISimable *isimable);

    ~AIGoalPullOver() override {}
};

// STRIPPED
AIGoalPullOver::AIGoalPullOver(ISimable *isimable) : AIGoal(isimable) {}

// total size: 0x18
class AIGoalHeadOnRam : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalHeadOnRam(ISimable *isimable);

    ~AIGoalHeadOnRam() override {}
};

AIGoalHeadOnRam::AIGoalHeadOnRam(ISimable *isimable) : AIGoal(isimable) {
    IPursuitAI *ipv;
    if (isimable->QueryInterface(&ipv)) {
        UMath::Vector3 off = UMath::Vector3Make(0.0f, 0.0f, -3.0f);
        ipv->SetInPositionOffset(off);
    }
    AddAction("AIActionHeadOnRam");
    AddAction("AIActionRace");
    AddAction("AIActionTooDamaged");
    AddAction("AIActionGetUnstuck");
    AddAction("AIActionAirborne");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalStaticRoadBlock : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalStaticRoadBlock(ISimable *isimable);

    ~AIGoalStaticRoadBlock() override {}
};

AIGoalStaticRoadBlock::AIGoalStaticRoadBlock(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionStaticRoadBlock");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalFleePursuit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalFleePursuit(ISimable *isimable);

    ~AIGoalFleePursuit() override {}
};

AIGoalFleePursuit::AIGoalFleePursuit(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionRace");
    AddAction("AIActionTooDamaged");
    AddAction("AIActionGetUnstuck");
    AddAction("AIActionAirborne");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalHeliPursuit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalHeliPursuit(ISimable *isimable);

    ~AIGoalHeliPursuit() override {}
};

AIGoalHeliPursuit::AIGoalHeliPursuit(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionHeliPursuit");
    AddAction("AIActionTooDamaged");
    ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalHeliExit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalHeliExit(ISimable *isimable);
    void Update(float dT) override;

    ~AIGoalHeliExit() override {}
};

AIGoalHeliExit::AIGoalHeliExit(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionHeliExit");
    AddAction("AIActionTooDamaged");
    ChooseAction(0.0f);
}

void AIGoalHeliExit::Update(float dT) {
    AIGoal::Update(dT);
    if (!IsCurrentAction("AIActionHeliExit")) {
        return;
    }
    if (mCurrentAction->IsFinished()) {
        IVehicleAI *iai;
        // no if check
        GetOwner()->QueryInterface(&iai);
        iai->UnSpawn();
    }
}

// total size: 0x18
class AIGoalRacer : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {}

    AIGoalRacer(ISimable *isimable);
    void Update(float dT) override;

    ~AIGoalRacer() override {}
};

AIGoalRacer::AIGoalRacer(ISimable *isimable) : AIGoal(isimable) {
    AddAction("AIActionRace");
    AddAction("AIActionGetUnstuck");
    ChooseAction(0.0f);
}
