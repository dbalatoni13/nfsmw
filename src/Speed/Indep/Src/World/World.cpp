#include "./World.hpp"
#include "./DebugVehicleSelection.h"
#include "./DebugWorld.h"
#include "./Track.hpp"
#include "./TrackStreamer.hpp"
#include "CarRender.hpp"
#include "Clans.hpp"
#include "RaceParameters.hpp"
#include "Rain.hpp"
#include "Scenery.hpp"
#include "Skids.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Rumble.hpp"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "TrackInfo.hpp"
#include "TrackPath.hpp"

#include "types.h"

static void World_Init();

// BSS Class Init
bVector3 ZeroVector = bVector3(0, 0, 0);
Sim::SubSystem _Physics_System_World = Sim::SubSystem(nullptr, World_Init, nullptr);

float UglyTimestepHack = 0.016666668f;
World *pCurrentWorld = nullptr;

extern int WorldLoopCounter;                  // zMisc
extern bVector3 *SkipFEOverrideStartPosition; // zMiscSmall

extern void ResetWorldAnimations(); // zAnim
extern void ResetPropTimers();      // zPhysics

World::World() {
    pCurrentWorld = this;
    this->WorldRandomSeed = bRandom(0x7FFFFFFF);
    this->TotalNumCars = 0;
    this->AICarPos = 0;
    this->TrafficCarPos = 0;
    this->ParkedCarPos = 0;
    this->CollisionPredictionID = 1;
    this->PotentialDriveTargetID = 1;
    this->OnlinePauseWorld = 0;

    this->ResetTimeScale();
    this->PreviousTimeToWaste = 0.0f;
    bMemSet(this->PlayerPositions, 0x0, 0x60);
    bMemSet(this->PlayerDATs, 0x0, 0x18);
}

World::~World() {
    while (!this->CarList.IsEmpty()) {
        delete this->CarList.GetHead()->Remove();
    }
    pCurrentWorld = nullptr;
}

// STRIPPED
void World::DoSnapshot(ReplaySnapshot *snapshot) {}

void World::ResetTimeScale() {
    this->fTimeScale = 1.0f;
    this->fTimeScaleTimer = 0.0f;
    this->fTimeToRemainPaused = 0.0f;
    this->TimeScaleCallback = 0;
    this->TimeScaleCallbackParam = 0;
}

// UNSOLVED
void World_DEBUGStartLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec) {
    const char *regionName = LoadedTrackInfo->GetLoadedTrackInfo();
    float rotInitialVec = 0.0f;

    if (strcasecmp(regionName, "L2RA") == 0) {
        startLoc.x = -2510.0f;
        startLoc.y = 151.0f;
        startLoc.z = 1767.0f;
        rotInitialVec = 0.15f;
    } else if (strcasecmp(regionName, "L2RB") == 0) {
        startLoc.x = 0.0f;
        startLoc.y = 21.0f;
        startLoc.z = 0.0f;
    } else if (strcasecmp(regionName, "L2RC") == 0) {
        startLoc.x = 0.0f;
        startLoc.y = 21.0f;
        startLoc.z = 0.0f;
    } else if (strcasecmp(regionName, "L2RD") == 0) {
        startLoc.x = 16.0f;
        startLoc.y = 457.0f;
        startLoc.z = 43.0f;
        rotInitialVec = 0.63f;
    } else if (strcasecmp(regionName, "L2RE") == 0) {
        startLoc.x = 16.0f;
        startLoc.y = 457.0f;
        startLoc.z = 43.0f;
    } else if (strcasecmp(regionName, "L2RG") == 0) {
        startLoc.x = 2036.0f;
        startLoc.y = 70.0f;
        startLoc.z = -2010.0f;
    } else if (strcasecmp(regionName, "L4RA") == 0) {
        startLoc.x = 30.0f;
        startLoc.y = 20.0f;
        startLoc.z = 40.0f;
    } else if (strcasecmp(regionName, "L5RD") == 0) {
        startLoc.x = -20.0f;
        startLoc.y = 2.0f;
        startLoc.z = 100.0f;
    } else if (strcasecmp(regionName, "L2RX") == 0) {
        startLoc.x = -2510.0f;
        startLoc.y = 151.0f;
        startLoc.z = 1767.0f;
        rotInitialVec = 0.63f;
    }

    initialVec = UMath::Vector3Make(0.0f, 0.0f, 1.0f);

    if (rotInitialVec != 0.0f) {
        UMath::Matrix4 rotMat;

        UMath::MultYRot(UMath::Matrix4::kIdentity, rotInitialVec, rotMat);
        UMath::Rotate(initialVec, rotMat, initialVec);
    }

    GRaceCustom *quickRace = GRaceDatabase::Get().GetStartupRace();

    if (quickRace) {
        quickRace->GetStartPosition(startLoc);
        quickRace->GetStartDirection(initialVec);
    } else if (GManager::Get().Exists()) {
        GManager::Get().GetRespawnLocation(startLoc, initialVec);
    }

    if (SkipFEOverrideStartPosition != nullptr) {
        startLoc.x = -SkipFEOverrideStartPosition->y;
        startLoc.y = SkipFEOverrideStartPosition->z;
        startLoc.z = SkipFEOverrideStartPosition->x;
    }
}

