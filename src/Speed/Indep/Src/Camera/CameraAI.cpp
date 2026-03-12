#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MICECameraFinished.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

#include <algorithm>
#include <list>

#include <types.h>

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"

extern Avoidables *TheAvoidables;
extern bool gGameBreakerCamera;

void SetNewSndCamAction(Attrib::StringKey mode, EVIEW_ID viewID);

static float kJumpTimeMultiplier = 2.0f;
static const float kEndJumpThreshold = 0.0f;
static const float kEndJumpValue = -1.0f;
static const float kEndPursuitThreshold = 0.0f;
static const float kEndPursuitValue = -1.0f;
static const float kJumpSpeedHigh = 100.0f;
static const float kJumpSpeedLow = 80.0f;
static const float kJumpDuration = 5.0f;

static const float Tweak_JumpCamHighestAirTresh[2] = {2.5f, 1.8f};
static const float Tweak_JumpCamLongestAirTresh[2] = {25.0f, 15.0f};

// --- Director methods ---

CameraAI::Director::Director(EVIEW_ID viewID)
    : UTL::Collections::Listable<Director, 2>(), //
      mViewID(viewID),                  //
      mDesiredMode(""),                 //
      mAction(nullptr),                 //
      mInputQ(1, 0x98c7a2f5, "CAMERA", false), //
      mPrepareToEnableIce(false),       //
      mPursuitStartTime(0.0f),          //
      mJumpTime(0.0f),                  //
      mIsCinematicMomement(false),      //
      mCinematicSlowdownSeconds(0.0f) {
}

CameraAI::Director::~Director() {
    ReleaseAction();
}

void CameraAI::Director::ReleaseAction() {
    if (mAction != nullptr) {
        delete mAction;
        mAction = nullptr;
    }
}

void CameraAI::Director::Reset() {
    mIsCinematicMomement = false;
    mJumpTime = 0.0f;
    mPursuitStartTime = 0.0f;
    mCinematicSlowdownSeconds = 0.0f;
    SetAction(Attrib::StringKey("DRIVE"));
    if (mAction != nullptr) {
        mAction->Reset();
    }
}

void CameraAI::Director::JumpStart(float time) {
    mJumpTime = time * kJumpTimeMultiplier;
}

void CameraAI::Director::EndJumping() {
    if (mJumpTime >= kEndJumpThreshold) {
        return;
    }
    mJumpTime = kEndJumpValue;
}

void CameraAI::Director::EndPursuitStart() {
    if (mPursuitStartTime >= kEndPursuitThreshold) {
        return;
    }
    mPursuitStartTime = kEndPursuitValue;
}

CameraMover *CameraAI::Director::GetMover() {
    if (mAction != nullptr) {
        return mAction->GetMover();
    }
    return nullptr;
}

void CameraAI::Director::Update(float dT) {
    if (!TheGameFlowManager.IsPaused() && mJumpTime > 0.0f) {
        mJumpTime -= dT;
    }
    if (!FEManager::ShouldPauseSimulation(true) && mPursuitStartTime > 0.0f) {
        mPursuitStartTime -= dT;
    }
    SelectAction();
    if (mAction != nullptr) {
        if (mAction->GetName() == Attrib::StringKey("DRIVE")) {
            mAction->SetSpecial(mCinematicSlowdownSeconds);
        }
        if (mAction != nullptr) {
            mAction->Update(dT);
        }
    }
}

void CameraAI::Director::SetAction(Attrib::StringKey desiredMode) {
    Action *action;
    mDesiredMode = desiredMode;
    if (mAction != nullptr) {
        const Attrib::StringKey &key = mAction->GetNext();
        if (!key.IsEmpty()) {
            mDesiredMode = key;
        }
        if (mAction != nullptr) {
            if (mAction->GetName() == mDesiredMode) {
                return;
            }
        }
    }
    if (!mDesiredMode.IsEmpty()) {
        action = Action::CreateInstance(UCrc32(mDesiredMode), this);
        if (action != nullptr) {
            ReleaseAction();
            mAction = action;
            SetNewSndCamAction(mDesiredMode, mViewID);
        }
    }
}

