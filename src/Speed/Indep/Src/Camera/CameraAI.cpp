#include "CameraAI.hpp"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MICECameraFinished.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

Avoidables *TheAvoidables = nullptr;

typedef UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32> CameraAIActionFactory;

BIND_SIM_SUBSYSTEM(CameraAI, CameraAI::Init, CameraAI::Shutdown);

IMPLEMENT_LISTABLE(CameraAI::Director)
IMPLEMENT_FACTORY(CameraAIActionFactory)

extern int gGameBreakerCamera;
extern GameFlowManager TheGameFlowManager;

void SetNewSndCamAction(Attrib::StringKey camtype, EVIEW_ID viewID);

static const float Tweak_JumpCamHighestAirTresh[2] = {2.5f, 1.8f};
static const float Tweak_JumpCamLongestAirTresh[2] = {25.0f, 15.0f};

namespace CameraAI {

} // namespace CameraAI

CameraAI::Director::Director(EVIEW_ID view)
    : mView(view), mActionName("CDActionDrive"), mAction(0), mQueue(1, 0x98c7a2f5, "CameraAI", false), mUnused(0), mPursuitTimer(0.0f),
      mJumpTimer(0.0f), mFlags(0), mCinematicScale(0.0f) {}

CameraAI::Director::~Director() {
    ReleaseAction();
}

void CameraAI::Director::ReleaseAction() {
    if (mAction) {
        delete mAction;
        mAction = 0;
    }
}

void CameraAI::Director::Reset() {
    mJumpTimer = 0.0f;
    mPursuitTimer = 0.0f;
    mFlags = 0;
    mCinematicScale = 0.0f;
    SetAction(Attrib::StringKey("CDActionDrive"));
    if (mAction) {
        mAction->Reset();
    }
}

void CameraAI::Director::SetAction(Attrib::StringKey action) {
    mActionName = action;
    if (mAction) {
        Attrib::StringKey override_name = mAction->GetNext();
        Attrib::StringKey *p_override = &override_name;
        if (!p_override->IsEmpty()) {
            mActionName = *p_override;
        }
    }
    if (mAction) {
        if (mAction->GetName() == mActionName) {
            return;
        }
    }
    if (mActionName.IsEmpty()) {
        return;
    }
    Action *next = UTL::COM::Factory<Director *, Action, UCrc32>::CreateInstance(UCrc32(mActionName.GetHash32()), this);
    if (!next) {
        return;
    }
    ReleaseAction();
    mAction = next;
    SetNewSndCamAction(mActionName, mView);
}

static bool AreMomentCamerasEnabled() {
    if (FEDatabase->IsSplitScreenMode()) {
        return false;
    }
    if (FEDatabase->IsLANMode() || FEDatabase->IsOnlineMode()) {
        return false;
    }
    return FEDatabase->GetGameplaySettings()->JumpCam;
}

static inline bool ICEHasGenericGroup() {
    ICEManager *mgr = &TheICEManager;
    unsigned int empty_hash = bStringHash("");
    return mgr->GetPlayGenericGroupHash() != empty_hash;
}

