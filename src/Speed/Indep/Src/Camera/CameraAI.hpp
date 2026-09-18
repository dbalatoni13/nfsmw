#ifndef CAMERA_CAMERAAI_H
#define CAMERA_CAMERAAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/attribuserinclude.h"

class CameraMover;

DECLARE_CONTAINER_TYPE(CameraAIAvoidables);

struct Avoidables : public UTL::Std::list<IBody *, _type_CameraAIAvoidables> {
    USE_FASTALLOC(Avoidables)
};

extern Avoidables *TheAvoidables;

namespace CameraAI {

class Director;
class Action;

class Action : public UTL::COM::Object, public UTL::COM::Factory<Director *, Action, UCrc32> {
  public:
    Action(unsigned int icount) : UTL::COM::Object(icount) {}

    void operator delete(void *mem, size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    virtual ~Action() {}
    virtual void Update(float dT) = 0;
    virtual void Reset() = 0;
    virtual const Attrib::StringKey &GetName() const = 0;
    virtual Attrib::StringKey GetNext() const = 0;
    virtual CameraMover *GetMover() = 0;
    virtual void SetSpecial(float scale) = 0;
};

class Director : public UTL::Collections::Listable<Director, 2> {
  public:
    EVIEW_ID mView;               // offset 0x4, size 0x4
    Attrib::StringKey mActionName; // offset 0x8, size 0x10
    Action *mAction;              // offset 0x18, size 0x4
    ActionQueue mQueue;           // offset 0x1C, size 0x294
    bool mUnused;                 // offset 0x2B0, size 0x1
    float mPursuitTimer;          // offset 0x2B4, size 0x4
    float mJumpTimer;             // offset 0x2B8, size 0x4
    int mFlags;                   // offset 0x2BC, size 0x4
    float mCinematicScale;        // offset 0x2C0, size 0x4
    // vptr                       // offset 0x2C4, size 0x4

    Director(EVIEW_ID view);
    virtual ~Director();

    void ReleaseAction();
    void Reset();
    void SetAction(Attrib::StringKey action);
    void SelectAction();
    void TotaledStart();
    void JumpStart(float t);
    void EndJumping();
    void PursuitStart();
    void EndPursuitStart();
    void Update(float dT);
    CameraMover *GetMover();

    EVIEW_ID GetView() const {
        return mView;
    }

    Action *GetAction() {
        return mAction;
    }

    float GetJumpTimer() const {
        return mJumpTimer;
    }

    bool IsJumping() {
        return mJumpTimer > 0.0f;
    }

    float GetPursuitTimer() const {
        return mPursuitTimer;
    }

    int GetFlags() const {
        return mFlags;
    }

    int IsCinematicMomement() {
        return mFlags;
    }

    float GetCinematicScale() const {
        return mCinematicScale;
    }

    void SetCinematicSlowdown(float seconds) {
        mCinematicScale = seconds;
    }

    USE_FASTALLOC(Director)
};

void Update(float dT);
void Reset();
void Init();
void Shutdown();
void MaybeDoTotaledCam(IPlayer *iplayer);
void MaybeDoPursuitCam(IVehicle *ivehicle);
void MaybeDoJumpCam(ISimable *isimable);
void MaybeKillJumpCam(unsigned int world_id);
void MaybeKillPursuitCam(unsigned int world_id);
void SetAction(EVIEW_ID view, const char *action);
void AddAvoidable(class IBody *ibody);
void RemoveAvoidable(class IBody *ibody);
void StartCinematicSlowdown(EVIEW_ID view, float scale);

}; // namespace CameraAI

class ISimable;

// Sus inline (_IHandle y el destructor) salen de CameraAI.hpp:147 en el original.
class IDebugWatchCar : public UTL::COM::IUnknown, public UTL::Collections::Listable<IDebugWatchCar, 2> {
  public:
    virtual ISimable *GetSimable() = 0;

    DECL_INTERFACE(IDebugWatchCar);
};

#endif