void CameraAI::Director::SelectAction() {
    if (TheICEManager.IsEditorOff()) {

        if (mJumpTime < 0.0f) {
            mJumpTime = 0.0f;
            mDesiredMode = Attrib::StringKey("DRIVE");
            mIsCinematicMomement = true;
        }

        if (mPursuitStartTime < 0.0f) {
            mPursuitStartTime = 0.0f;
            mDesiredMode = Attrib::StringKey("DRIVE");
            mIsCinematicMomement = true;
        }

        if (mPursuitStartTime > 0.0f && mPursuitStartTime < kJumpDuration) {
            mDesiredMode = Attrib::StringKey("TRACK_COP");
        }

        if (!gGameBreakerCamera) {
            eView *view = &eViews[mViewID];
            if (view != nullptr) {
                CameraMover *cm = view->GetCameraMover();
                if (cm != nullptr && cm->GetType() == CM_DRIVE_CUBIC) {
                    CameraAnchor *anchor = cm->GetAnchor();
                    if (anchor != nullptr) {
                        if (AreMomentCamerasEnabled() &&
                            (anchor->GetVelocityMagnitude() > kJumpSpeedHigh ||
                             (anchor->GetVelocityMagnitude() > kJumpSpeedLow && anchor->IsTouchingGround())) &&
                            anchor->IsCloseToRoadBlock()) {
                            mDesiredMode = Attrib::StringKey("JUMP");
                            mJumpTime = kJumpDuration;
                            {
                                MGamePlayMoment msg(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x2d8acb81);
                                msg.Send(UCrc32("MomentStrm"));
                            }
                        }
                    }
                }
            }
        }

        bool isICEPlaying = false;
        INIS *nis = UTL::Collections::Singleton<INIS>::Get();
        if (nis != nullptr) {
            if (nis->IsPlaying()) {
                ICEScene *scene = nis->GetScene();
                if (scene != nullptr) {
                    isICEPlaying = scene->IsControllingCamera();
                    mIsCinematicMomement = nis->IsWorldMomement();
                }
            }
        }

        if (isICEPlaying || TheICEManager.IsGenericCameraPlaying()) {
            mDesiredMode = Attrib::StringKey("ICE");
            mJumpTime = 0.0f;
            mPursuitStartTime = 0.0f;
        } else {
            if (mAction != nullptr && mAction->GetName() == Attrib::StringKey("ICE")) {
                TheICEManager.SetUseRealTime(false);
                mDesiredMode = Attrib::StringKey("DRIVE");
                INIS *nis2 = UTL::Collections::Singleton<INIS>::Get();
                if (nis2 != nullptr) {
                    nis2->FireEventTag("CameraFinished");
                }
                MICECameraFinished().Post(UCrc32(0x20d60dbf));
            }
        }
    }

    if (mAction != nullptr) {
        const Attrib::StringKey &key = mAction->GetNext();
        if (!key.IsEmpty()) {
            mDesiredMode = key;
        }
        if (mAction != nullptr) {
            if (mAction->GetName() == mDesiredMode) {
                return;
            }
        }
    }
    if (!mDesiredMode.IsEmpty()) {
        Action *action = Action::CreateInstance(UCrc32(mDesiredMode), this);
        if (action != nullptr) {
            mIsCinematicMomement = false;
            mCinematicSlowdownSeconds = 0.0f;
            ReleaseAction();
            mAction = action;
            SetNewSndCamAction(mDesiredMode, mViewID);
        }
    }
}

void CameraAI::Director::TotaledStart() {
    mDesiredMode = Attrib::StringKey("TOTALED");
    mJumpTime = 0.0f;
    SetAction(mDesiredMode);
}

void CameraAI::Director::PursuitStart() {
    if (mPursuitStartTime <= 0.0f) {
        {
            MGamePlayMoment msg(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x88bff834);
            msg.Send(UCrc32("MomentStrm"));
        }
        {
            MMiscSound snd(1);
            snd.Send(UCrc32("play"));
        }

        mPursuitStartTime = 5.0f;
        mCinematicSlowdownSeconds = 3.0f;
    }
}

// --- Free functions ---

IPlayer *FindPlayer(EVIEW_ID id) {
    for (IPlayer *const *iter = IPlayer::GetList(PLAYER_LOCAL).begin(); iter != IPlayer::GetList(PLAYER_LOCAL).end(); ++iter) {
        IPlayer *ip = *iter;
        if (ip->GetControllerPort() == static_cast<int>(id)) {
            return ip;
        }
    }
    return nullptr;
}

