#ifndef INTTERFACES_SIMACTIVITIES_IVEHICLE_CACHE_H
#define INTTERFACES_SIMACTIVITIES_IVEHICLE_CACHE_H

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

enum eVehicleCacheResult {
    VCR_DONTCARE = 1,
    VCR_WANT = 0,
};

static const int MAX_VEHICLE_CACHE = 18;

// total size: 0xC
class IVehicleCache : public UTL::COM::IUnknown, public UTL::Collections::Listable<IVehicleCache, MAX_VEHICLE_CACHE> {
  public:
    DECL_INTERFACE(IVehicleCache);

#ifndef EA_BUILD_A124
    virtual const char *GetCacheName() const;
#endif
    virtual eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const;
    virtual void OnRemovedVehicleCache(IVehicle *ivehicle);
};

#endif
