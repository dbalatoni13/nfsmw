#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

PursuitFormation::PursuitFormation()
    : mMinFinisherCops(1), //
      mMaxCops(0),         //
      mHasFinisher(false) {
    Reset();
}

PursuitFormation::~PursuitFormation() {
    Reset();
}

void PursuitFormation::Reset() {
    mTargetOffsets.clear();
}

void PursuitFormation::AddTargetOffset(const UMath::Vector3 &targetOffset, int minTargets, UCrc32 ipg, const UMath::Vector3 &inPositionOffset) {
    mTargetOffsets.push_back(TargetOffset(targetOffset, inPositionOffset, minTargets, ipg));
}

BoxInFormation::BoxInFormation(int copcount, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    if (pursuit->GetTarget()->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib) {
        tightness = pursuitLevelAttrib->BoxinTightness();
        finishertime = pursuitLevelAttrib->BoxinDuration();
    } else {
        tightness = 0.5f;
        finishertime = 2.0f;
    }

    UMath::Vector3 pos;
    UMath::Vector3 fpos;
    float foff = 2.0f - (tightness * 5.0f);
    float fscale = 0.7f - (tightness * 0.5f);

    getPosition(0, 1.0f, pos);
    getPosition(3, fscale, fpos);
    fpos.z = foff;
    AddTargetOffset(pos, 1, "AIGoalRam", fpos);

    getPosition(1, 1.0f, pos);
    getPosition(1, fscale, fpos);
    AddTargetOffset(pos, 2, "AIGoalRam", fpos);

    getPosition(2, 1.0f, pos);
    getPosition(2, fscale, fpos);
    AddTargetOffset(pos, 2, "AIGoalRam", fpos);

    getPosition(3, 1.0f, pos);
    getPosition(3, fscale, fpos);
    AddTargetOffset(pos, 4, "AIGoalRam", fpos);

    SetMaxCops(4);
    SetMinFinisherCops(2);
    SetHasFinisher(true);
}

void BoxInFormation::getPosition(int idx, float scale, UMath::Vector3 &pos) {
    struct vec3 {
        UMath::Vector3 v;
    };
    static const vec3 base_pos[4] = {{0.0f, 0.0f, 14.0f}, {-3.5f, 0.0f, 0.0f}, {3.5f, 0.0f, 0.0f}, {0.0f, 0.0f, -7.5f}};

    UMath::Scale(base_pos[idx].v, scale, pos);
}

// Functionally matching
void BoxInFormation::Update(float dT, IPursuit *pursuit) {
    float finisher = pursuit->TimeToFinisherAttempt();
    float ftight = (tightness * 0.2f) + 0.2f;
    float scale = (finisher / GetTimeToFinisher() * ftight) + (1.0f - ftight);

    for (int i = 0; i < 4; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

RollingBlockFormation::RollingBlockFormation(int numCops, struct IPursuit *pursuit) {
    IPerpetrator *iperp;
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr; // r29
    if (pursuit->GetTarget()->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib) {
        tightness = pursuitLevelAttrib->RollingBlockTightness();
        finishertime = pursuitLevelAttrib->RollingBlockDuration();
    } else {
        tightness = 0.5f;
        finishertime = 2.0f;
    }

    float fscale = 1.0f - (tightness * 0.8f);
    float foff = 2.0f - (tightness * 5.0f);
    static const int priority[5] = {1, 2, 2, 3, 3};

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        UMath::Vector3 fpos;

        getPosition(i, 1.0f, pos);
        getPosition(i, fscale, fpos);
        fpos.z = foff;
        AddTargetOffset(pos, priority[i], "AIGoalRam", fpos);
    }

    SetMaxCops(4);
    SetMinFinisherCops(2);
    SetHasFinisher(true);
}

void RollingBlockFormation::getPosition(int idx, float scale, UMath::Vector3 &pos) {
    struct vec3 {
        UMath::Vector3 v;
    };
    static const vec3 base_pos[5] = {{0.0f, 0.0f, 14.0f}, {-2.5f, 0.0f, 14.0f}, {2.5f, 0.0f, 10.0f}, {-5.0f, 0.0f, 14.0f}, {5.0f, 0.0f, 14.0f}};

    UMath::Scale(base_pos[idx].v, scale, pos);
}

// Functionally matching
void RollingBlockFormation::Update(float dT, IPursuit *pursuit) {
    float finisher = pursuit->TimeToFinisherAttempt();
    float ftight = tightness * 0.4f;
    float scale = (finisher / GetTimeToFinisher() * ftight) + (1.0f - ftight);

    for (int i = 0; i < 5; i++) {
        UMath::Vector3 pos;
        getPosition(i, scale, pos);
        mTargetOffsets[i].mOffset = pos;
    }
}

FollowFormation::FollowFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -17.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -17.0f);
    AddTargetOffset(stupid_hack, 4, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -17.0f);
    AddTargetOffset(stupid_hack, 4, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    SetMaxCops(6);
    SetHasFinisher(false);
}