CameraAI::Director *FindDirector(EVIEW_ID id) {
    for (CameraAI::Director *const *iter = CameraAI::Director::GetList().begin(); iter != CameraAI::Director::GetList().end(); ++iter) {
        CameraAI::Director *cd = *iter;
        if (cd->GetViewID() == id) {
            return cd;
        }
    }
    return nullptr;
}

CameraAI::Director *FindDirector(unsigned int id) {
    for (CameraAI::Director *const *iter = CameraAI::Director::GetList().begin(); iter != CameraAI::Director::GetList().end(); ++iter) {
        CameraAI::Director *cd = *iter;
        IPlayer *iplayer = FindPlayer(cd->GetViewID());
        if (iplayer != nullptr) {
            ISimable *isimable = iplayer->GetSimable();
            if (isimable != nullptr && isimable->GetWorldID() == id) {
                return cd;
            }
        }
    }
    return nullptr;
}

bool AreMomentCamerasEnabled() {
    bool splitCheck = false;
    if (FEDatabase->IsSplitScreenMode()) {
        splitCheck = (FEDatabase->iNumPlayers == 2);
    }
    if (splitCheck) {
        return false;
    }
    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        return false;
    }
    return FEDatabase->GetGameplaySettings()->JumpCam;
}

// --- CameraAI namespace functions ---

void CameraAI::Update(float dT) {
    IPlayer::Count(PLAYER_LOCAL);
    unsigned int player = 0;
    do {
        EVIEW_ID viewID = static_cast<EVIEW_ID>(++player);
        IPlayer *iplayer = FindPlayer(viewID);
        Director *cd = FindDirector(viewID);
        if (cd != nullptr && iplayer == nullptr) {
            delete cd;
            continue;
        }
        if (iplayer != nullptr && cd == nullptr) {
            cd = new (static_cast<const char *>(0)) Director(viewID);
        }
    } while (player <= static_cast<unsigned int>(PLAYER_LOCAL));
    for (Director *const *iter = Director::GetList().begin(); iter != Director::GetList().end(); ++iter) {
        Director *cd = *iter;
        cd->Update(dT);
    }
}

void CameraAI::Reset() {
    for (Director *const *iter = Director::GetList().begin(); iter != Director::GetList().end(); ++iter) {
        Director *cd = *iter;
        cd->Reset();
    }
}

void CameraAI::SetAction(EVIEW_ID viewID, const char *desiredMode) {
    for (Director *const *iter = Director::GetList().begin(); iter != Director::GetList().end(); ++iter) {
        Director *cd = *iter;
        if (cd->GetViewID() == viewID) {
            cd->SetAction(Attrib::StringKey(desiredMode));
        }
    }
}

void CameraAI::MaybeKillPursuitCam(unsigned int id) {
    Director *cd = FindDirector(id);
    if (cd != nullptr) {
        cd->EndPursuitStart();
    }
}