static void HideNonRaceSmackable(IModel *model) {
    ISceneryModel *scenery = (ISceneryModel *)model;

    if (scenery->QueryInterface(&scenery)) {
        if (scenery->IsExcluded(4)) {
            model->ReleaseModel();
        }
    }
}

void World_RestoreProps() {
    for (IModel::List::const_iterator m = IModel::GetList().begin(); m != IModel::GetList().end(); m++) {
        IModel *model = *m;
        ISceneryModel *iscenery = (ISceneryModel *)model;
        if (iscenery->QueryInterface(&iscenery)) {
            iscenery->RestoreScene();
        }
    }

    for (IExplosion::List::const_iterator m = IExplosion::GetList().begin(); m != IExplosion::GetList().end(); m++) {
        IExplosion *explosion = *m;
        ISimable *isimable = (ISimable *)explosion;
        if (isimable->QueryInterface(&isimable)) {
            isimable->Kill();
        }
    }

    if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        IModel::ForEach(HideNonRaceSmackable);
    }

    GManager::Get().RestorePursuitBreakerIcons(-1);
    ResetWorldAnimations();
    ResetPropTimers();
}

void World_Service() {
    UglyTimestepHack = 0.0f;

    INIS *nis = INIS::Get();
    if (nis != nullptr) {
        nis->ServiceLoads();
    }

    UglyTimestepHack = 0.0f;
    ServiceSpaceNodes();
    TheTrackStreamer.ServiceGameState();

    if (GManager::Get().Exists()) {
        GManager::Get().NotifyWorldService();
    }
    DebugVehicleSelection::Get().Service();
    DebugWorld::Get().Service();
}

RaceParameters TheRaceParameters;

static void World_Init() {
    ResetWorldTime();
    TheICEManager.Resolve();
    EstablishRemoteCaffeineConnection();
    TrackPathInitRemoteCaffeineConnection();
    InitCarEffects();
    InitClans();

    int max_skids = 0x80;
    if (TheRaceParameters.IsDriftRace()) {
        max_skids = 0x200;
    }
    InitSkids(max_skids);
    ResetCameraShakers();
    CameraMoverRestartRace();
    InitVisibleZones();
    TheTrackPathManager.ResetZoneVisitInfos();
    SetRainBase();
}

static void World_Shutdown() {
    bVerifyPoolIntegrity(0);
    CloseVisibleZones();
    CloseClans();
    CloseSkids();
    CloseCarEffects();
    ResetWorldTime();

    WorldLoopCounter = 0x186A0;
}
