#include "Speed/Indep/Src/Physics/SmackableTrigger.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"

static inline unsigned int ReadTriggerFlags(const WTrigger *trigger) {
    const unsigned char *b = (const unsigned char *)trigger;
    unsigned int hi = (unsigned int)b[0x11] << 16;
    return (unsigned int)b[0x13] | (((unsigned int)b[0x12] << 8) | hi);
}

static inline void WriteTriggerFlags(WTrigger *trigger, unsigned int v) {
    unsigned char *b = (unsigned char *)trigger;
    unsigned int w = *(unsigned int *)(b + 0x10);
    *(unsigned int *)(b + 0x10) = (w & 0xFF000000u) | v;
}

SmackableTrigger::SmackableTrigger(HMODEL hmodel, bool virgin, const UMath::Matrix4 &objectmatrix, const UMath::Vector3 &dim,
                                   unsigned int extra_flags) {
    unsigned int flags = extra_flags | 0x40143u;
    CARP::EventList *events = (CARP::EventList *)gFastMem.Alloc(0x48, "SmackTrigger");
    mTrigger = new WTrigger(objectmatrix, dim, events, flags);
    events->fNumEvents = 1;
    mEventData = (ESpawnSmackable::StaticData *)&events->Event()[1];
    CARP::EventStaticData *event = events->Event();
    event->fDataOffset = sizeof(CARP::EventStaticData);
    event->fEventID = ESpawnSmackable::kEventID;
    event->fPad = 0;
    event->fEventSize = 0x38; // sizeof(ESpawnSmackable)
    WCollisionAssets::Get().AddTrigger(mTrigger);
    if (!virgin) {
        mTrigger->UpdateBox(objectmatrix, dim);
    }
    mEventData->fScenery = hmodel;
    mEventData->fVirginSpawn = virgin;
    UMath::Matrix4ToQuaternion(objectmatrix, mEventData->fOrientation);
    mEventData->fPosition = UMath::Vector4To3(objectmatrix.v3);
    WriteTriggerFlags(mTrigger, ReadTriggerFlags(mTrigger) | 1u);
}

void SmackableTrigger::Fire() {
    mTrigger->FireEvents(nullptr);
}

void SmackableTrigger::Disable() {
    WriteTriggerFlags(mTrigger, ReadTriggerFlags(mTrigger) & 0x00FFFFFEu);
}

void SmackableTrigger::Enable() {
    WriteTriggerFlags(mTrigger, ReadTriggerFlags(mTrigger) | 1u);
}

bool SmackableTrigger::IsEnabled() const {
    if (((unsigned char *)mTrigger)[0x13] & 1) {
        return true;
    }
    return false;
}

void SmackableTrigger::GetObjectMatrix(UMath::Matrix4 &matrix) const {
    UMath::Vector4 q = mEventData->fOrientation;
    UMath::QuaternionToMatrix4(q, matrix);
    matrix[3] = UMath::Vector4Make(mEventData->fPosition, 1.0f);
}

void SmackableTrigger::Move(const UMath::Matrix4 &matrix, const UMath::Vector3 &dim, bool virgin) {
    mTrigger->UpdateBox(matrix, dim);
    mEventData->fVirginSpawn = virgin;
    UMath::Matrix4ToQuaternion(matrix, mEventData->fOrientation);
    mEventData->fPosition = UMath::Vector4To3(matrix.v3);
}

SmackableTrigger::~SmackableTrigger() {
    gFastMem.Free(mTrigger->fEvents, 0x48, "SmackTrigger");
    mTrigger->fEvents = nullptr;
    WCollisionAssets::Get().RemoveTrigger(mTrigger);
    delete mTrigger;
    mTrigger = nullptr;
    mEventData = nullptr;
}
