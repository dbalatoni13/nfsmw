#include "Speed/Indep/Src/World/WTrigger.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern EventDynamicData gEventDynamicData;

WTrigger::WTrigger() {
    bMemSet(this, 0, sizeof(WTrigger));
}

WTrigger::WTrigger(const UMath::Matrix4 &mat, const UMath::Vector3 &dimensions, EventList *eventList, unsigned int flags) {
    memcpy(this, &mat, sizeof(UMath::Matrix4));
    fShape = 1;
    fEvents = eventList;
    fFlags = flags;
    fHeight = dimensions.y + dimensions.y;
    fType = 0;
    fPosRadius.x = mat[3][0];
    fPosRadius.y = mat[3][1];
    fPosRadius.z = mat[3][2];
    fPosRadius.w = UMath::Length(dimensions);
    fMatRow0Width.w = dimensions.x + dimensions.x;
    fMatRow2Length.w = dimensions.z + dimensions.z;
}

bool WTrigger::HasEvent(unsigned int eventID, const CARP::EventStaticData** foundEvent) const {
    if (fEvents != nullptr) {
        return EventManager::ListHasEvent(fEvents, eventID, foundEvent);
    }
    return false;
}

bool WTrigger::TestDirection(const UMath::Vector3& vec) const {
    return UMath::Dot(UMath::Vector4To3(fMatRow2Length), vec) >= 0.0f;
}

void WTrigger::FireEvents(HSIMABLE__ *hSimable) {
    if (fEvents != nullptr) {
        bMemSet(&gEventDynamicData, 0, sizeof(EventDynamicData));
        gEventDynamicData.fhSimable = reinterpret_cast<uintptr_t>(hSimable);
        gEventDynamicData.fPosition = fPosRadius;
        gEventDynamicData.fPosition.w = 1.0f;
        gEventDynamicData.fTrigger = this;
        gEventDynamicData.fTriggerStimulus = WTriggerManager::Get().GetCurrentStimulus();
        EventManager::FireEventList(fEvents, false);
    }
    if (fFlags & 2) {
        Disable();
    }
}

int LoaderTrigger(bChunk* chunk) {
    return 0;
}

int UnloaderTrigger(bChunk* chunk) {
    return 0;
}

WTriggerManager *WTriggerManager::fgTriggerManager;

WTriggerManager::WTriggerManager()
    : fSilencableEnabled(true) //
    , fIterCount(0) //
    , fgFireOnExitList(nullptr) {
    fgFireOnExitList = new FireOnExitList();
}

WTriggerManager::~WTriggerManager() {
    if (fgFireOnExitList != nullptr) {
        delete fgFireOnExitList;
    }
}

void WTriggerManager::Init() {
    fgTriggerManager = new WTriggerManager();
    Restart();
    for (unsigned int i = 0; i < WCollisionAssets::Get().NumTriggers(); i++) {
        WTrigger &trig = WCollisionAssets::Get().Trigger(i);
        if (trig.fFlags & 0x200) {
            WCollisionAssets::Get().Trigger(i).fFlags &= ~0x400;
        }
    }
}

void WTriggerManager::Restart() {
    fgTriggerManager->fgFireOnExitList->clear();
    Get().EnableSilencables();
}

void WTriggerManager::ClearAllFireOnExit() {
    if (fgFireOnExitList != nullptr) {
        fgFireOnExitList->clear();
    }
}

void WTriggerManager::DeleteRefs(const WTrigger *trig) {
    std::set<FireOnExitRec>::const_iterator iter = fgFireOnExitList->begin();
    while (iter != fgFireOnExitList->end()) {
        const FireOnExitRec &rec = *iter;
        if (trig == &rec.mTrigger) {
            std::set<FireOnExitRec>::const_iterator newlocation = iter;
            ++newlocation;
            fgFireOnExitList->erase(iter);
            iter = newlocation;
            if (iter == fgFireOnExitList->end()) {
                return;
            }
        }
        ++iter;
    }
}