void CameraAI::Director::SelectAction() {
    if (TheICEManager.IsEditorOff()) {
        if (mJumpTimer < 0.0f) {
            mJumpTimer = 0.0f;
            mActionName = Attrib::StringKey("CDActionDrive");
            mFlags = 1;
        }
        if (mPursuitTimer < 0.0f) {
            mPursuitTimer = 0.0f;
            mActionName = Attrib::StringKey("CDActionDrive");
            mFlags = 1;
        }
        if (mPursuitTimer > 0.0f && mPursuitTimer < 2.0f) {
            mActionName = Attrib::StringKey("CDActionTrackCop");
        }

        if (!gGameBreakerCamera) {
            eView *view = &eViews[mView];
            if (view) {
                CameraMover *mover = view->GetCameraMover();
                if (mover && mover->GetType() == CM_DRIVE_CUBIC) {
                    CameraAnchor *anchor = mover->GetAnchor();
                    if (anchor && AreMomentCamerasEnabled()) {
                        float vel = anchor->GetVelocityMagnitude();
                        if ((vel > 20.0f || (vel > 10.0f && anchor->IsTouchingGround())) &&
                            anchor->IsCloseToRoadBlock()) {
                            mActionName = Attrib::StringKey("CDActionTrackCar");
                            mJumpTimer = 2.0f;
                            MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x2d8acb81).Send("MomentStrm");
                        }
                    }
                }
            }
        }

        int want_ice = 0;
        INIS *nis = INIS::Get();
        if (nis && nis->IsPlaying()) {
            ICEScene *scene = nis->GetScene();
            if (scene) {
                want_ice = scene->IsControllingCamera();
                mFlags = nis->IsWorldMomement();
            }
        }
        if (want_ice || ICEHasGenericGroup()) {
            mActionName = Attrib::StringKey("CDActionIce");
            mJumpTimer = 0.0f;
            mPursuitTimer = 0.0f;
        } else if (mAction && mAction->GetName() == Attrib::StringKey("CDActionIce")) {
            TheICEManager.SetUseRealTimeWord(0);
            mActionName = Attrib::StringKey("CDActionDrive");
            if (INIS::Get()) {
                INIS::Get()->FireEventTag("ICEEnd");
            }
            MICECameraFinished().Post(UCrc32(0x20d60dbf));
        }
    }

    if (mAction) {
        Attrib::StringKey next = mAction->GetNext();
        Attrib::StringKey *pnext = &next;
        if (!pnext->IsEmpty()) {
            mActionName = *pnext;
        }
    }
    if (mAction) {
        if (mAction->GetName() == mActionName) {
            return;
        }
    }
    if (mActionName.IsEmpty()) {
        return;
    }
    Action *created = UTL::COM::Factory<Director *, Action, UCrc32>::CreateInstance(UCrc32(mActionName.GetHash32()), this);
    if (!created) {
        return;
    }
    mFlags = 0;
    mCinematicScale = 0.0f;
    ReleaseAction();
    mAction = created;
    SetNewSndCamAction(mActionName, mView);
}

void CameraAI::Director::TotaledStart() {
    mActionName = Attrib::StringKey("CDActionTrackCar");
    mJumpTimer = 2.0f;

    SetAction(mActionName);
}

void CameraAI::Director::JumpStart(float t) {
    mJumpTimer = t * (4.0f / 3.0f);
}

void CameraAI::Director::EndJumping() {
    if (mJumpTimer < 1.0f) {
        mJumpTimer = -1.0f;
    }
}

void CameraAI::Director::PursuitStart() {
    if (mPursuitTimer > 0.0f) {
        return;
    }
    MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x88bff834).Send("MomentStrm");
    MMiscSound(1).Send("Snd");
    mPursuitTimer = 2.5f;
    mCinematicScale = 0.5f;
}

void CameraAI::Director::EndPursuitStart() {
    if (mPursuitTimer >= 2.0f) {
        return;
    }
    mPursuitTimer = -1.0f;
}

void CameraAI::Director::Update(float dT) {
    if (!TheGameFlowManager.IsPaused()) {

        if (mJumpTimer > 0.0f) {
            mJumpTimer -= dT;
        }
    }
    if (!FEManager::ShouldPauseSimulation(true)) {

        if (mPursuitTimer > 0.0f) {
            mPursuitTimer -= dT;
        }
    }
    SelectAction();
    if (mAction) {
        if (mAction->GetName() == Attrib::StringKey("CDActionDrive")) {
            mAction->SetSpecial(mCinematicScale);
        }
    }
    if (mAction) {
        mAction->Update(dT);
    }
}

CameraMover *CameraAI::Director::GetMover() {
    if (mAction) {
        return mAction->GetMover();
    }
    return 0;
}

