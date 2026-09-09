#include "Speed/Indep/Src/AI/AIGoal.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

IMPLEMENT_FACTORY(AIGoal);

AIGoal::~AIGoal() {
    for (AIAction::List::const_iterator iter = this->mActions.begin(); iter != this->mActions.end(); ++iter) {
        delete *iter;
    }
    this->mActions.clear();

    this->mCurrentAction = nullptr;
}

void AIGoal::AddAction(const char *name) {
    AIActionParams params(this->mOwner);
    AIAction *action = AIAction::CreateInstance(UCrc32(name), &params);

    action->SetActionName(name);

    this->mActions.push_back(action);
}

void AIGoal::OnBehaviorChange(const UCrc32 &mechanic) {
    for (AIAction::List::const_iterator iter = this->mActions.begin(); iter != this->mActions.end(); ++iter) {
        (*iter)->OnBehaviorChange(mechanic);
    }
}

void AIGoal::ChooseAction(float dT) {
    bool currentDone = false;
    float currentScore = 0.0f;

    if (this->mCurrentAction != nullptr) {
        currentDone = this->mCurrentAction->IsFinished();
        currentScore = this->mCurrentAction->GetScore();
    }

    AIAction *new_action = nullptr;
    for (AIAction::List::const_iterator iter = this->mActions.begin(); iter != this->mActions.end(); ++iter) {
        AIAction *action = *iter;
        if (action != this->mCurrentAction && action->CanBeAttempted(dT)) {
            if (currentDone || action->GetScore() >= currentScore) {
                new_action = action;
                currentDone = false;
                currentScore = action->GetScore();
            }
        }
    }
    if (new_action != nullptr) {
        if (this->mCurrentAction != nullptr) {
            this->mCurrentAction->FinishAction(dT);
        }
        this->mCurrentAction = new_action;
        new_action->BeginAction(dT);
    }
}

void AIGoal::Update(float dT) {
    ProfileNode profile_node("TODO", 0);

    {
        ProfileNode profile_node("TODO2", 0);
        this->ChooseAction(dT);
    }
    if (this->mCurrentAction != nullptr) {
        ProfileNode profile_node("TODO3", 0);
        this->mCurrentAction->Update(dT);
    }
}

AIGoal::AIGoal(ISimable *isimable) {
    this->mOwner = isimable;
    this->mCurrentAction = nullptr;
}

