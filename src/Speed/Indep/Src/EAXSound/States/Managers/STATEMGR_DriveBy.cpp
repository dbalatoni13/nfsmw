#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_DriveBy.hpp"

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Events/ESpawnSmackable.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/World/EventManager.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/Src/World/WTrigger.h"

extern float CAMERA_WOOSH_OFFSET;
CSTATEMGR_DriveBy::CSTATEMGR_DriveBy()
    : CSTATEMGR_Base() {
    WooshCheckFrameCntr = 4;
}

CSTATEMGR_DriveBy::~CSTATEMGR_DriveBy() {}

void TestSmackableForWoosh(IModel *model, int carid) {
    eDRIVE_BY_TYPE bytype;

    if (!IsSoundEnabled) {
        return;
    }

    if (model->IsHidden()) {
        return;
    }

    if (model->GetAttributes().GetClass() != Attrib::Gen::smackable::ClassKey()) {
        return;
    }

    Attrib::Gen::smackable smackattribs(model->GetAttributes());

    if (smackattribs.IsWooshable()) {
        UMath::Matrix4 transform;

        model->GetTransform(transform);

        if (transform[1].y > 0.8f) {
            float maxradius = 12.0f;
            UMath::Vector4 cam_pos;
            UMath::Vector4 objpos;
            bVector3 WorldCarPos;
            float dissquared;

            if (bLength(*SndCamera::GetV3WorldCarVel(carid)) >= 3.0f) {
                WorldCarPos = bVector3(SndCamera::GetWorldCarPos(carid)->x, SndCamera::GetWorldCarPos(carid)->y, 0.0f);
                cam_pos.x = -WorldCarPos.y;
                cam_pos.y = 0.0f;
                cam_pos.z = WorldCarPos.x;
                objpos = transform.v3;
                objpos.y = 0.0f;
                dissquared = VU0_v4distancesquarexyz(objpos, cam_pos);
                if (dissquared <= maxradius * maxradius) {
                    bytype = smackattribs.WooshType();
                    goto CreateWoosh;
                }
            }
        } else {
            float maxradius = 4.0f;
            UMath::Vector4 cam_pos;
            UMath::Vector4 objpos;
            bVector3 bcam_pos;
            float dissquared;

            objpos = transform.v3;
            bScaleAdd(&bcam_pos, SndCamera::GetCamPos(carid), SndCamera::GetCamDir(carid), CAMERA_WOOSH_OFFSET);
            cam_pos.x = -bcam_pos.y;
            cam_pos.y = bcam_pos.z;
            cam_pos.z = bcam_pos.x;
            dissquared = VU0_v4distancesquarexyz(objpos, cam_pos);
            if (dissquared <= maxradius * maxradius) {
                bytype = DRIVE_BY_CAMERA_BY;
                goto CreateWoosh;
            }
        }
    }

    return;

CreateWoosh:
    stDriveByInfo tmpdrivebypackage;
    CSTATE_Base *ReturnedObj;

    tmpdrivebypackage.eDriveByType = bytype;
    tmpdrivebypackage.pEAXCar = SndCamera::GetPlayerCar(carid);
    tmpdrivebypackage.ClosingVelocity = tmpdrivebypackage.pEAXCar->GetPhysCar()->GetVelocityMagnitude();
    tmpdrivebypackage.UniqueID = reinterpret_cast<unsigned int>(model->GetInstanceHandle());
    ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
    if (ReturnedObj) {
        ReturnedObj->Attach(&tmpdrivebypackage);
    }
}



void TestAllSmackablesForWhoosh() {
    int numcars;

    numcars = SndCamera::NumPlayers;
    for (int n = 0; n < numcars; n++) {
        UMath::Vector3 pt;
        float radius;
        WTriggerList tList;

        pt.x = SndCamera::GetWorldCarPos3(n)->x;
        pt.y = -SndCamera::GetWorldCarPos3(n)->y;
        pt.z = SndCamera::GetWorldCarPos3(n)->z;
        if (SndCamera::GetWorldCarVel(n) >= 10.0f) {
            radius = 20.0f;
            tList.reserve(128);
            WTriggerManager::Get().GetIntersectingTriggers(pt, radius, &tList);

            for (WTrigger **tIter = tList.begin(); tIter != tList.end(); ++tIter) {
                WTrigger &trig = **tIter;
                const EventStaticData *event_data;

                if (trig.HasEvent(0xEB626F77, &event_data)) {
                    const ESpawnSmackable::StaticData *data;
                    IModel *model;

                    data = reinterpret_cast<const ESpawnSmackable::StaticData *>(event_data->StaticData());
                    model = IModel::FindInstance(data->fScenery);
                    if (model && !model->IsHidden()) {
                        TestSmackableForWoosh(model, n);
                    }
                }
            }
        }
    }
}

