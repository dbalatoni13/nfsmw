#ifndef WORLD_DEBUGVEHICLESELECTION_H
#define WORLD_DEBUGVEHICLESELECTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"

struct DebugVehicleSelection : public UTL::COM::Object, public IVehicleCache {
public:
    DebugVehicleSelection();
    
    void Init();
    void DeInit();
    void InitSelectionList();
    bool SwitchPlayerVehicle(const char *attribname);

    void Service();

    static DebugVehicleSelection &Get() {
        return *mThis;
    }

    static bool Exists() {
        return mThis != nullptr;
    }

    // NOT REAL, figure out how to remove this (and replace with bWare.hpp)
    void *operator new(std::size_t size, const char *file, int line) {
        return new char[0x38];
    }

private:
    // total size: 0x38
    unsigned int mSelectionIndex; // offset 0x1C, size 0x4
    UTL::Std::vector<const char *,_type_vector> mSelectionList; // offset 0x20, size 0x10
    const char *mCollisionObject; // offset 0x30, size 0x4
    const char *mCollisionSurface; // offset 0x34, size 0x4

    static DebugVehicleSelection *mThis;
    static bool mOnOff;
};

#endif