// total size: 0x20
class StaggerFollowFormation : public PursuitFormation {
  public:
    StaggerFollowFormation(int copcount);

    // Overrides: PursuitFormation
    inline ~StaggerFollowFormation() override {}
};

StaggerFollowFormation::StaggerFollowFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(0.0f, 0.0f, 13.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, 13.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.5f, 0.0f, -13.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(3.5f, 0.0f, 13.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    SetMaxCops(6);
    SetHasFinisher(false);
}

PitFormation::PitFormation(int copcount) {
    UMath::Vector3 stupid_hack;
    UMath::Vector3 stupid_hack1;

    stupid_hack = UMath::Vector3Make(4.0f, 0.0f, -2.7f);
    stupid_hack1 = UMath::Vector3Make(-10.0f, 0.0f, -2.7f);
    AddTargetOffset(stupid_hack, 1, "AIGoalPit", stupid_hack1);

    stupid_hack = UMath::Vector3Make(-4.0f, 0.0f, -2.7f);
    stupid_hack1 = UMath::Vector3Make(10.0f, 0.0f, -2.7f);
    AddTargetOffset(stupid_hack, 1, "AIGoalPit", stupid_hack1);

    SetMaxCops(1);
    SetHasFinisher(true);
}

HerdFormation::HerdFormation(int copcount) {
    UMath::Vector3 stupid_hack;

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, 0.0f);
    AddTargetOffset(stupid_hack, 1, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, 5.0f);
    AddTargetOffset(stupid_hack, 2, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    stupid_hack = UMath::Vector3Make(-3.0f, 0.0f, -5.0f);
    AddTargetOffset(stupid_hack, 3, UCrc32::kNull, UMath::Vector3Make(0.0f, 0.0f, 0.0f));

    SetMaxCops(3);
    SetHasFinisher(false);
}

// void HerdFormation::Update(float dT, IPursuit *pursuit) {}

void GroundSupportRequest::Reset() {
    bool bAddToContingent = true;
    if (mSupportRequestStatus == ACTIVE && mHeavySupport && mHeavySupport->HeavyStrategy == HEAVY_ROADBLOCK) {
        bAddToContingent = false;
    }
    mSupportRequestStatus = NOT_ACTIVE;
    mHeavySupport = nullptr;
    mLeaderSupport = nullptr;
    mSupportTimer = 0.0f;

    if (bAddToContingent) {
        for (IVehicle::List::iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
            IVehicle *iv = *iter;
            IPursuitAI *ipv;
            if (iv->QueryInterface(&ipv)) {
                ipv->SetSupportGoal((const char *)nullptr);
                if (iv->IsActive()) {
                    IVehicleAI *ivai;
                    // unchecked
                    ipv->QueryInterface(&ivai);
                    IPursuit *ip = ivai->GetPursuit();
                    if (ip) {
                        ip->AddVehicleToContingent(iv);
                    }
                }
            }
        }
    }
    mIVehicleList.clear();
}

void GroundSupportRequest::Update(float dT) {
    if (mSupportRequestStatus != NOT_ACTIVE) {
        mSupportTimer -= dT;
        if (mSupportTimer < 0.0f) {
            Reset();
        }
    }
}

AIPursuit::~AIPursuit() {}