void CSTATEMGR_DriveBy::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);
    IPlayer *player;
    int player_idx;

    TestAllSmackablesForWhoosh();
    for (player_idx = 0; player_idx < SndCamera::NumPlayers; player_idx++) {
        int n;

        n = player_idx;
        if (SndCamera::GetWorldCarVel(n) >= 5.0f) {
            float predict_ahead_time = 0.15f;
            bVector3 futurepos3d = bScaleAdd(*SndCamera::GetWorldCarPos3(n), *SndCamera::GetV3WorldCarVel(n), predict_ahead_time);
            bVector2 futurepos2d(futurepos3d.x, futurepos3d.y);
            DrivableScenerySection *scenery_section = TheVisibleSectionManager.FindDrivableSection(&futurepos2d);
            int LODOffset;
            emEvent **current_event;

            if (scenery_section) {
                LODOffset = TheVisibleSectionManager.GetLODOffset();
                current_event = emTriggerEventsInSection(&futurepos3d, scenery_section->GetSectionNumber() + LODOffset);
                if (current_event) {
                    emEvent *event;
                    EventTrigger *event_trigger;

                    event = *current_event;
                    while (event) {
                        event_trigger = event->pEventTrigger;
                        if (event->ID == 0x10007) {
                            stDriveByInfo tmpdrivebypackage;
                            unsigned int uhash;
                            CSTATE_Base *ReturnedObj;

                            tmpdrivebypackage.eDriveByType = DRIVE_BY_LAMPPOST;
                            tmpdrivebypackage.pEAXCar = nullptr;
                            tmpdrivebypackage.ClosingVelocity = 0.0f;
                            tmpdrivebypackage.UniqueID = 0;
                            uhash = event_trigger->GetNameHash();
                            if ((uhash != 0xF40A48EF) && (uhash != 0x72F66B23) && (uhash != 0xB6BD9C95) &&
                                (uhash != 0xF3ABE1C2)) {
                                if (uhash == 0x001D5D4F) {
                                    tmpdrivebypackage.eDriveByType = DRIVE_BY_TREE;
                                }
                            }

                            tmpdrivebypackage.pEAXCar = SndCamera::GetPlayerCar(n);
                            tmpdrivebypackage.ClosingVelocity = SndCamera::GetWorldCarVel(n);
                            tmpdrivebypackage.vLocation = *event_trigger->GetPosition();
                            ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
                            if (ReturnedObj) {
                                ReturnedObj->Attach(&tmpdrivebypackage);
                            }
                        }

                        current_event++;
                        if (!current_event) {
                            break;
                        }
                        event = *current_event;
                    }
                }
            }
        }
    }

    WooshCheckFrameCntr--;
    if (WooshCheckFrameCntr < 0) {
        WooshCheckFrameCntr = 4;
    }
    CSTATEMGR_Base::UpdateParams(t);
}

void CSTATEMGR_DriveBy::EnterWorld(eSndGameMode esgm) {
    int SFXID;

    SFXID = 0;
    for (int n = 0; n < 6; n++) {
        CSTATE_Base *NewState;

        NewState = CreateState(SFXID, 1);
        NewState->Setup(1);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

CSTATE_Base *CSTATEMGR_DriveBy::GetFreeState(void *ObjectPtr) {
    stDriveByInfo *NewDriveByInfo;
    CSTATE_DriveBy *CurStateObj;
    CSTATE_Base *CurReturn;

    NewDriveByInfo = static_cast<stDriveByInfo *>(ObjectPtr);
    if (g_EAXIsPaused()) {
        return nullptr;
    }

    for (CurStateObj = static_cast<CSTATE_DriveBy *>(m_pHeadStateObj); CurStateObj; CurStateObj = static_cast<CSTATE_DriveBy *>(CurStateObj->m_pNextState)) {
        if (CurStateObj->IsAttached()) {
            if (bDistBetween(CurStateObj->m_DriveByInfo.vLocation, NewDriveByInfo->vLocation) < 0.5f) {
                return nullptr;
            }
            if (NewDriveByInfo->UniqueID && NewDriveByInfo->UniqueID == CurStateObj->m_DriveByInfo.UniqueID &&
                NewDriveByInfo->eDriveByType == CurStateObj->m_DriveByInfo.eDriveByType) {
                return nullptr;
            }
        }
    }

    CurReturn = CSTATEMGR_Base::GetFreeState(ObjectPtr);
    if (CurReturn) {
        return CurReturn;
    }

    for (CurStateObj = static_cast<CSTATE_DriveBy *>(m_pHeadStateObj); CurStateObj; CurStateObj = static_cast<CSTATE_DriveBy *>(CurStateObj->m_pNextState)) {
        if (CurStateObj->m_DriveByInfo.eDriveByType <= NewDriveByInfo->eDriveByType) {
            CurStateObj->Detach();
            return CurStateObj;
        }
    }

    return nullptr;
}
