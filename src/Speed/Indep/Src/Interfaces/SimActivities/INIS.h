#ifndef INTERFACES_SIMACTIVITIES_INIS_H
#define INTERFACES_SIMACTIVITIES_INIS_H

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
    DECL_INTERFACE(INIS);

    virtual void AddCar(UCrc32 channel, IVehicle *vehicle) = 0;
    virtual IVehicle *GetCar(UCrc32 channelname) = 0;
    virtual void StartLocation(const UMath::Vector3 &position, float direction) = 0;
    virtual void StartLocationInRenderCoords(const bVector3 &position, unsigned short direction) = 0;
    virtual const UMath::Vector3 *GetStartLocation() = 0;
    virtual const UMath::Vector3 *GetStartCameraLocation() = 0;
    virtual void SetPreMovie(const char *movieName) = 0;
    virtual void SetPostMovie(const char *movieName) = 0;
    virtual CAnimChooser::eType GetType() = 0;
    virtual void Load(CAnimChooser::eType nisType, const char *scene, int cameratrack, bool PlayAsSoonAsLoaded) = 0;
    virtual bool SkipOverNIS() = 0;
    virtual void Pause() = 0;
    virtual void UnPause() = 0;
    virtual bool IsLoaded() const = 0;
    virtual bool IsPlaying() const = 0;
    virtual bool InMovie() const = 0;
    virtual void ServiceLoads() = 0;
    virtual ICEScene *GetScene() const = 0;
    virtual CAnimScene *GetAnimScene() const = 0;
    virtual void Release() = 0;
    virtual void StartEvents() = 0;
    virtual void FireEventTag(const char *tagName) = 0;
    virtual void ResetEvents(float SetTime) = 0;
    virtual void StartPlayingNow() = 0;
    virtual bool IsWorldMomement() const = 0;
};

class INISLISTENER : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(INISLISTENER);

    virtual void ArrestLevel(int level) = 0;
};

enum NIS_CAR_IDS {
    NIS_PLAYER_CAR1 = 0,
    NIS_PLAYER_CAR2 = 1,
    NIS_PLAYER_CAR3 = 2,
    NIS_PLAYER_CAR4 = 3,
    NIS_PLAYER_CAR5 = 4,
    NIS_PLAYER_CAR6 = 5,
    NIS_PLAYER_CAR7 = 6,
    NIS_PLAYER_CAR8 = 7,
    NIS_COP_CAR1 = 8,
    NIS_COP_CAR2 = 9,
    NIS_COP_CAR3 = 10,
    NIS_COP_CAR4 = 11,
    NIS_COP_CAR5 = 12,
    NIS_COP_CAR6 = 13,
    NIS_COP_CAR7 = 14,
    NIS_COP_CAR8 = 15,
    NIS_MAX_CARS = 16,
};

#endif
