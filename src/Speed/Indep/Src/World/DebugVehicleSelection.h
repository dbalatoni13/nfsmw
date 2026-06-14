#ifndef __E_DEBUG_VEHICLE_SECTION_H__
#define __E_DEBUG_VEHICLE_SECTION_H__

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"

// total size: 0x38
class DebugVehicleSelection : public UTL::COM::Object, public IVehicleCache {
  public:
    DebugVehicleSelection();
    ~DebugVehicleSelection();

    static bool IsEnabled() {
        return false;
    }

    static void Init();
    static void DeInit();

    static DebugVehicleSelection &Get() {
        return *mThis;
    }

    void Service();

  private:
    static DebugVehicleSelection *mThis;
    static bool mOnOff;

  public:
    void DebugDisplay();
    void ReportCollision(const char *objectname, const char *surfacename);

    eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const override {
        return VCR_DONTCARE;
    };

#ifndef EA_BUILD_A124
    const char *GetCacheName() const override {
        return "DebugVehicleSelection";
    };
#endif

    void OnRemovedVehicleCache(IVehicle *ivehicle) override {}

  private:
    void InitSelectionList();
    bool SwitchPlayerVehicle(const char *attribname);

    unsigned int mSelectionIndex;                                // offset 0x1C, size 0x4
    UTL::Std::vector<const char *, _type_vector> mSelectionList; // offset 0x20, size 0x10
    const char *mCollisionObject;                                // offset 0x30, size 0x4
    const char *mCollisionSurface;                               // offset 0x34, size 0x4
};

#endif
