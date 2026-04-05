#ifndef WORLD_DEBUGVEHICLESELECTION_H
#define WORLD_DEBUGVEHICLESELECTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"

// total size: 0x38
class DebugVehicleSelection : public UTL::COM::Object, public IVehicleCache {
  public:
    static void Init();
    static void DeInit();

    DebugVehicleSelection();
    ~DebugVehicleSelection();
    void Service();
#ifndef EA_BUILD_A124
    const char *GetCacheName() const override {
        return "DebugVehicleSelection";
    };
#endif

    static DebugVehicleSelection &Get() {
        return *mThis;
    }

  private:
    unsigned int mSelectionIndex;                                // offset 0x1C, size 0x4
    UTL::Std::vector<const char *, _type_vector> mSelectionList; // offset 0x20, size 0x10
    const char *mCollisionObject;                                // offset 0x30, size 0x4
    const char *mCollisionSurface;                               // offset 0x34, size 0x4

    static DebugVehicleSelection *mThis;
    static bool mOnOff;
};

#endif