static float AverageAir(ISimable *isimable, float fSeconds, float *pHighest, float *pLongest) {
    IRigidBody *irb = isimable->GetRigidBody();
    if (irb == nullptr) return 0.0f;

    ICollisionBody *irbc;
    if (!isimable->QueryInterface(&irbc)) return 0.0f;

    float fSpeed = irb->GetSpeed();
    if (fSpeed < 1.0f) return 0.0f;

    int nSteps = static_cast<int>(fSpeed * fSeconds * 0.5f);
    if (nSteps <= 0) return 0.0f;

    ISuspension *isuspension;
    if (!isimable->QueryInterface(&isuspension)) return 0.0f;

    IVehicle *vehicle;
    if (!isimable->QueryInterface(&vehicle)) return 0.0f;

    UMath::Vector3 p = UMath::Vector3::kZero;

    for (unsigned int j = 0; j < isuspension->GetNumWheels(); j++) {
        UMath::Vector3 wp = isuspension->GetWheelPos(j);
        const UMath::Vector3 &upVec = irbc->GetUpVector();
        float compression = isuspension->GetCompression(j);
        UMath::ScaleAdd(upVec, -compression, wp, wp);
        UMath::ScaleAdd(wp, 0.25f, p, p);
    }

    UMath::Vector4 vNormal = irbc->GetGroundNormal();
    WWorldPos pTopo = isimable->GetWPos();
    pTopo.SetTolerance(5.0f);

    float fElevation = pTopo.HeightAtPoint(p);
    float fAirSum = 0.0f;
    float fAirMax = bMax(0.0f, p.y - fElevation);
    float fStep = fSeconds / static_cast<float>(nSteps);
    float fAirTime = fStep;
    float fDeparture = 0.0f;

    if (fAirMax <= 0.0f) {
        fAirTime = 0.0f;
    } else {
        fDeparture = -fStep;
    }

    Attrib::Gen::pvehicle attributes(vehicle->GetVehicleAttributes());
    Attrib::Gen::chassis chassis(attributes.chassis(0), 0, nullptr);

    float fDownForce = -Physics::Info::AerodynamicDownforce(chassis, fSpeed);
    float fDownAccel = irbc->GetGravity() + fDownForce / irb->GetMass();

    UMath::Vector3 a = UMath::Vector3Make(0.0f, fDownAccel, 0.0f);

    UMath::Vector3 v = irb->GetLinearVelocity();

    UMath::Vector3 pNew;
    UMath::ScaleAdd(v, 0.5f, p, pNew);

    const float tbarr = 1.0f;

    UMath::Vector4 seg[2];
    seg[0] = UMath::Vector4Make(p, tbarr);
    seg[1] = UMath::Vector4Make(pNew, tbarr);

    WCollisionMgr::WorldCollisionInfo cInfo;
    WCollisionMgr(0, 3).CheckHitWorld(seg, cInfo, 2);

    if (cInfo.HitSomething()) {
        return 0.0f;
    }

    int i = 1;
    float fFuture = fAirTime;

    for (i = 1; i < nSteps; i++) {
        fFuture = fStep * static_cast<float>(i) - fDeparture;
        UMath::ScaleAdd(a, fFuture * 0.5f, v, pNew);
        UMath::ScaleAdd(pNew, fFuture, p, pNew);

        if (pTopo.Update(pNew, vNormal, true, nullptr, true)) {
            if (pTopo.OnValidFace() && 0.5f <= vNormal.y) {
                float fElevation = pTopo.HeightAtPoint(pNew);
                float fAir = pNew.y - fElevation;
                if (fAir <= 0.0f) break;
                fAirMax = bMax(fAirMax, fAir);
                fAirSum += fAir;
            }
        }
    }

    if (pHighest != nullptr) {
        *pHighest = fAirMax;
    }
    if (pLongest != nullptr) {
        *pLongest = fFuture;
    }

    return fAirSum / static_cast<float>(i);
}

void CameraAI::MaybeKillJumpCam(unsigned int id) {
    Director *cd = FindDirector(id);
    if (cd != nullptr) {
        cd->EndJumping();
    }
}

void CameraAI::Init() {
    TheAvoidables = new Avoidables();
}

void CameraAI::Shutdown() {
    if (TheAvoidables != nullptr) {
        delete TheAvoidables;
    }
    TheAvoidables = nullptr;
    Director::List copy(Director::GetList());
    for (Director *const *iter = copy.begin(); iter != copy.end(); ++iter) {
        Director *cd = *iter;
        if (cd != nullptr) {
            delete cd;
        }
    }
}

void CameraAI::AddAvoidable(IBody *body) {
    Avoidables::iterator iter = _STL::find(TheAvoidables->begin(), TheAvoidables->end(), body);
    if (iter == TheAvoidables->end()) {
        TheAvoidables->push_back(body);
    }
}

void CameraAI::RemoveAvoidable(IBody *body) {
    Avoidables::iterator iter = _STL::find(TheAvoidables->begin(), TheAvoidables->end(), body);
    if (iter != TheAvoidables->end()) {
        TheAvoidables->erase(iter);
    }
}

void CameraAI::StartCinematicSlowdown(EVIEW_ID viewID, float seconds) {
    for (Director *const *iter = Director::GetList().begin(); iter != Director::GetList().end(); ++iter) {
        Director *cd = *iter;
        if (cd->GetViewID() == viewID) {
            Action *action = cd->GetAction();
            if (action != nullptr && action->GetName() == Attrib::StringKey("DRIVE")) {
                cd->SetCinematicSlowdown(seconds);
            }
        }
    }
}