static IPlayer *FindPlayer(EVIEW_ID view) {
    for (IPlayer::List::const_iterator it = IPlayer::GetList(PLAYER_LOCAL).begin(); it != IPlayer::GetList(PLAYER_LOCAL).end(); ++it) {
        IPlayer *player = *it;
        if (player->GetRenderPort() == view) {
            return player;
        }
    }
    return 0;
}

static CameraAI::Director *FindDirector(EVIEW_ID view) {
    const UTL::Collections::Listable<CameraAI::Director, 2>::List &list = CameraAI::Director::GetList();
    for (UTL::Collections::Listable<CameraAI::Director, 2>::List::const_iterator it = list.begin(); it != list.end(); ++it) {
        CameraAI::Director *director = *it;
        if (director->GetView() == view) {
            return director;
        }
    }
    return 0;
}

static CameraAI::Director *FindDirector(unsigned int world_id) {
    for (UTL::Collections::Listable<CameraAI::Director, 2>::List::const_iterator it = CameraAI::Director::GetList().begin(); it != CameraAI::Director::GetList().end(); ++it) {
        CameraAI::Director *director = *it;
        IPlayer *player = FindPlayer(director->GetView());
        if (player) {
            ISimable *simable = player->GetSimable();
            if (simable && simable->GetWorldID() == world_id) {
                return director;
            }
        }
    }
    return 0;
}

void CameraAI::Update(float dT) {
    unsigned int playercount = IPlayer::Count(PLAYER_LOCAL);
    {
        unsigned int player = EVIEW_FLAILER;
        do {
            EVIEW_ID viewID = (EVIEW_ID)(player + 1);
            IPlayer *iplayer = FindPlayer(viewID);
            Director *cd = FindDirector(viewID);
            if (cd && !iplayer) {
                delete cd;
            } else if (iplayer && !cd) {
                new Director(viewID);
            }
            player++;
        } while (player <= EVIEW_PLAYER1);
    }
    for (UTL::Collections::Listable<Director, 2>::List::const_iterator iter = Director::GetList().begin(); iter != Director::GetList().end();
         ++iter) {
        Director *cd = *iter;
        cd->Update(dT);
    }
}

void CameraAI::Reset() {
    for (UTL::Collections::Listable<Director, 2>::List::const_iterator it = Director::GetList().begin(); it != Director::GetList().end(); ++it) {
        (*it)->Reset();
    }
}

void CameraAI::SetAction(EVIEW_ID view, const char *action) {
    for (UTL::Collections::Listable<Director, 2>::List::const_iterator it = Director::GetList().begin(); it != Director::GetList().end(); ++it) {
        Director *director = *it;
        if (director->GetView() == view) {
            director->SetAction(Attrib::StringKey(action));
        }
    }
}

void CameraAI::StartCinematicSlowdown(EVIEW_ID view, float scale) {
    for (UTL::Collections::Listable<Director, 2>::List::const_iterator it = Director::GetList().begin(); it != Director::GetList().end(); ++it) {
        Director *director = *it;
        if (director->GetView() == view) {
            Action *action = director->GetAction();
            if (action && action->GetName() == Attrib::StringKey("CDActionDrive")) {
                director->SetCinematicSlowdown(scale);
            }
        }
    }
}

