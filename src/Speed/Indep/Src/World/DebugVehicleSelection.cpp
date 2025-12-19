#include "./DebugVehicleSelection.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "dolphin/types.h"
#include <cstddef>

void DebugVehicleSelection::Init() {
    if (!this->mThis) {
        this->mThis = ::new (__FILE__, __LINE__) DebugVehicleSelection();
    }
}

DebugVehicleSelection::DebugVehicleSelection() : UTL::COM::Object(1), IVehicleCache((UTL::COM::Object *)this) {
    this->mCollisionObject = "";
    this->mCollisionSurface = "";
    this->mSelectionIndex = 0;
    this->mSelectionList.reserve(32);
    this->InitSelectionList();
    this->mOnOff = 1;
}

// UNSOLVED
DebugVehicleSelection::~DebugVehicleSelection() {
    this->mSelectionList.clear();
    this->mOnOff = 0;
}

bool ChangePlayerVehicle = false;

void DebugVehicleSelection::Service() {
    if (ChangePlayerVehicle) {
        this->mSelectionIndex = ++this->mSelectionIndex - (this->mSelectionIndex / this->mSelectionList.size() * this->mSelectionList.size());
        this->SwitchPlayerVehicle(this->mSelectionList[this->mSelectionIndex]);
        ChangePlayerVehicle = false;
        bRefreshTweaker();
    }
}

void DebugVehicleSelection::InitSelectionList() {
    const Attrib::Class *aClass = Attrib::Database::Get().GetClass(0x4A97EC8F);
    unsigned int key = aClass->GetFirstCollection();
    this->mSelectionList.reserve(aClass->GetNumCollections());
    while (key != 0) {
        Attrib::Gen::pvehicle atr = Attrib::Gen::pvehicle(key, 0, nullptr);
        if (atr.MODEL().GetHash32() != UCrc32::kNull.GetValue() && atr.GetParent() == 0xA6ABC921) {
            const char *vehicleName = atr.CollectionName();
            this->mSelectionList.push_back(vehicleName);
        }
        key = aClass->GetNextCollection(key);
    }
}

bool DebugVehicleSelection::SwitchPlayerVehicle(const char *attribName) {
    IPlayer *iplayer = UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::First(PLAYER_LOCAL);
    if (!iplayer) return false;

    ISimable *oldcar = iplayer->GetSimable();
    if (!oldcar) return false;

    UMath::Matrix4 transform;
    oldcar->GetTransform(transform);
    // Attrib::StringToKey(attribName);
    // VehicleParams params = VehicleParams::VehicleParams();

    // ISimable *icar = UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::First(PLAYER_LOCAL)->GetSimable();

    // oldcar->Attach(icar);
    oldcar->Kill();

    return true;
}