void WTriggerManager::SubmitForFire(WTrigger &trig, HSIMABLE__ *hSimable) {
    if ((reinterpret_cast<const unsigned char *>(&trig)[0x12] >> 7) != 0) {
        ISimable *iSimable = ISimable::FindInstance(hSimable);
        if (iSimable != nullptr) {
            FireOnExitRec rec(trig, hSimable);
            fgFireOnExitList->insert(rec);
        } else {
            trig.FireEvents(hSimable);
        }
    }
    if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(&trig)[0x11]) << 16 & 0x40000) != 0) {
        trig.FireEvents(hSimable);
    }
    if (((reinterpret_cast<const unsigned char *>(&trig)[0x11] << 16 | reinterpret_cast<const unsigned char *>(&trig)[0x12] << 8) & 0x48000) == 0) {
        trig.FireEvents(hSimable);
    }
}

void WTriggerManager::Update(float dT) {
    fProcessingStimulus = 1;
    IRigidBody * const *enditer = IRigidBody::GetList().end();
    for (IRigidBody * const *iter = IRigidBody::GetList().begin(); iter != enditer; ++iter) {
        IRigidBody *rigidBody = *iter;
        if (rigidBody->IsSimple()) {
            ProcessSRB(rigidBody, dT);
        } else {
            ProcessRB(rigidBody, dT);
        }
    }
    fProcessingStimulus = 2;
    std::set<FireOnExitRec>::const_iterator iter = fgFireOnExitList->begin();
    while (iter != fgFireOnExitList->end()) {
        const FireOnExitRec &rec = *iter;
        ISimable *iSimable = ISimable::FindInstance(rec.mhSimable);
        if (iSimable != nullptr) {
            IRigidBody *iRigidBody = iSimable->GetRigidBody();
            if (iRigidBody != nullptr) {
                bool result;
                if (iRigidBody->IsSimple()) {
                    result = CheckCollideSRB(iRigidBody, &rec.mTrigger, dT);
                } else {
                    result = CheckCollideRB(iRigidBody, &rec.mTrigger, dT);
                }
                if (result == true) {
                    ++iter;
                    continue;
                }
                rec.mTrigger.FireEvents(rec.mhSimable);
            }
        }
        std::set<FireOnExitRec>::const_iterator newlocation = iter;
        ++newlocation;
        fgFireOnExitList->erase(iter);
        iter = newlocation;
        if (iter == fgFireOnExitList->end()) {
            return;
        }
    }
}

void WTrigger::operator delete(void *mem, unsigned int size) {
    gFastMem.Free(mem, size, nullptr);
}

WTrigger::~WTrigger() {
    if (!(fFlags & 0x100) && fEvents != nullptr) {
        ::operator delete(fEvents);
    }
    if (WTriggerManager::Exists()) {
        WTriggerManager::Get().DeleteRefs(this);
    }
}

void WTrigger::UpdateBox(const UMath::Matrix4& mat, const UMath::Vector3& dimension) {
    UMath::Vector4 oldPosRad = fPosRadius;
    unsigned int flags = fFlags;
    EventList* eventList = fEvents;

    memcpy(this, &mat, sizeof(UMath::Matrix4));

    reinterpret_cast<unsigned char *>(this)[0x10] = (reinterpret_cast<unsigned char *>(this)[0x10] & 0xF0) | 1;

    if (mat.v1.y < 0.0f) {
        flags |= 0x1000;
    } else {
        flags &= ~0x1000;
    }

    fHeight = dimension.y + dimension.y;
    fEvents = eventList;
    *reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) =
        (*reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(this) + 0x10) & 0xFF000000) | (flags & 0x00FFFFFF);
    reinterpret_cast<unsigned char *>(this)[0x10] &= 0x0F;

    fPosRadius.x = mat[3][0];
    fPosRadius.y = mat[3][1];
    fPosRadius.z = mat[3][2];
    fPosRadius.w = UMath::Length(dimension);

    fMatRow0Width.w = dimension.x + dimension.x;
    fMatRow2Length.w = dimension.z + dimension.z;

    WGridManagedDynamicElem::AddElem(&oldPosRad, &fPosRadius, WGrid_kTrigger, reinterpret_cast<unsigned int>(this));
}

bool WTrigger::UpdatePos(const UMath::Vector3 &newPos, unsigned int triggerInd) {
    UMath::Vector4 oldPosRad = fPosRadius;
    fPosRadius.x = newPos.x;
    fPosRadius.y = newPos.y;
    fPosRadius.z = newPos.z;
    WGridManagedDynamicElem::AddElem(&oldPosRad, &fPosRadius, WGrid_kTrigger, triggerInd);
    return true;
}
