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

#include <list>

#include <types.h>

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

extern Avoidables *TheAvoidables;
extern bool gGameBreakerCamera;

void SetNewSndCamAction(Attrib::StringKey mode, EVIEW_ID viewID);

static float kJumpTimeMultiplier = 2.0f;
static float kEndJumpThreshold = 0.0f;
static float kEndJumpValue = -1.0f;
static float kEndPursuitThreshold = 0.0f;
static float kEndPursuitValue = -1.0f;
static float kJumpSpeedHigh = 100.0f;
static float kJumpSpeedLow = 80.0f;
static float kJumpDuration = 5.0f;

// --- Director methods ---

CameraAI::Director::Director(EVIEW_ID viewID)
    : UTL::Collections::Listable<Director, 2>(), //
      mViewID(viewID),                  //
      mDesiredMode(""),                 //
      mAction(nullptr),                 //
      mInputQ(false),                   //
      mPrepareToEnableIce(false),       //
      mPursuitStartTime(0.0f),          //
      mJumpTime(0.0f),                  //
      mIsCinematicMomement(false),      //
      mCinematicSlowdownSeconds(0.0f) {
    Reset();
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
    mPursuitStartTime = 0.0f;
    mJumpTime = 0.0f;
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
    mDesiredMode = desiredMode;
    if (mAction != nullptr) {
        Attrib::StringKey key = mAction->GetNext();
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
            ReleaseAction();
            mAction = action;
            SetNewSndCamAction(mDesiredMode, mViewID);
        }
    }
}

void CameraAI::Director::SelectAction() {
    if (TheICEManager.IsEditorOff()) {
        bool isICEPlaying = false;

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
                            UMath::Vector4 position = UMath::Vector4::kZero;
                            UMath::Vector4 vector = UMath::Vector4::kZero;
                            UMath::Vector4 velocity = UMath::Vector4::kZero;
                            MGamePlayMoment msg(position, vector, velocity, 0,
                                                Attrib::StringHash32("jump"));
                            msg.Deliver();
                        }
                    }
                }
            }
        }

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
            mPursuitStartTime = 0.0f;
            mJumpTime = 0.0f;
        } else {
            if (mAction != nullptr && mAction->GetName() == Attrib::StringKey("ICE")) {
                TheICEManager.SetUseRealTime(false);
                mDesiredMode = Attrib::StringKey("DRIVE");
                INIS *nis2 = UTL::Collections::Singleton<INIS>::Get();
                if (nis2 != nullptr) {
                    nis2->FireEventTag("CameraFinished");
                }
                MICECameraFinished finishedMsg;
                finishedMsg.Post(UCrc32(0x20d60dbf));
            }
        }
    }

    if (mAction != nullptr) {
        Attrib::StringKey key = mAction->GetNext();
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
    Attrib::StringKey key("TOTALED");
    mDesiredMode = key;
    mJumpTime = 0.0f;
    SetAction(mDesiredMode);
}

void CameraAI::Director::PursuitStart() {
    if (mPursuitStartTime <= 0.0f) {
        UMath::Vector4 position = UMath::Vector4::kZero;
        UMath::Vector4 vector = UMath::Vector4::kZero;
        UMath::Vector4 velocity = UMath::Vector4::kZero;
        MGamePlayMoment msg(position, vector, velocity, 0,
                            Attrib::StringHash32("pursuit"));
        msg.Deliver();

        MMiscSound snd(1);
        snd.SetID(Attrib::StringHash32("play"));
        snd.Deliver();

        mPursuitStartTime = 5.0f;
        mCinematicSlowdownSeconds = 3.0f;
    }
}

// --- Free functions ---

IPlayer *FindPlayer(EVIEW_ID id) {
    const UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::List &list =
        UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::GetList(PLAYER_LOCAL);
    for (IPlayer *const *iter = list.begin(); iter != list.end(); ++iter) {
        IPlayer *ip = *iter;
        if (ip->GetControllerPort() == static_cast<int>(id)) {
            return ip;
        }
    }
    return nullptr;
}

CameraAI::Director *FindDirector(EVIEW_ID id) {
    const CameraAI::Director::List &list = CameraAI::Director::GetList();
    for (CameraAI::Director *const *iter = list.begin(); iter != list.end(); ++iter) {
        CameraAI::Director *cd = *iter;
        if (cd->GetViewID() == id) {
            return cd;
        }
    }
    return nullptr;
}

