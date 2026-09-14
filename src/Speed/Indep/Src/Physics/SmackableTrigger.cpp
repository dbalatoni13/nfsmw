#include "SmackableTrigger.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDisposable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRecordablePlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/AI/AIRoadBlock.h"

SmackableTrigger::SmackableTrigger(HMODEL hmodel, bool virgin, const UMath::Matrix4 &objectmatrix, const UMath::Vector3 &dim,
                                   unsigned int extra_flags) {
    unsigned int flags = extra_flags | 0x40143;
    void *eventMem = gFastMem.Alloc(0x48, "SmackTrigger");
    CARP::EventList *el = static_cast<CARP::EventList *>(eventMem);
    this->mTrigger = new WTrigger(objectmatrix, dim, el, flags);
    el->fNumEvents = 1;
    CARP::EventStaticData *es = el->Event();
    this->mEventData = reinterpret_cast<ESpawnSmackable::StaticData *>(&es[1]);
    es->fDataOffset = 0x10;
    es->fEventID = ESpawnSmackable::kEventID;
    es->fPad = 0;
    es->fEventSize = 0x38;
    WCollisionAssets::Get().AddTrigger(this->mTrigger);
    if (!virgin) {
        this->mTrigger->UpdateBox(objectmatrix, dim);
    }
    this->mEventData->fScenery = hmodel;
    this->mEventData->fVirginSpawn = virgin;
    UMath::Matrix4ToQuaternion(objectmatrix, this->mEventData->fOrientation);
    this->mEventData->fPosition = UMath::Vector4To3(objectmatrix.v3);
    this->mTrigger->Enable();
}

void SmackableTrigger::Fire() {
    this->mTrigger->FireEvents(nullptr);
}

void SmackableTrigger::Disable() {
    this->mTrigger->Disable();
}

void SmackableTrigger::Enable() {
    this->mTrigger->Enable();
}

bool SmackableTrigger::IsEnabled() const {
    if (this->mTrigger->IsEnabled()) {
        return true;
    }
    return false;
}

void SmackableTrigger::GetObjectMatrix(UMath::Matrix4 &matrix) const {
    UMath::Vector4 q = this->mEventData->fOrientation;
    UMath::QuaternionToMatrix4(q, matrix);
    matrix.v3 = UMath::Vector4Make(this->mEventData->fPosition, 1.0f);
}

void SmackableTrigger::Move(const UMath::Matrix4 &matrix, const UMath::Vector3 &dim, bool virgin) {
    this->mTrigger->UpdateBox(matrix, dim);
    this->mEventData->fVirginSpawn = virgin;
    UMath::Matrix4ToQuaternion(matrix, this->mEventData->fOrientation);
    this->mEventData->fPosition = UMath::Vector4To3(matrix.v3);
}

SmackableTrigger::~SmackableTrigger() {
    gFastMem.Free(this->mTrigger->fEvents, 0x48, "SmackTrigger");
    this->mTrigger->fEvents = nullptr;
    WCollisionAssets::Get().RemoveTrigger(this->mTrigger);
    delete this->mTrigger;
    this->mTrigger = nullptr;
    this->mEventData = nullptr;
}

#define IMPL_LISTABLE(TYPE, N)                                                                                                                       \
    template <> UTL::Collections::Listable<TYPE, N>::List UTL::Collections::Listable<TYPE, N>::_mTable = UTL::Collections::Listable<TYPE, N>::List();

#define IMPL_LISTABLESET(TYPE, N, ENUM, BUCKETS)                                                                                                     \
    template <>                                                                                                                                      \
    UTL::Collections::ListableSet<TYPE, N, ENUM, BUCKETS>::_ListSet UTL::Collections::ListableSet<TYPE, N, ENUM, BUCKETS>::_mLists =                 \
        UTL::Collections::ListableSet<TYPE, N, ENUM, BUCKETS>::_ListSet();

#define IMPL_INSTANCABLE(HANDLE, TYPE, N)                                                                                                            \
    template <>                                                                                                                                      \
    UTL::Collections::Instanceable<HANDLE, TYPE, N>::_List UTL::Collections::Instanceable<HANDLE, TYPE, N>::_mList =                                 \
        UTL::Collections::Instanceable<HANDLE, TYPE, N>::_List();                                                                                     \
    template <> unsigned int UTL::Collections::Instanceable<HANDLE, TYPE, N>::_mHNext = 0;

IMPL_LISTABLE(IExplosion, 96)
IMPL_LISTABLE(IDisposable, 160)
IMPL_LISTABLESET(IVehicle, 10, eVehicleList, 10)
IMPL_LISTABLE(IRigidBody, 160)
IMPL_LISTABLE(ICollisionBody, 160)
IMPL_LISTABLE(ISimpleBody, 96)
IMPL_LISTABLESET(Sim::IEntity, 8, eEntityList, 4)
IMPL_LISTABLESET(IPlayer, 8, ePlayerList, 3)
IMPL_LISTABLE(IRecordablePlayer, 8)
IMPL_LISTABLE(IInputPlayer, 8)
IMPL_LISTABLE(IModel, 434)
IMPL_LISTABLE(IPursuit, 8)
IMPL_LISTABLE(IRoadBlock, 8)
IMPL_LISTABLE(IHud, 2)
IMPL_LISTABLE(IVehicleCache, 18)
IMPL_LISTABLE(ITrafficCenter, 8)
IMPL_LISTABLE(ISpikeable, 10)
IMPL_INSTANCABLE(HSIMABLE, ISimable, 160)
IMPL_INSTANCABLE(HACTIVITY, Sim::IActivity, 40)
IMPL_INSTANCABLE(HMODEL, IModel, 434)
IMPL_INSTANCABLE(HCAUSE, ICause, 10)