// total size: 0x18
class AIGoalNone : public AIGoal {
  public:
    AIGoalNone(ISimable *isimable);
    ~AIGoalNone() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalNone(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalNone);

AIGoalNone::AIGoalNone(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionNone");
}

// total size: 0x18
class AIGoalTraffic : public AIGoal {
  public:
    AIGoalTraffic(ISimable *isimable);
    ~AIGoalTraffic() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalTraffic(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalTraffic);

AIGoalTraffic::AIGoalTraffic(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionTraffic");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPatrol : public AIGoal {
  public:
    AIGoalPatrol(ISimable *isimable);
    ~AIGoalPatrol() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalPatrol(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalPatrol);

AIGoalPatrol::AIGoalPatrol(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionTraffic");
    this->AddAction("AIActionTooDamaged");
    this->ChooseAction(0.0f);
}

// total size: 0x1C
class AIGoalPursuit : public AIGoal {
  public:
    AIGoalPursuit(ISimable *isimable);
    ~AIGoalPursuit() override;

    // Overrides: AIGoal
    void Update(float dT) override;

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalPursuit(isimable);
    }

  private:
    float mFwdCG; // offset 0x18, size 0x4
};

BIND_AIGOAL_FACTORY(AIGoalPursuit);

AIGoalPursuit::AIGoalPursuit(ISimable *isimable)
    : AIGoal(isimable), //
      mFwdCG(0.0f) {
    this->AddAction("AIActionPursuitOffRoad");
    this->AddAction("AIActionRace");

    if (!this->GetOwner()->IsPlayer()) {
        this->AddAction("AIActionTraffic");
    }

    this->AddAction("AIActionTooDamaged");
    this->AddAction("AIActionGetUnstuck");
    this->AddAction("AIActionAirborne");

    this->ChooseAction(0.0f);
}

void AIGoalPursuit::Update(float dT) {
    ProfileNode profile_node("TODO", 0);
    this->AIGoal::Update(dT);
}

AIGoalPursuit::~AIGoalPursuit() {}

// total size: 0x18
class AIGoalStopShort : public AIGoal {
  public:
    AIGoalStopShort(ISimable *isimable);
    ~AIGoalStopShort() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalStopShort(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalStopShort);

AIGoalStopShort::AIGoalStopShort(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionStopShort");
    this->AddAction("AIActionTooDamaged");
    this->AddAction("AIActionGetUnstuck");
    this->AddAction("AIActionAirborne");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalRam : public AIGoal {
  public:
    AIGoalRam(ISimable *isimable);
    ~AIGoalRam() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalRam(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalRam);

AIGoalRam::AIGoalRam(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionRam");
    this->AddAction("AIActionPursuitOffRoad");
    this->AddAction("AIActionRace");
    this->AddAction("AIActionTooDamaged");
    this->AddAction("AIActionGetUnstuck");
    this->AddAction("AIActionAirborne");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPit : public AIGoal {
  public:
    AIGoalPit(ISimable *isimable);
    ~AIGoalPit() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalPit(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalPit);

AIGoalPit::AIGoalPit(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionRam");
    this->AddAction("AIActionPursuitOffRoad");
    this->AddAction("AIActionRace");
    this->AddAction("AIActionTooDamaged");
    this->AddAction("AIActionGetUnstuck");
    this->AddAction("AIActionAirborne");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPullOver : public AIGoal {
  public:
    AIGoalPullOver(ISimable *isimable);
    ~AIGoalPullOver() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalRam(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalPullOver);

// STRIPPED
AIGoalPullOver::AIGoalPullOver(ISimable *isimable) : AIGoal(isimable) {}

// total size: 0x18
class AIGoalHeadOnRam : public AIGoal {
  public:
    AIGoalHeadOnRam(ISimable *isimable);
    ~AIGoalHeadOnRam() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalHeadOnRam(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalHeadOnRam);

AIGoalHeadOnRam::AIGoalHeadOnRam(ISimable *isimable) : AIGoal(isimable) {
    IPursuitAI *ipv;
    if (isimable->QueryInterface(&ipv)) {
        UMath::Vector3 off = UMath::Vector3Make(0.0f, 0.0f, -3.0f);
        ipv->SetInPositionOffset(off);
    }
    this->AddAction("AIActionHeadOnRam");
    this->AddAction("AIActionRace");
    this->AddAction("AIActionTooDamaged");
    this->AddAction("AIActionGetUnstuck");
    this->AddAction("AIActionAirborne");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalStaticRoadBlock : public AIGoal {
  public:
    AIGoalStaticRoadBlock(ISimable *isimable);
    ~AIGoalStaticRoadBlock() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalStaticRoadBlock(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalStaticRoadBlock);

static const float Tweak_RoadBlockFwdCG = 0.8f;   // Decl: 1178
static const float Tweak_RoadBlockMass = 4400.0f; // Decl: 1179
AIGoalStaticRoadBlock::AIGoalStaticRoadBlock(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionStaticRoadBlock");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalFleePursuit : public AIGoal {
  public:
    AIGoalFleePursuit(ISimable *isimable);
    ~AIGoalFleePursuit() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalFleePursuit(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalFleePursuit);

AIGoalFleePursuit::AIGoalFleePursuit(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionRace");
    this->AddAction("AIActionTooDamaged");
    this->AddAction("AIActionGetUnstuck");
    this->AddAction("AIActionAirborne");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalHeliPursuit : public AIGoal {
  public:
    AIGoalHeliPursuit(ISimable *isimable);
    ~AIGoalHeliPursuit() override {}

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalHeliPursuit(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalHeliPursuit);

AIGoalHeliPursuit::AIGoalHeliPursuit(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionHeliPursuit");
    this->AddAction("AIActionTooDamaged");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalHeliExit : public AIGoal {
  public:
    AIGoalHeliExit(ISimable *isimable);
    ~AIGoalHeliExit() override {}

    void Update(float dT) override;

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalHeliExit(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalHeliExit);

AIGoalHeliExit::AIGoalHeliExit(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionHeliExit");
    this->AddAction("AIActionTooDamaged");
    this->ChooseAction(0.0f);
}

void AIGoalHeliExit::Update(float dT) {
    this->AIGoal::Update(dT);
    if (!this->IsCurrentAction(UCrc32("AIActionHeliExit"))) {
        return;
    }
    if (this->mCurrentAction->IsFinished()) {
        IVehicleAI *iai;
        // no if check
        this->GetOwner()->QueryInterface(&iai);
        iai->UnSpawn();
    }
}

// total size: 0x18
class AIGoalRacer : public AIGoal {
  public:
    AIGoalRacer(ISimable *isimable);

    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalRacer(isimable);
    }
};

BIND_AIGOAL_FACTORY(AIGoalRacer);

AIGoalRacer::AIGoalRacer(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionRace");
    this->AddAction("AIActionGetUnstuck");
    this->ChooseAction(0.0f);
}
