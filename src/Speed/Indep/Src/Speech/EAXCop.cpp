#include "EAXCop.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"

EAXCop::EAXCop(int speakerID, HSIMABLE handle, int bID, int cID)
    : EAXCharacter(speakerID, handle, bID, cID) {
    ISimable *simable;

    mPctTractiveTires = 0.0f;
    mRank = 0;
    *reinterpret_cast<unsigned int *>(&mInFormation) = 0;
    *reinterpret_cast<unsigned int *>(&mInPosition) = 0;
    *reinterpret_cast<unsigned int *>(&mAhead) = 0;
    mTimeAirborne = WorldTimer;
    mTrafficHitCount = 0;
    mTimeNoLOS = WorldTimer;
    mT_closingDist = 0;
    mCurrRoad = MAX_ROADNAMES;
    mTgtOffset.x = UMath::Vector3::kZero.x;
    mNumRammed = 0;
    mLastRammedTime = 0;
    mT_lastactivity = 0;
    mOrigin = MAX_ROADNAMES;
    mTgtOffset.z = UMath::Vector3::kZero.z;
    mTgtOffset.y = UMath::Vector3::kZero.y;
    simable = ISimable::FindInstance(handle);
    if (simable) {
        IVehicleAI *vai;
        if (simable->QueryInterface(&vai) && vai) {
            WRoadNav *nav = vai->GetDriveToNav();
            if (nav) {
                unsigned int road_id = nav->GetRoadSpeechId();
                if (road_id != 0x6B) {
                    mOrigin = static_cast<RoadNames>(road_id);
                }
            }
        }
    }
}

EAXCop::~EAXCop() {}

int EAXCop::GetBackupTypeFromDispatch(int type) {
    return type;
}

void EAXCop::Update() {
    EAXCharacter::Update();
}

void EAXCop::SetActive(bool activity) {
    EAXCharacter::SetActive(activity);
}

void EAXCop::Reset() {
    EAXCharacter::Reset();
    mInFormation = false;
    mInPosition = false;
    mAhead = false;
    mPctTractiveTires = 0.0f;
    mTrafficHitCount = 0;
    mNumRammed = 0;
}

void EAXCop::AttemptVehicleStop() {}

void EAXCop::VehicleReport() {}

void EAXCop::InitiatePursuit() {}

void EAXCop::LocationReport() {}

void EAXCop::SelfStrategy(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::InitialCallForBackup() {}

void EAXCop::CallForBackup(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::UnitBackupReply() {}

void EAXCop::InitiateStrategy(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::CallToPosition(EAXCop *cop) {
    if (cop) {
        mTgtOffset = cop->mTgtOffset;
    }
}

void EAXCop::CallToPositionReminder() {}

void EAXCop::StrategyExecute() {}

void EAXCop::Collision(int collisionType, float force, EAXCop *spkr) {
    if (force > 0.0f || spkr) {
        mLastEvent = collisionType;
    }
}

void EAXCop::AnticipateSuccess() {}

void EAXCop::AnticipateFail() {}

void EAXCop::OutcomeFail(short intensity) {
    if (intensity) {
        mLastEvent = intensity;
    }
}

void EAXCop::StrategyReset(bool new_strategy) {
    if (new_strategy) {
        mLastEvent = 0;
    }
}

void EAXCop::SuspectBehavior() {}

void EAXCop::SuspectOutrun() {}

void EAXCop::SuspectUTurn() {}

void EAXCop::LostSuspect() {}

void EAXCop::LostVisual() {}

void EAXCop::RegainVisual() {}

void EAXCop::SwarmingReply() {}

void EAXCop::Arrest() {}

void EAXCop::PursuitUpdateReply() {}

void EAXCop::ReinitiatePursuit() {}

void EAXCop::NegativeBackupReply() {}

void EAXCop::BackupReminder(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::BackupArrives() {}

void EAXCop::IntentToRam() {}

void EAXCop::CallforEV(unsigned int type) {
    if (type) {
        mLastEvent = static_cast<int>(type);
    }
}

void EAXCop::UnitDisabled(int other) {
    mLastEvent = other;
}

void EAXCop::Bailout() {}

void EAXCop::LoBailout() {}

void EAXCop::HiBailout() {}

void EAXCop::BailoutTraffic() {}

void EAXCop::BailoutBadRoad() {}

void EAXCop::Spotter() {}

void EAXCop::SpotterReply() {}

void EAXCop::Reply911() {}

void EAXCop::DenyBailout() {}

void EAXCop::PrimaryEngage() {}

void EAXCop::Bullhorn() {}

void EAXCop::PreBullhorn() {}

void EAXCop::BullhornArrest() {}

void EAXCop::SuspectConfirmed() {}

void EAXCop::FocusChange() {}

void EAXCop::Spotted() {}

void EAXCop::DirectionChange() {}

void EAXCop::CallForSwarming() {}

void EAXCop::SpotterWanted() {}

void EAXCop::Offroad(unsigned int id, bool subsequent) {
    if (subsequent) {
        mLastEvent = static_cast<int>(id);
    }
}

void EAXCop::WeatherReport() {}

void EAXCop::CallForRB() {}

void EAXCop::RBReminder() {}

void EAXCop::NegRBReply() {}

void EAXCop::RBApproach() {}

void EAXCop::RBEngage(bool spikes_hit) {
    if (spikes_hit) {
        mLastEvent = 1;
    }
}

void EAXCop::PursuitApproaching() {}

void EAXCop::RBAverted() {}

void EAXCop::CallForSubRB() {}

void EAXCop::RearEnded(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::HeadOn(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SideSwiped(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::TBoned(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectRollover(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectAirborne(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectSpunout(Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectBrake() {}

void EAXCop::SwapVoices(EAXCop *cop) {
    if (cop) {
        int callsign = mCallsign.name;
        int unit = mCallsign.number;
        mCallsign.name = cop->mCallsign.name;
        mCallsign.number = cop->mCallsign.number;
        cop->mCallsign.name = callsign;
        cop->mCallsign.number = unit;
    }
}

bool EAXCop::IsPrimary() {
    return mRank == 0;
}

bool EAXCop::SetRank(int newrank) {
    mRank = newrank;
    return true;
}

void EAXCop::Impact_Suspect_World() {}

void EAXCop::Impact_Suspect_Semi() {}

void EAXCop::Impact_Suspect_Train() {}

void EAXCop::Impact_Suspect_Guardrail() {}

void EAXCop::Impact_Suspect_GasStation() {}

void EAXCop::Impact_Suspect_Spikebelt() {}

void EAXCop::Impact_Suspect_Traffic(Type_intensity intensity) {
    mLastEvent = intensity;
}
