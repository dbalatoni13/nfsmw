#include "./DebugVehicleSelection.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "dolphin/types.h"

void DebugVehicleSelection::Init() {
    if (!this->mThis) {
        this->mThis = new (__FILE__, __LINE__) DebugVehicleSelection();
    }
}

DebugVehicleSelection::DebugVehicleSelection() : UTL::COM::Object(1), IVehicleCache((UTL::COM::Object *)this) {
    this->mCollisionObject = nullptr;
    this->mCollisionSurface = nullptr;
    this->mSelectionList = UTL::Std::vector<const char *, _type_vector>();
    this->InitSelectionList();
    this->mOnOff = 1;
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
    if (key != 0) {
        // Attrib::Gen::pvehicle atr;
        // Attrib::Instance::Instance(Attrib::FindCollection(atr.ClassKey(), key))
    }
}
