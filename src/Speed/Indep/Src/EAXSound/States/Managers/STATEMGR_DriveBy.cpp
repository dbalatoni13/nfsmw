#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Events/ESpawnSmackable.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/EventManager.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/Src/World/WTriggerList.h"

static const bool debug_SmackableWooshs_ALL = true; // size: 0x1, Decl: 23
static const bool debug_SmackableWooshs_CAM = true; // size: 0x1, Decl: 24
static const bool debug_SmackableWooshs_CAR = true; // size: 0x1, Decl: 25
static const bool debug_EventWooshs = true;         // size: 0x1, Decl: 26

CSTATEMGR_DriveBy::CSTATEMGR_DriveBy() : CSTATEMGR_Base() {
    this->WooshCheckFrameCntr = 4;
}

CSTATEMGR_DriveBy::~CSTATEMGR_DriveBy() {}

float CAMERA_WOOSH_OFFSET = 2.0f; // size: 0x4, address: 0x80417F40, Decl: 44

void TestSmackableForWoosh(IModel *model, int carid) {
    if (!debug_SmackableWooshs_ALL) {
        return;
    }
    if (IsSoundEnabled == 0) {
        return;
    }
    if (model->IsHidden()) {
        return;
    }
    if (model->GetAttributes().GetClass() != Attrib::Gen::smackable::ClassKey()) {
        return;
    }

    Attrib::Gen::smackable smackattribs(model->GetAttributes());

    if (!smackattribs.IsWooshable()) {
        return;
    }

    UMath::Matrix4 transform;
    model->GetTransform(transform);

    eDRIVE_BY_TYPE bytype;
    int numcars;
    int foundcarid;
    if (transform.v1.y > 0.8f) {
        float maxradius = 12.0f;

        if (bLength(SndCamera::GetV3WorldCarVel(carid)) < 3.0f) {
            return;
        }
        if (!debug_SmackableWooshs_CAR) {
            return;
        }

        UMath::Vector4 cam_pos;
        UMath::Vector4 objpos = transform.v3;
        objpos.y = 0.0f;

        bVector3 WorldCarPos(SndCamera::GetWorldCarPos(carid)->x, SndCamera::GetWorldCarPos(carid)->y, 0.0f);
        BVEC_TO_UMATH((&WorldCarPos), cam_pos);

        float dissquared = UMath::DistanceSquarexyz(objpos, cam_pos);
        if (dissquared > maxradius * maxradius) {
            return;
        }

        bytype = smackattribs.WooshType();
        goto CreateWoosh;
    } else {
        if (!debug_SmackableWooshs_CAM) {
            return;
        }
        float maxradius = 4.0f;
        UMath::Vector4 cam_pos;
        UMath::Vector4 objpos = transform.v3;

        bVector3 bcam_pos;
        bScaleAdd(&bcam_pos, SndCamera::GetCamPos(carid), SndCamera::GetCamDir(carid), CAMERA_WOOSH_OFFSET);

        BVEC_TO_UMATH((&bcam_pos), cam_pos);

        float dissquared = UMath::DistanceSquarexyz(objpos, cam_pos);
        if (dissquared > maxradius * maxradius) {
            return;
        }

        bytype = DRIVE_BY_CAMERA_BY;
    }

CreateWoosh:
    stDriveByInfo tmpdrivebypackage;
    tmpdrivebypackage.eDriveByType = bytype;
    tmpdrivebypackage.pEAXCar = SndCamera::GetPlayerCar(carid);
    tmpdrivebypackage.ClosingVelocity = tmpdrivebypackage.pEAXCar->GetPhysCar()->GetVelocityMagnitude();
    tmpdrivebypackage.UniqueID = reinterpret_cast<uintptr_t>(model->GetInstanceHandle());
    UMATH_TO_BVEC((&tmpdrivebypackage.vLocation), transform.v3);

    CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
    if (ReturnedObj != nullptr) {
        ReturnedObj->Attach(&tmpdrivebypackage);
    }
}

