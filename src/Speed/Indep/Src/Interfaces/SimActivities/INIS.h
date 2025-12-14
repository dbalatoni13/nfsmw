#ifndef INTERFACES_SIMACTIVITIES_INIS_H
#define INTERFACES_SIMACTIVITIES_INIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Animation/AnimChooser.hpp"
#include "Speed/Indep/Src/Animation/AnimScene.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

class INIS : public UTL::COM::IUnknown, public UTL::Collections::Singleton<INIS> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INIS(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    ~INIS() override {}

    virtual void AddCar(UCrc32 channel, IVehicle *vehicle);
    virtual IVehicle *GetCar(UCrc32 channelname);
    virtual void StartLocation(const UMath::Vector3 &position, float direction);
    virtual void StartLocationInRenderCoords(const bVector3 &position, unsigned short direction);
    virtual const UMath::Vector3 *GetStartLocation();
    virtual const UMath::Vector3 *GetStartCameraLocation();
    virtual void SetPreMovie(const char *movieName);
    virtual void SetPostMovie(const char *movieName);

    virtual CAnimChooser::eType GetType() {}

    virtual void Load(CAnimChooser::eType nisType, const char *scene, int cameratrack, bool PlayAsSoonAsLoaded);
    virtual bool SkipOverNIS();
    virtual void Pause();
    virtual void UnPause();

    virtual bool IsLoaded() const {}

    virtual bool IsPlaying() const {}

    virtual bool InMovie() const {}

    virtual void ServiceLoads();
    virtual ICEScene *GetScene() const;
    virtual CAnimScene *GetAnimScene() const;
    virtual void Release();
    virtual void StartEvents();
    virtual void FireEventTag(const char *tagName);
    virtual void ResetEvents(float SetTime);

    virtual void StartPlayingNow() {}

    virtual bool IsWorldMomement() const {}
};

#endif
