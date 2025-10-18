#ifndef INTERFACES_SIMACTIVITIES_INIS_H
#define INTERFACES_SIMACTIVITIES_INIS_H

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Animation/AnimChooser.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"


class INIS : public UTL::COM::IUnknown, public UTL::Collections::Singleton<INIS> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INIS(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~INIS() {}
    virtual struct ICEScene *GetScene() const;
    virtual struct CAnimScene *GetAnimScene() const;
    virtual void AddCar(UCrc32 channel, IVehicle *vehicle) const;
    virtual void AddCar(UCrc32 channelname) const;
    virtual void StartLocation(const UMath::Vector3 &position, float direction);
    virtual void StartLocationInRenderCoords(const bVector3 &position, unsigned short direction);
    virtual const UMath::Vector3 GetStartLocation();
    virtual const UMath::Vector3 GetStartCameraLocation();
    virtual void SetPreMovie(const char *movieName);
    virtual void SetPostMovie(const char *movieName);
    virtual void Load(CAnimChooser::eType nisType, const char *scene, int cameratrack, bool PlayAsSoonAsLoaded);
    virtual void StartEvents();
    virtual void FireEventTag(const char *tagName);
    virtual void Pause();
    virtual void UnPause();
    virtual void ResetEvents(float SetTime);
    virtual void ServiceLoads();
    virtual void SkipOverNIS();
    
    virtual CAnimChooser::eType GetType() {}
    virtual bool IsLoaded() const {}
    virtual bool IsPlaying() const {}
    virtual bool InMovie() const {}
    virtual void StartPlayingNow() {}
    virtual bool IsWorldMomement() const {}
};

#endif
