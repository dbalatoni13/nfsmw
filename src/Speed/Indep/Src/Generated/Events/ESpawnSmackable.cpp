#include "ESpawnSmackable.hpp"

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Physics/Smackable.h"

ESpawnSmackable::ESpawnSmackable(UMath::Vector3 pPosition, HMODEL pScenery, UMath::Vector4 pOrientation, bool pVirginSpawn, WTrigger *pTrigger,
                                 unsigned int phSimable)
    : Event(0x40), fPosition(pPosition), fScenery(pScenery), fOrientation(pOrientation), fVirginSpawn(pVirginSpawn), fTrigger(pTrigger),
      fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) phSimable);

    if (isimable) {
        IModel *iscenery = IModel::FindInstance(fScenery);

        if (!iscenery || iscenery->GetWorldID() == isimable->GetWorldID()) {

            fScenery = NULL;
        }
    }
}

ESpawnSmackable::~ESpawnSmackable() {
    UMath::Vector4 q = fOrientation;
    UMath::Matrix4 mat;

    UMath::QuaternionToMatrix4(q, mat);
    mat.v3 = UMath::Vector4Make(fPosition, 1.0f);

    IModel *iscenery = IModel::FindInstance(fScenery);

    if (iscenery) {

        if (iscenery->GetSimable()) {

            fTrigger->Enable();
        } else {

            ISimable *isim = ISimable::CreateInstance("Smackable", SmackableParams(mat, fVirginSpawn, iscenery, false));

            if (!isim && fTrigger) {
                fTrigger->Enable();
            }
        }
    }
}

const char *ESpawnSmackable::GetEventName() const {
    return "ESpawnSmackable";
}

void ESpawnSmackable_MakeEvent_Callback(const void *staticData) {
    new ESpawnSmackable(((ESpawnSmackable::StaticData *) staticData)->fPosition, ((ESpawnSmackable::StaticData *) staticData)->fScenery,
                        ((ESpawnSmackable::StaticData *) staticData)->fOrientation, ((ESpawnSmackable::StaticData *) staticData)->fVirginSpawn,
                        gEventDynamicData.fTrigger, gEventDynamicData.fhSimable);
}
