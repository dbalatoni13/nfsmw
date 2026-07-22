#include "RBVehicle.h"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"

class RBRemote : public RBVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);

  protected:
    RBRemote(const BehaviorParams &bp, const RBComplexParams &params);
    ~RBRemote() override;
    bool CanCollideWith(const RigidBody &other) const override;
    virtual void SetInvulerability(float time, bool permanent);
};

Behavior *RBRemote::Construct(const BehaviorParams &params) {
    const RBComplexParams rp(params.fparams.Fetch<RBComplexParams>(UCrc32(0xa6b47fac)));
    return new RBRemote(params, rp);
}

RBRemote::RBRemote(const BehaviorParams &bp, const RBComplexParams &params)
    : RBVehicle(bp, params) {}

RBRemote::~RBRemote() {}

bool RBRemote::CanCollideWith(const RigidBody &other) const {
    ISimable *othersimable = other.GetOwner();
    if (othersimable->GetSimableType() == SIMABLE_VEHICLE) {
        if (TheRaceParameters.nMaxCops > 0) {
            return true;
        }
        if (FEDatabase->OnlineSettings.CollisionDetection) {
            GRacerInfo *thisinfo = GRaceStatus::Get().GetRacerInfo(GetOwner());
            if (thisinfo) {
                GRacerInfo *otherinfo = GRaceStatus::Get().GetRacerInfo(othersimable);
                if (otherinfo &&
                    thisinfo->GetLapsCompleted() != otherinfo->GetLapsCompleted()) {
                    return false;
                }
            }

            IHumanAI *humanai;
            if (GetOwner()->QueryInterface(&humanai) && humanai->IsFacingWrongWay()) {
                return false;
            }
            if (othersimable->QueryInterface(&humanai) && humanai->IsFacingWrongWay()) {
                return false;
            }
        } else {
            return false;
        }
    }
    return RBVehicle::CanCollideWith(other);
}

void RBRemote::SetInvulerability(float time, bool permanent) {}
