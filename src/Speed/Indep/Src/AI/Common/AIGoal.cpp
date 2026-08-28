#include "Speed/Indep/Src/AI/AIGoal.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

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

// TODO move these to the header?

// total size: 0x18
class AIGoalNone : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalNone(isimable);
    }

    AIGoalNone(ISimable *isimable);

    ~AIGoalNone() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalNone(UCrc32("AIGoalNone"), AIGoalNone::Construct);

AIGoalNone::AIGoalNone(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionNone");
}

// total size: 0x18
class AIGoalTraffic : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalTraffic(isimable);
    }

    AIGoalTraffic(ISimable *isimable);

    ~AIGoalTraffic() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalTraffic(UCrc32("AIGoalTraffic"), AIGoalTraffic::Construct);

AIGoalTraffic::AIGoalTraffic(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionTraffic");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalPatrol : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalPatrol(isimable);
    }

    AIGoalPatrol(ISimable *isimable);

    ~AIGoalPatrol() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalPatrol(UCrc32("AIGoalPatrol"), AIGoalPatrol::Construct);

AIGoalPatrol::AIGoalPatrol(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionTraffic");
    this->AddAction("AIActionTooDamaged");
    this->ChooseAction(0.0f);
}

// total size: 0x1C
class AIGoalPursuit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalPursuit(isimable);
    }

    AIGoalPursuit(ISimable *isimable);

    // Overrides: AIGoal
    void Update(float dT) override;

    // Overrides: AIGoal
    ~AIGoalPursuit() override;

  private:
    float mFwdCG; // offset 0x18, size 0x4
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalPursuit(UCrc32("AIGoalPursuit"), AIGoalPursuit::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalStopShort(isimable);
    }

    AIGoalStopShort(ISimable *isimable);

    ~AIGoalStopShort() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalStopShort(UCrc32("AIGoalStopShort"), AIGoalStopShort::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalRam(isimable);
    }

    AIGoalRam(ISimable *isimable);

    ~AIGoalRam() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalRam(UCrc32("AIGoalRam"), AIGoalRam::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalPit(isimable);
    }

    AIGoalPit(ISimable *isimable);

    ~AIGoalPit() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalPit(UCrc32("AIGoalPit"), AIGoalPit::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalRam(isimable);
    }

    AIGoalPullOver(ISimable *isimable);

    ~AIGoalPullOver() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalPullOver(UCrc32("AIGoalPullOver"), AIGoalPullOver::Construct);

// STRIPPED
AIGoalPullOver::AIGoalPullOver(ISimable *isimable) : AIGoal(isimable) {}

// total size: 0x18
class AIGoalHeadOnRam : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalHeadOnRam(isimable);
    }

    AIGoalHeadOnRam(ISimable *isimable);

    ~AIGoalHeadOnRam() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalHeadOnRam(UCrc32("AIGoalHeadOnRam"), AIGoalHeadOnRam::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalStaticRoadBlock(isimable);
    }

    AIGoalStaticRoadBlock(ISimable *isimable);

    ~AIGoalStaticRoadBlock() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalStaticRoadBlock(UCrc32("AIGoalStaticRoadBlock"), AIGoalStaticRoadBlock::Construct);

AIGoalStaticRoadBlock::AIGoalStaticRoadBlock(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionStaticRoadBlock");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalFleePursuit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalFleePursuit(isimable);
    }

    AIGoalFleePursuit(ISimable *isimable);

    ~AIGoalFleePursuit() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalFleePursuit(UCrc32("AIGoalFleePursuit"), AIGoalFleePursuit::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalHeliPursuit(isimable);
    }

    AIGoalHeliPursuit(ISimable *isimable);

    ~AIGoalHeliPursuit() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalHeliPursuit(UCrc32("AIGoalHeliPursuit"), AIGoalHeliPursuit::Construct);

AIGoalHeliPursuit::AIGoalHeliPursuit(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionHeliPursuit");
    this->AddAction("AIActionTooDamaged");
    this->ChooseAction(0.0f);
}

// total size: 0x18
class AIGoalHeliExit : public AIGoal {
  public:
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalHeliExit(isimable);
    }

    AIGoalHeliExit(ISimable *isimable);
    void Update(float dT) override;

    ~AIGoalHeliExit() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalHeliExit(UCrc32("AIGoalHeliExit"), AIGoalHeliExit::Construct);

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
    static AIGoal *Construct(ISimable *isimable) {
        return new AIGoalRacer(isimable);
    }

    AIGoalRacer(ISimable *isimable);
    void Update(float dT) override;

    ~AIGoalRacer() override {}
};

UTL::COM::Factory<ISimable *, AIGoal, UCrc32>::Prototype _AIGoalRacer(UCrc32("AIGoalRacer"), AIGoalRacer::Construct);

AIGoalRacer::AIGoalRacer(ISimable *isimable) : AIGoal(isimable) {
    this->AddAction("AIActionRace");
    this->AddAction("AIActionGetUnstuck");
    this->ChooseAction(0.0f);
}