CameraAI::Director *FindDirector(unsigned int id) {
    const CameraAI::Director::List &list = CameraAI::Director::GetList();
    for (CameraAI::Director *const *iter = list.begin(); iter != list.end(); ++iter) {
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
    if (FEDatabase->IsLANMode() || FEDatabase->IsOnlineMode()) {
        return false;
    }
    return FEDatabase->GetGameplaySettings()->JumpCam;
}

// --- CameraAI namespace functions ---

void CameraAI::Update(float dT) {
    unsigned int playercount = UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::GetList(PLAYER_LOCAL).size();
    for (unsigned int player = 1; player <= playercount; ++player) {
        EVIEW_ID viewID = static_cast<EVIEW_ID>(player);
        IPlayer *iplayer = FindPlayer(viewID);
        Director *cd = FindDirector(viewID);
        if (cd == nullptr) {
            if (iplayer != nullptr) {
                cd = new ("CameraAI") Director(viewID);
            }
        } else if (iplayer == nullptr) {
            delete cd;
        }
    }
    const Director::List &list = Director::GetList();
    for (Director *const *iter = list.begin(); iter != list.end(); ++iter) {
        Director *cd = *iter;
        cd->Update(dT);
    }
}

void CameraAI::Reset() {
    const Director::List &list = Director::GetList();
    for (Director *const *iter = list.begin(); iter != list.end(); ++iter) {
        Director *cd = *iter;
        cd->Reset();
    }
}

void CameraAI::SetAction(EVIEW_ID viewID, const char *desiredMode) {
    const Director::List &list = Director::GetList();
    for (Director *const *iter = list.begin(); iter != list.end(); ++iter) {
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
    if (irb != nullptr) {
        ICollisionBody *irbc;
        if (isimable->QueryInterface(&irbc)) {
            float fSpeed = irb->GetSpeed();
            if (fSpeed >= 1.0f) {
                int nSteps = static_cast<int>(fSpeed * fSeconds * 0.5f);
                if (nSteps > 0) {
                    ISuspension *isuspension;
                    IVehicle *vehicle;
                    if (isimable->QueryInterface(&isuspension) && isimable->QueryInterface(&vehicle)) {
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
                        float fAirTime = 0.0f;
                        float fDeparture = 0.0f;

                        if (0.0f < fAirMax) {
                            fDeparture = -fStep;
                            fAirTime = fStep;
                        }

                        Attrib::Gen::pvehicle attributes(vehicle->GetVehicleAttributes());
                        Attrib::Gen::chassis chassis(attributes.chassis(0), 0, nullptr);

                        float fDownForce = Physics::Info::AerodynamicDownforce(chassis, fSpeed);
                        float gravity = irbc->GetGravity();
                        float fDownAccel = gravity + (-fDownForce / irb->GetMass());

                        UMath::Vector3 a = UMath::Vector3Make(0.0f, fDownAccel, 0.0f);

                        const UMath::Vector3 &linVel = irb->GetLinearVelocity();
                        UMath::Vector3 v;
                        v.x = linVel.x;
                        v.y = linVel.y;
                        v.z = linVel.z;

                        UMath::Vector3 pNew;
                        UMath::ScaleAdd(v, 0.5f, p, pNew);

                        const float tbarr = 1.0f;

                        UMath::Vector4 seg[2];
                        seg[0] = UMath::Vector4Make(p, tbarr);
                        seg[1] = UMath::Vector4Make(pNew, tbarr);

                        WCollisionMgr::WorldCollisionInfo cInfo;
                        WCollisionMgr collMgr(0, 3);
                        collMgr.CheckHitWorld(seg, cInfo, 2);

                        if (!cInfo.HitSomething()) {
                            int i = 1;
                            bool bHighest = pHighest != nullptr;
                            bool bLongest = pLongest != nullptr;
                            float fFuture = fAirTime;

                            if (nSteps > 1) {
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
                            }

                            if (bHighest) {
                                *pHighest = fAirMax;
                            }
                            if (bLongest) {
                                *pLongest = fFuture;
                            }

                            return fAirSum / static_cast<float>(i);
                        }
                    }
                }
            }
        }
    }
    return 0.0f;
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
        TheAvoidables->clear();
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
    Avoidables::iterator iter;
    for (iter = TheAvoidables->begin(); iter != TheAvoidables->end(); ++iter) {
        if (*iter == body) {
            break;
        }
    }
    if (iter == TheAvoidables->end()) {
        TheAvoidables->push_back(body);
    }
}

void CameraAI::RemoveAvoidable(IBody *body) {
    Avoidables::iterator iter;
    for (iter = TheAvoidables->begin(); iter != TheAvoidables->end(); ++iter) {
        if (*iter == body) {
            break;
        }
    }
    if (iter != TheAvoidables->end()) {
        TheAvoidables->erase(iter);
    }
}

void CameraAI::StartCinematicSlowdown(EVIEW_ID viewID, float seconds) {
    const Director::List &list = Director::GetList();
    for (Director *const *iter = list.begin(); iter != list.end(); ++iter) {
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
    const Director::List &list = Director::GetList();
    for (Director *const *iter = list.begin(); iter != list.end(); ++iter) {
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

void CameraAI::MaybeDoJumpCam(ISimable *simable) {
    // TODO
}
