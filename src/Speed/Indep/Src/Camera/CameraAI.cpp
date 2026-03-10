#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

#include <list>

#include <types.h>

DECLARE_CONTAINER_TYPE(CameraAIAvoidables);

struct Avoidables : public _STL::list<IBody *, UTL::Std::Allocator<IBody *, _type_CameraAIAvoidables> > {
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }
};

static Avoidables *TheAvoidables;

static float kJumpTimeMultiplier = 2.0f;
static float kEndJumpThreshold = 0.0f;
static float kEndJumpValue = -1.0f;
static float kEndPursuitThreshold = 0.0f;
static float kEndPursuitValue = -1.0f;

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
    mCinematicSlowdownSeconds = 0.0f;
    mJumpTime = 0.0f;
    mPursuitStartTime = 0.0f;
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
    if (mAction == nullptr) {
        return nullptr;
    }
    return mAction->GetMover();
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
    // TODO
}

void CameraAI::Director::SelectAction() {
    // TODO
}

void CameraAI::Director::TotaledStart() {
    // TODO
}

void CameraAI::Director::PursuitStart() {
    // TODO
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
    if (FEDatabase->IsSplitScreenMode()) {
        if (FEDatabase->iNumPlayers == 2) {
            return false;
        }
    }
    if (!FEDatabase->IsLANMode() && !FEDatabase->IsOnlineMode()) {
        return FEDatabase->GetGameplaySettings()->JumpCam;
    }
    return false;
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
    // TODO
}

void CameraAI::RemoveAvoidable(IBody *body) {
    // TODO
}

void CameraAI::StartCinematicSlowdown(EVIEW_ID viewID, float seconds) {
    // TODO
}

void CameraAI::MaybeDoTotaledCam(IPlayer *iplayer) {
    // TODO
}

void CameraAI::MaybeDoPursuitCam(IVehicle *ivehicle) {
    // TODO
}

void CameraAI::MaybeDoJumpCam(ISimable *simable) {
    // TODO
}
