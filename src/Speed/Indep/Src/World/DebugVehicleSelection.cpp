#include "./DebugVehicleSelection.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

DebugVehicleSelection *DebugVehicleSelection::mThis = nullptr;
bool DebugVehicleSelection::mOnOff = false;

void DebugVehicleSelection::Init() {
    if (mThis == nullptr) {
        mThis = ::new ("DebugVehicleSelection", 0) DebugVehicleSelection();
    }
}

DebugVehicleSelection::DebugVehicleSelection()
    : UTL::COM::Object(1), IVehicleCache(this), mSelectionIndex(0), mSelectionList(), mCollisionObject(""), mCollisionSurface("") {
    this->mSelectionList.reserve(32);
    this->InitSelectionList();
    this->mOnOff = true;
}

DebugVehicleSelection::~DebugVehicleSelection() {
    this->mSelectionList.clear();
    this->mOnOff = false;
}

bool ChangePlayerVehicle = false;

void DebugVehicleSelection::Service() {
    if (ChangePlayerVehicle) {
        this->mSelectionIndex = (this->mSelectionIndex + 1) % this->mSelectionList.size();
        this->SwitchPlayerVehicle(this->mSelectionList[this->mSelectionIndex]);
        ChangePlayerVehicle = false;
        bRefreshTweaker();
    }
}

void DebugVehicleSelection::InitSelectionList() {
    const Attrib::Class *aclass = Attrib::Database::Get().GetClass(0x4A97EC8F);
    Attrib::Key key = aclass->GetFirstCollection();
    this->mSelectionList.reserve(aclass->GetNumCollections());
    while (key != 0) {
        Attrib::Gen::pvehicle atr = Attrib::Gen::pvehicle(key, 0, nullptr);
        if (atr.MODEL().GetHash32() != UCrc32::kNull.GetValue() && atr.GetParent() == 0xA6ABC921) {
            const char *vehicleName = atr.CollectionName();
            this->mSelectionList.push_back(vehicleName);
        }
        key = aclass->GetNextCollection(key);
    }
}

// STRIPPED
void DebugVehicleSelection::DebugDisplay() {}

// STRIPPED
void DebugVehicleSelection::ReportCollision(const char *objectname, const char *surfacename) {}

bool DebugVehicleSelection::SwitchPlayerVehicle(const char *attribname) {
    IPlayer *iplayer = IPlayer::First(PLAYER_LOCAL);
    if (iplayer == nullptr)
        return false;

    ISimable *oldcar = iplayer->GetSimable();
    if (oldcar == nullptr)
        return false;

    UMath::Matrix4 transform;
    oldcar->GetTransform(transform);

    VehicleParams params(this,                            //
                         DRIVER_HUMAN,                    //
                         Attrib::StringToKey(attribname), //
                         UMath::Vector4To3(transform.v2), //
                         UMath::Vector4To3(transform.v3), //
                         2,                               //
                         nullptr,                         //
                         nullptr);

    ISimable *icar = ISimable::CreateInstance("PVehicle", params);
    if (icar != nullptr) {
        icar->Attach(IPlayer::First(PLAYER_LOCAL));
        oldcar->Kill();
        return true;
    }

    return false;
}
