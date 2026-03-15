#include "Speed/Indep/Src/Physics/SmackableTrigger.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

SmackableTrigger::SmackableTrigger(HMODEL__ *hmodel, bool virgin, const UMath::Matrix4 &objectmatrix,
                                   const UMath::Vector3 &dim, unsigned int extra_flags) {
    unsigned int flags = extra_flags | 0x40143;
    void *eventMem = gFastMem.Alloc(0x48, "SmackTrigger");
    EventList *el = static_cast<EventList *>(eventMem);
    mTrigger = new WTrigger(&objectmatrix, &dim, el, flags);
    el->fNumEvents = 1;
    mEventData = reinterpret_cast<Event::StaticData *>(el + 2);
    EventStaticData *es = reinterpret_cast<EventStaticData *>(el + 1);
    es->fDataOffset = 0x10;
    es->fEventID = 0xEB626F77;
    es->fPad = 0;
    es->fEventSize = 0x38;
    WCollisionAssets::Get().AddTrigger(mTrigger);
    if (!virgin) {
        mTrigger->UpdateBox(&objectmatrix, &dim);
    }
    mEventData[4].fEventID = reinterpret_cast<unsigned int>(hmodel);
    mEventData[9].fEventID = static_cast<unsigned int>(virgin);
    UMath::Matrix4ToQuaternion(objectmatrix,
                               *reinterpret_cast<UMath::Vector4 *>(&mEventData[5]));
    *reinterpret_cast<UMath::Vector3 *>(&mEventData[1]) = Vector4To3(objectmatrix.v3);
    mTrigger->Enable();
}

void SmackableTrigger::Fire() {
    mTrigger->FireEvents(nullptr);
}

void SmackableTrigger::Disable() {
    mTrigger->Disable();
}

void SmackableTrigger::Enable() {
    mTrigger->Enable();
}

bool SmackableTrigger::IsEnabled() const {
    if (mTrigger->IsEnabled()) {}
    if (mTrigger->fFlags & 1)
        return true;
    return false;
}

void SmackableTrigger::GetObjectMatrix(UMath::Matrix4 &matrix) const {
    UMath::Vector4 q = *reinterpret_cast<const UMath::Vector4 *>(&mEventData[5]);
    UMath::QuaternionToMatrix4(q, matrix);
    matrix.v3 = UMath::Vector4Make(*reinterpret_cast<const UMath::Vector3 *>(&mEventData[1]), 1.0f);
}

void SmackableTrigger::Move(const UMath::Matrix4 &matrix, const UMath::Vector3 &dim, bool virgin) {
    mTrigger->UpdateBox(&matrix, &dim);
    mEventData[9].fEventID = static_cast<unsigned int>(virgin);
    UMath::Matrix4ToQuaternion(matrix,
                               *reinterpret_cast<UMath::Vector4 *>(&mEventData[5]));
    *reinterpret_cast<UMath::Vector3 *>(&mEventData[1]) = Vector4To3(matrix.v3);
}

SmackableTrigger::~SmackableTrigger() {
    gFastMem.Free(mTrigger->fEvents, 0x48, "SmackTrigger");
    mTrigger->fEvents = nullptr;
    WCollisionAssets::Get().RemoveTrigger(mTrigger);
    delete mTrigger;
    mTrigger = nullptr;
    mEventData = nullptr;
}