void CameraAI::MaybeDoTotaledCam(IPlayer *iplayer) {
    if (Sim::GetUserMode() != Sim::USER_SINGLE) {
        return;
    }
    for (Director *const *iter = Director::GetList().begin(); iter != Director::GetList().end(); ++iter) {
        Director *cd = *iter;
        if (cd->GetViewID() == iplayer->GetControllerPort()) {
            cd->TotaledStart();
        }
    }
}

void CameraAI::MaybeDoPursuitCam(IVehicle *ivehicle) {
    if (TheICEManager.IsEditorOn()) {
        return;
    }
    if (!AreMomentCamerasEnabled()) {
        return;
    }
    INIS *nis = UTL::Collections::Singleton<INIS>::Get();
    if (nis != nullptr) {
        return;
    }
    GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
    if (parms != nullptr) {
        if (parms->GetIsPursuitRace()) {
            return;
        }
        if (GRaceStatus::Get().GetRaceTimeElapsed() < 5.0f) {
            return;
        }
    }
    if (ivehicle->GetDriverStyle() == STYLE_DRAG) {
        return;
    }
    ISimable *isimable = ivehicle->GetSimable();
    if (isimable == nullptr) {
        return;
    }
    Director *cd = FindDirector(isimable->GetWorldID());
    if (cd == nullptr) {
        return;
    }
    if (gGameBreakerCamera) {
        return;
    }
    cd->PursuitStart();
}

// NON-MATCHING: dead store offset mismatch (stw r0, 0xa0(r1) vs 0x70(r1))
// GCC internal stack slot reuse picks Vector4 temp slot instead of _GetKind return slot
void CameraAI::MaybeDoJumpCam(ISimable *isimable) {
    if (TheICEManager.IsEditorOn()) {
        return;
    }
    if (!AreMomentCamerasEnabled()) {
        return;
    }
    if (UTL::Collections::Singleton<INIS>::Get() != nullptr) {
        return;
    }
    IVehicle *ivehicle;
    if (isimable->QueryInterface(&ivehicle)) {
        if (ivehicle->GetDriverStyle() == STYLE_DRAG) {
            return;
        }
    }
    Director *cd = FindDirector(isimable->GetWorldID());
    if (cd == nullptr) {
        return;
    }
    if (cd->IsJumping()) {
        return;
    }
    if (gGameBreakerCamera) {
        return;
    }
    UMath::Vector3 velocity;
    isimable->GetLinearVelocity(velocity);
    float speed = UMath::Length(velocity);
    if (speed < 10.0f) {
        return;
    }
    int set = 0;
    bVector3 position;
    TrackPathZone *zone = TheTrackPathManager.FindZone(
        reinterpret_cast<const bVector2 *>(
            &bConvertFromBond(position, isimable->GetPosition())),
        TRACK_PATH_ZONE_JUMP_CAM, nullptr);
    if (zone != nullptr) {
        set = 1;
    }
    float highest = 0.0f;
    float longest = 0.0f;
    float avg = AverageAir(isimable, 3.0f, &highest, &longest);
    if (avg >= 20.1f) {
        return;
    }
    if (highest >= 20.1f) {
        return;
    }
    if (longest >= 3.1f) {
        return;
    }
    if (avg <= 1.0f) {
        return;
    }
    if (highest > Tweak_JumpCamHighestAirTresh[set] &&
        longest * speed > Tweak_JumpCamLongestAirTresh[set]) {
        SetAction(cd->GetViewID(), "CDActionTrackCar");
        cd->JumpStart(bClamp(longest + longest, 1.0f, 4.0f));
        MGamePlayMoment msg(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0xa3447e3f);
        msg.Send(UCrc32("MomentStrm"));
    }
    if (avg > 1.0f) {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai != nullptr) {
            Observer *observer = ai->GetObserver();
            if (observer != nullptr) {
                observer->NotifyAirborne(highest, longest);
            }
        }
    }
}


// Static member definitions
template <>
UTL::Collections::Listable<CameraAI::Director, 2>::List
    UTL::Collections::Listable<CameraAI::Director, 2>::_mTable;
