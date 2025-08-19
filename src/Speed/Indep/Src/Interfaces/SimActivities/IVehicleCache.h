#ifndef INTTERFACES_SIMACTIVITIES_IVEHICLE_CACHE_H
#define INTTERFACES_SIMACTIVITIES_IVEHICLE_CACHE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

enum eVehicleCacheResult {
    VCR_DONTCARE = 1,
    VCR_WANT = 0,
};

class IVehicleCache : public UTL::COM::IUnknown, public UTL::Collections::Listable<IVehicleCache, 18> {
    // IVehicleCache() {}

  protected:
    virtual ~IVehicleCache() {}

  public:
    IVehicleCache(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual void OnRemovedVehicleCache(IVehicle *ivehicle);
    virtual eVehicleCacheResult OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const;

    virtual const char *GetCacheName() const {}
};

#endif
