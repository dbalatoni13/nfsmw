#include "EPlayRaceNIS.hpp"

#include <new>

#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Animation/AnimChooser.hpp"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

extern int gGameBreakerCamera;
extern bool Tweak_TriggerMomentAlways;

EPlayRaceNIS::EPlayRaceNIS(GMarker *pStartMarker, const char *pSceneName, const char *pSceneType, int pPlayerIdx, int pCameraTrack,
                           const char *pPreMovie, const char *pPostMovie)
    : Event(0x30), fStartMarker(pStartMarker), fSceneName(EventManager::EmbedField(this, pSceneName)),
      fSceneType(EventManager::EmbedField(this, pSceneType)), fPlayerIdx(pPlayerIdx), fCameraTrack(pCameraTrack),
      fPreMovie(EventManager::EmbedField(this, pPreMovie)), fPostMovie(EventManager::EmbedField(this, pPostMovie)) {
    if (!fSceneName) {
        return;
    }

    if (bStrLen(fSceneName) == 0) {
        return;
    }

    if (INIS::Get()) {
        return;
    }

    CAnimChooser::eType nisType = CAnimChooser::Intro;

    if (bStrICmp("Outro", fSceneType) == 0) {

        nisType = CAnimChooser::Ending;
    } else if (bStrICmp("Arrest", fSceneType) == 0) {

        nisType = CAnimChooser::Arrest;
    } else if (bStrICmp("Moment", fSceneType) == 0) {

        if (!Tweak_TriggerMomentAlways) {

            if (!FEDatabase->GetGameplaySettings()->JumpCam) {
                return;
            }

            if (FEDatabase->IsSplitScreenMode()) {
                return;
            }

            if (gGameBreakerCamera) {
                return;
            }

            GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();

            if (parms) {
                if (!parms->GetIsPursuitRace()) {
                    return;
                }
            }

            if (Sim::GetTime() - g_TriggerMomentNISTime > 2.0f) {
                nisType = CAnimChooser::Intro;

                return;
            }

            bool bTriggerMomentCopsAffected = false;
            {
                IVehicle *playerVehicle = IVehicle::First(VEHICLE_PLAYERS);

                if (playerVehicle) {

                    IVehicleAI *ivehiclea = playerVehicle->GetAIVehiclePtr();

                    if (ivehiclea) {

                        IPursuit *ipursuit = ivehiclea->GetPursuit();

                        if (ipursuit && ipursuit->IsPerpBusted()) {
                            return;
                        }
                    }

                    ISimable *simable = playerVehicle->GetSimable();

                    if (simable) {

                        for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_AICOPS).begin();
                             iter != IVehicle::GetList(VEHICLE_AICOPS).end(); ++iter) {

                            IVehicle *p_car = *iter;

                            if (p_car && p_car->IsActive() && p_car->GetVehicleClass() == VehicleClass::CAR) {

                                IVehicleAI *p_vehicleai;

                                if (p_car->QueryInterface(&p_vehicleai)) {

                                    AITarget *p_target = p_vehicleai->GetTarget();

                                    if (p_target && p_target->IsValid()) {

                                        ISimable *p_targetsimable = p_target->GetSimable();

                                        if (p_targetsimable && p_targetsimable->GetWorldID() == simable->GetWorldID()) {

                                            UMath::Vector3 car_pos = simable->GetPosition();
                                            UMath::Vector3 cop_pos = p_car->GetPosition();

                                            float dist = UMath::Distancexz(cop_pos, car_pos);
                                            const IRigidBody *body = simable->GetRigidBody();
                                            float distTresh = 80.0f;

                                            if (body) {

                                                UMath::Vector3 car_fwd;
                                                UMath::Vector3 cop_dir;

                                                body->GetForwardVector(car_fwd);
                                                UMath::Direction(car_pos, cop_pos, cop_dir);

                                                float minTresh = 0.25f;
                                                float dot = UMath::Dot(car_fwd, cop_dir) + 1.0f;
                                                float dotScale = 0.75f;
                                                float dotHalf = 0.5f;

                                                distTresh = ((dot * dotHalf) * dotScale + minTresh) * distTresh;
                                            }

                                            if (dist < distTresh) {

                                                bTriggerMomentCopsAffected = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (!bTriggerMomentCopsAffected) {

                return;
            }
        }

        nisType = CAnimChooser::Moment;
    } else if (bStrICmp("Intro", fSceneType) != 0) {

        if (fSceneType) {
            return;
        }
    }

    TheICEManager.SetGenericCameraToPlay("", "");

    Sim::IActivity *activity = Sim::IActivity::CreateInstance("NISActivity", Sim::Param());

    INIS *nis;

    if (activity && activity->QueryInterface(&nis)) {

        if (nisType != CAnimChooser::Moment) {

            char channelName[24];
            int numRacers = IVehicle::Count(VEHICLE_RACERS) < 8 ? IVehicle::Count(VEHICLE_RACERS) : 8;
            IVehicle *racerVehicle = IVehicle::First(VEHICLE_RACERS);

            // El `i++` en la clausula del for (y el `i + 1` en el bSPrintf) es lo
            // que reparte los cinco GPR como el objetivo: con `++i` dentro del
            // bSPrintf, `i` vive 100 insns y `racerVehicle` 34, y global_alloc
            // ordena racerVehicle(9705) > _mLists(3214) > i(3000) -> r30/r29/r28;
            // el objetivo es i > racerVehicle > _mLists. Medido: 31 filas -> 3.
            for (int i = 0; i < numRacers; i++) {

                bSPrintf(channelName, "car%d", i + 1);
                nis->AddCar(UCrc32(channelName), racerVehicle);

                racerVehicle = racerVehicle->Next(VEHICLE_RACERS);
            }
        }

        UMath::Vector3 markerPos = UMath::Vector3::kZero;
        UMath::Vector3 markerDir = UMath::Vector3::kZero;

        markerDir.z = 1.0f;

        if (fStartMarker) {

            markerPos = fStartMarker->GetPosition();
            markerDir = fStartMarker->GetDirection();
        } else {

            IVehicle *playerVehicle = IVehicle::First(VEHICLE_PLAYERS);

            if (playerVehicle) {

                ISimable *simable = playerVehicle->GetSimable();

                if (simable) {

                    UMath::Matrix4 mat;

                    simable->GetTransform(mat);

                    markerPos = UMath::Vector4To3(mat.v3);
                    markerDir = UMath::Vector4To3(mat.v0);
                }
            }
        }

        float markerAngle = 1.0f - UMath::Atan2a(markerDir.x, markerDir.z);

        nis->SetPreMovie(fPreMovie);
        nis->SetPostMovie(fPostMovie);
        nis->StartLocation(markerPos, markerAngle);
        nis->Load(nisType, fSceneName, fCameraTrack, true);

        IPlayer *player = IPlayer::First(PLAYER_LOCAL);

        if (player && player->InGameBreaker()) {

            player->ToggleGameBreaker();
        }
    }
}

EPlayRaceNIS::~EPlayRaceNIS() {
}

const char *EPlayRaceNIS::GetEventName() const {
    return "EPlayRaceNIS";
}

void EPlayRaceNIS_MakeEvent_Callback(const void *staticData) {
    new EPlayRaceNIS(((EPlayRaceNIS::StaticData *) staticData)->fStartMarker, ((EPlayRaceNIS::StaticData *) staticData)->fSceneName,
                     ((EPlayRaceNIS::StaticData *) staticData)->fSceneType, ((EPlayRaceNIS::StaticData *) staticData)->fPlayerIdx,
                     ((EPlayRaceNIS::StaticData *) staticData)->fCameraTrack, ((EPlayRaceNIS::StaticData *) staticData)->fPreMovie,
                     ((EPlayRaceNIS::StaticData *) staticData)->fPostMovie);
}

int EPlayRaceNIS_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 7) {
        new EPlayRaceNIS((GMarker *) lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3), (int) lua_tonumber(L, 4),
                         (int) lua_tonumber(L, 5), lua_tostring(L, 6), lua_tostring(L, 7));
    }
    return 0;
}

void EPlayRaceNIS_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EPlayRaceNIS::StaticData *) event)->fStartMarker) CARP::TagReference(group);
    new (&((EPlayRaceNIS::StaticData *) event)->fSceneName) CARP::TagReference(group);
    new (&((EPlayRaceNIS::StaticData *) event)->fSceneType) CARP::TagReference(group);
    new (&((EPlayRaceNIS::StaticData *) event)->fPreMovie) CARP::TagReference(group);
    new (&((EPlayRaceNIS::StaticData *) event)->fPostMovie) CARP::TagReference(group);
}
