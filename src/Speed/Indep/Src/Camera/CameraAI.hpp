#ifndef CAMERA_CAMERAAI_H
#define CAMERA_CAMERAAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

class CameraMover;
class IBody;
class ISimable;

namespace CameraAI {

// total size: 0x18
struct Action : public UTL::COM::Object, public UTL::COM::Factory<class Director *, Action, UCrc32> {
    Action() : UTL::COM::Object(0) {}

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    void *operator new(std::size_t size, const char *name) {
        return gFastMem.Alloc(size, name);
    }

    void operator delete(void *mem, const char *name) {
        if (mem) {
            gFastMem.Free(mem, 0, name);
        }
    }

    virtual ~Action() {}

    virtual void Update(float dT) {}
    virtual void Reset() {}
    virtual const Attrib::StringKey &GetName() const = 0;
    virtual Attrib::StringKey GetNext() const = 0;
    virtual CameraMover *GetMover() = 0;
    virtual void SetSpecial(float val) {}
};

// total size: 0x2C8
class Director : public UTL::Collections::Listable<Director, 2> {
  public:
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    void *operator new(std::size_t size, const char *name) {
        return gFastMem.Alloc(size, name);
    }

    void operator delete(void *mem, const char *name) {
        if (mem) {
            gFastMem.Free(mem, 0, name);
        }
    }

    void operator delete(void *mem, std::size_t size, const char *name) {
        if (mem) {
            gFastMem.Free(mem, size, name);
        }
    }

    EVIEW_ID GetViewID() const {
        return mViewID;
    }

    Action *GetAction() {
        return mAction;
    }

    bool IsJumping() {
        return mJumpTime > 0.0f;
    }

    bool IsCinematicMomement() {
        return mIsCinematicMomement;
    }

    float GetCinematicSlowdown() {
        return mCinematicSlowdownSeconds;
    }

    void SetCinematicSlowdown(float seconds) {
        mCinematicSlowdownSeconds = seconds;
    }

    Director(EVIEW_ID viewID);
    virtual ~Director();

    void ReleaseAction();
    void Reset();
    void SetAction(Attrib::StringKey desiredMode);
    void SelectAction();
    void TotaledStart();
    void JumpStart(float time);
    void EndJumping();
    void PursuitStart();
    void EndPursuitStart();
    void Update(float dT);
    CameraMover *GetMover();

  private:
    EVIEW_ID mViewID;                       // offset 0x4, size 0x4
    Attrib::StringKey mDesiredMode;         // offset 0x8, size 0x10
    Action *mAction;                        // offset 0x18, size 0x4
    ActionQueue mInputQ;                    // offset 0x1C, size 0x294
    bool mPrepareToEnableIce;               // offset 0x2B0, size 0x1
    float mPursuitStartTime;               // offset 0x2B4, size 0x4
    float mJumpTime;                       // offset 0x2B8, size 0x4
    bool mIsCinematicMomement;             // offset 0x2BC, size 0x1
    float mCinematicSlowdownSeconds;       // offset 0x2C0, size 0x4
};

void Update(float dT);
void Reset();
void SetAction(EVIEW_ID viewID, const char *action);
void StartCinematicSlowdown(EVIEW_ID viewID, float seconds);
void Init();
void Shutdown();
void AddAvoidable(IBody *body);
void RemoveAvoidable(IBody *body);
void MaybeDoTotaledCam(IPlayer *iplayer);
void MaybeDoPursuitCam(IVehicle *ivehicle);
void MaybeKillPursuitCam(unsigned int handle);
void MaybeKillJumpCam(unsigned int handle);
void MaybeDoJumpCam(ISimable *simable);

}; // namespace CameraAI

IPlayer *FindPlayer(EVIEW_ID viewID);
CameraAI::Director *FindDirector(EVIEW_ID viewID);
CameraAI::Director *FindDirector(unsigned int handle);
bool AreMomentCamerasEnabled();

#endif