void CameraAI::MaybeDoTotaledCam(IPlayer *iplayer) {
    if (Sim::GetUserMode() != 0) {
        return;
    }
    for (UTL::Collections::Listable<Director, 2>::List::const_iterator it = Director::GetList().begin(); it != Director::GetList().end(); ++it) {
        Director *director = *it;
        if (director->GetView() == iplayer->GetRenderPort()) {
            director->TotaledStart();
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
    if (INIS::Get()) {
        return;
    }

    GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
    if (parms) {
        if (parms->GetIsPursuitRace()) {
            return;
        }
        if (GRaceStatus::Get().GetRaceTimeElapsed() < 1.0f) {
            return;
        }
    }
    if (ivehicle->GetDriverStyle() == STYLE_DRAG) {
        return;
    }

    ISimable *simable = ivehicle->GetSimable();
    if (!simable) {
        return;
    }

    Director *director = FindDirector(simable->GetWorldID());
    if (!director) {
        return;
    }
    if (gGameBreakerCamera) {
        return;
    }

    director->PursuitStart();
}

void CameraAI::MaybeKillPursuitCam(unsigned int world_id) {
    Director *director = FindDirector(world_id);
    if (director) {
        director->EndPursuitStart();
    }
}

static float AverageAir(ISimable *isimable, float fSeconds, float *pHighest, float *pLongest) {
    IRigidBody *irb = isimable->GetRigidBody();
    if (!irb) {
        return 0.0f;
    }

    ICollisionBody *irbc;
    if (!isimable->QueryInterface(&irbc)) {
        return 0.0f;
    }

    float fSpeed = irb->GetSpeed();
    if (fSpeed < 10.0f) {
        return 0.0f;
    }

    int nSteps = static_cast<int>(fSpeed * fSeconds * (1.0f / 3.0f));
    if (nSteps <= 0) {
        return 0.0f;
    }

    ISuspension *isuspension;
    if (!isimable->QueryInterface(&isuspension)) {
        return 0.0f;
    }

    IVehicle *vehicle;
    if (!isimable->QueryInterface(&vehicle)) {
        return 0.0f;
    }

    UMath::Vector3 p = UMath::Vector3::kZero;
    for (unsigned int j = 0; j < isuspension->GetNumWheels(); j++) {
        UMath::Vector3 wp = isuspension->GetWheelPos(j);
        UMath::ScaleAdd(irbc->GetUpVector(), -isuspension->GetWheelRadius(j), wp, wp);
        UMath::ScaleAdd(wp, 0.25f, p, p);
    }

    UMath::Vector4 vNormal = irbc->GetGroundNormal();
    WWorldPos pTopo = isimable->GetWPos();
    pTopo.SetTolerance(4.0f);
    float fElevation = pTopo.HeightAtPoint(p);
    float fAirSum = bMax(0.0f, p.y - fElevation);
    float fAirMax = fAirSum;
    float fStep = fSeconds / static_cast<float>(nSteps);
    float fAirTime = (fAirSum > 0.0f) ? fStep : 0.0f;
    float fDeparture = (fAirSum > 0.0f) ? -fStep : 0.0f;

    const Attrib::Gen::pvehicle attributes = vehicle->GetVehicleAttributes();
    const Attrib::Gen::chassis chassis(attributes.chassis(0), 0, 0);
    float fDownForce = -Physics::Info::AerodynamicDownforce(chassis, fSpeed);
    float fDownAccel = irbc->GetGravity() + fDownForce / irb->GetMass();

    UMath::Vector3 a = UMath::Vector3Make(0.0f, fDownAccel, 0.0f);
    UMath::Vector3 v = irb->GetLinearVelocity();
    UMath::Vector3 pNew;
    const float tbarr = 0.5f;
    UMath::ScaleAdd(v, tbarr, p, pNew);

    UMath::Vector4 seg[2];
    seg[0] = UMath::Vector4Make(p, 1.0f);
    seg[1] = UMath::Vector4Make(pNew, 1.0f);
    WCollisionMgr::WorldCollisionInfo cInfo;
    WCollisionMgr(0, 3).CheckHitWorld(seg, cInfo, 2);
    if (cInfo.HitSomething()) {
        return 0.0f;
    }

    int i;
    for (i = 1; i < nSteps; i++) {
        fAirTime = fStep * static_cast<float>(i) - fDeparture;
        UMath::ScaleAdd(a, fAirTime * tbarr, v, pNew);
        UMath::ScaleAdd(pNew, fAirTime, p, pNew);
        if (pTopo.Update(pNew, vNormal, true, 0, true) && pTopo.OnValidFace() && vNormal.y >= tbarr) {
            float fElevation = pTopo.HeightAtPoint(pNew);
            float fAir = pNew.y - fElevation;
            if (fAir > 0.0f) {
                fAirMax = bMax(fAirMax, fAir);
                fAirSum += fAir;
            } else {
                break;
            }
        }
    }

    if (pHighest) {
        *pHighest = fAirMax;
    }
    if (pLongest) {
        *pLongest = fAirTime;
    }
    return fAirSum / static_cast<float>(i);
}

void CameraAI::MaybeKillJumpCam(unsigned int world_id) {
    Director *director = FindDirector(world_id);
    if (director) {
        director->EndJumping();
    }
}

void CameraAI::MaybeDoJumpCam(ISimable *isimable) {
    if (TheICEManager.IsEditorOn()) {
        return;
    }
    if (!AreMomentCamerasEnabled()) {
        return;
    }
    if (INIS::Get()) {
        return;
    }

    IVehicle *ivehicle = 0;
    if (isimable->QueryInterface(&ivehicle) && ivehicle->GetDriverStyle() == STYLE_DRAG) {
        return;
    }

    Director *director = FindDirector(isimable->GetWorldID());
    if (!director) {
        return;
    }
    if (director->IsJumping()) {
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

    int in_jump_zone = 0;
    bVector3 position;
    TrackPathZone *zone = TheTrackPathManager.FindZone(
        reinterpret_cast<const bVector2 *>(&bConvertFromBond(position, isimable->GetPosition())), TRACK_PATH_ZONE_JUMP_CAM, 0);
    if (zone) {
        in_jump_zone = 1;
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

    if (highest > Tweak_JumpCamHighestAirTresh[in_jump_zone] && longest * speed > Tweak_JumpCamLongestAirTresh[in_jump_zone]) {
        CameraAI::SetAction(director->GetView(), "CDActionTrackCar");
        director->JumpStart(bClamp(longest + longest, 1.0f, 4.0f));

        MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0xa3447e3f).Send("MomentStrm");
    }

    if (avg > 1.0f) {
        SoundAI *sound = UTL::Collections::Singleton<SoundAI>::Get();
        if (sound && sound->GetObserver()) {
            sound->GetObserver()->NotifyAirborne(highest, longest);
        }
    }
}

void CameraAI::Init() {
    TheAvoidables = new Avoidables;
}

void CameraAI::Shutdown() {
    delete TheAvoidables;
    TheAvoidables = 0;
    UTL::Collections::Listable<Director, 2>::List list = Director::GetList();
    for (UTL::Collections::Listable<Director, 2>::List::iterator it = list.begin(); it != list.end(); ++it) {
        delete *it;
    }
}

void CameraAI::AddAvoidable(IBody *ibody) {
    Avoidables::iterator it = std::find(TheAvoidables->begin(), TheAvoidables->end(), ibody);
    if (it == TheAvoidables->end()) {
        TheAvoidables->push_back(ibody);
    }
}

void CameraAI::RemoveAvoidable(IBody *ibody) {
    Avoidables::iterator it = std::find(TheAvoidables->begin(), TheAvoidables->end(), ibody);
    if (it != TheAvoidables->end()) {
        TheAvoidables->erase(it);
    }
}
static void _cam64_prime_conv(UMath::Matrix4 &m, bMatrix4 &bm, UMath::Vector3 &v, bVector3 &bv) {
    ConversionUtil::RightToLeftMatrix4(m, m);
    ConversionUtil::RightToLeftVector3(v, v);
    ConversionUtil::RightToLeftMatrix4(bm, m);
    ConversionUtil::RightToLeftVector3(bv, v);
}

// `_STL` es el espacio de nombres del STLport de GameCube; en PS2 configure.py lo
// define vacio (-D_STL=) y esta instanciacion da el find<> global que la STL de SGI
// tambien emite. MSVC no conoce `_STL`.
#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_PLAYSTATION2)
namespace _STL {
template IVehicle *const *find<IVehicle *const *, IVehicle const *>(IVehicle *const *, IVehicle *const *, IVehicle const *const &);
}
#endif