void TestAllSmackablesForWhoosh() {
    int numcars = SndCamera::NumPlayers;
    for (int n = 0; n < numcars; n++) {
        UMath::Vector3 pt;
        BVEC_TO_UMATH(SndCamera::GetWorldCarPos3(n), pt);
        if (SndCamera::GetWorldCarVel(n) < 10.0f) {
            continue;
        }

        float radius = 20.0f;
        WTriggerList tList;
        tList.reserve(128);
        WTriggerManager::Get().GetIntersectingTriggers(pt, radius, &tList);

        for (WTriggerList::iterator tIter = tList.begin(); tIter != tList.end(); ++tIter) {
            WTrigger &trig = **tIter;
            const CARP::EventStaticData *event_data;

            if (trig.HasEvent(ESpawnSmackable::kEventID, &event_data)) {
                const ESpawnSmackable::StaticData *data = reinterpret_cast<const ESpawnSmackable::StaticData *>(event_data->StaticData());
                IModel *model = IModel::FindInstance(data->fScenery);
                if (!model->IsHidden()) {
                    TestSmackableForWoosh(model, n);
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
        int n = player_idx;
        if (SndCamera::GetWorldCarVel(n) >= 5.0f) {
            float predict_ahead_time = 0.15f;
            bVector3 futurepos3d = bScaleAdd(*SndCamera::GetWorldCarPos3(n), *SndCamera::GetV3WorldCarVel(n), predict_ahead_time);
            bVector2 futurepos2d(futurepos3d.x, futurepos3d.y);
            DrivableScenerySection *scenery_section = TheVisibleSectionManager.FindDrivableSection(&futurepos2d);
            int LODOffset = TheVisibleSectionManager.GetLODOffset();

            if (scenery_section == nullptr) {
                continue;
            }

            emEvent **current_event = emTriggerEventsInSection(&futurepos3d, scenery_section->GetSectionNumber() + LODOffset);
            if (current_event == nullptr) {
                continue;
            }

            while (*current_event != nullptr) {
                emEvent *event = *current_event;
                EventTrigger *event_trigger = event->pEventTrigger;

                if (event->ID == 0x10007) {
                    stDriveByInfo tmpdrivebypackage;
                    unsigned int uhash = event->pEventTrigger->GetNameHash();

                    if ((uhash == STRINGHASH_LAMPPOST) || (uhash == STRINGHASH_PILLAR) || (uhash == STRINGHASH_TRAFFIC_LIGHT) ||
                        (uhash == STRINGHASH_FREEWAY_SIGN)) {
                        tmpdrivebypackage.eDriveByType = DRIVE_BY_LAMPPOST;
                    } else if (uhash == STRINGHASH_TREE) {
                        tmpdrivebypackage.eDriveByType = DRIVE_BY_TREE;
                    }

                    tmpdrivebypackage.pEAXCar = SndCamera::GetPlayerCar(n);
                    tmpdrivebypackage.ClosingVelocity = SndCamera::GetWorldCarVel(n);
                    tmpdrivebypackage.vLocation = *event_trigger->GetPosition();

                    CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);

                    if (ReturnedObj != nullptr) {
                        ReturnedObj->Attach(&tmpdrivebypackage);
                    }
                }

                current_event++;
                if (current_event == nullptr) {
                    break;
                }
            }
        }
    }

    this->WooshCheckFrameCntr--;
    if (this->WooshCheckFrameCntr < 0) {
        this->WooshCheckFrameCntr = 4;
    }

    CSTATEMGR_Base::UpdateParams(t);
}

void CSTATEMGR_DriveBy::UpdateSmackables(float t) {}

void CSTATEMGR_DriveBy::EnterWorld(eSndGameMode esgm) {
    int SFXID = 0;
    for (int n = 0; n < 6; n++) {
        CSTATE_Base *NewState = this->CreateState(SFXID, 1);
        NewState->Setup(1);
    }

    CSTATEMGR_Base::EnterWorld(esgm);
}

CSTATE_Base *CSTATEMGR_DriveBy::GetFreeState(void *ObjectPtr) {
    stDriveByInfo *NewDriveByInfo = static_cast<stDriveByInfo *>(ObjectPtr);
    if (g_EAXIsPaused()) {
        return nullptr;
    }

    CSTATE_DriveBy *CurStateObj;
    for (CurStateObj = static_cast<CSTATE_DriveBy *>(this->m_pHeadStateObj); CurStateObj != nullptr;
         CurStateObj = static_cast<CSTATE_DriveBy *>(CurStateObj->m_pNextState)) {
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

    CSTATE_Base *CurReturn = CSTATEMGR_Base::GetFreeState(ObjectPtr);
    if (CurReturn != nullptr) {
        return CurReturn;
    }

    for (CurStateObj = static_cast<CSTATE_DriveBy *>(this->m_pHeadStateObj); CurStateObj != nullptr;
         CurStateObj = static_cast<CSTATE_DriveBy *>(CurStateObj->m_pNextState)) {
        if (CurStateObj->m_DriveByInfo.eDriveByType <= NewDriveByInfo->eDriveByType) {
            CurStateObj->Detach();
            return CurStateObj;
        }
    }

    return nullptr;
}
