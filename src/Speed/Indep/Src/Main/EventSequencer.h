#ifndef MAIN_EVENTSEQUENCER_H
#define MAIN_EVENTSEQUENCER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// TODO move?
struct EventDynamicData {
    // total size: 0x64
    UMath::Vector4 fPosition;        // offset 0x0, size 0x10
    UMath::Vector4 fVector;          // offset 0x10, size 0x10
    UMath::Vector4 fVelocity;        // offset 0x20, size 0x10
    UMath::Vector4 fAngularVelocity; // offset 0x30, size 0x10
    struct WTrigger *fTrigger;       // offset 0x40, size 0x4
    int fTriggerStimulus;            // offset 0x44, size 0x4
    unsigned int fhSimable;          // offset 0x48, size 0x4
    unsigned int fhActivity;         // offset 0x4C, size 0x4
    unsigned int fWorldID;           // offset 0x50, size 0x4
    unsigned int fhModel;            // offset 0x54, size 0x4
    unsigned int fEventSeqEngine;    // offset 0x58, size 0x4
    unsigned int fEventSeqSystem;    // offset 0x5C, size 0x4
    unsigned int fEventSeqState;     // offset 0x60, size 0x4
};

namespace EventSequencer {

class System;

class IContext : public UTL::COM::IUnknown {
  public:
    IContext(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IContext() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual bool SetDynamicData(const System *system, EventDynamicData *data);
};

struct HENGINE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HENGINE__ *HENGINE;

enum QueueMode {
    QUEUE_DISABLE = 0,
    QUEUE_SHALLOW = 1,
    QUEUE_ALLOW = 2,
    QUEUE_FLUSH = 3,
    QUEUE_ABORT = 4,
};

class IEngine : public UTL::COM::IUnknown, public UTL::Collections::Instanceable<HENGINE, IEngine, 434> {
  public:
    IEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IEngine() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual void Release();
    virtual const char *Name() const;
    virtual void Relocate(unsigned int deltaAddress);
    virtual void Unload();
    virtual IContext *GetContext() const;
    virtual void SetContext(IContext *context);
    virtual unsigned int NumSystems() const;
    virtual unsigned int GetSystemID(unsigned int index) const;
    virtual System *GetSystemByIndex(unsigned int index) const;
    virtual System *FindSystem(unsigned int systemID) const;
    virtual bool AnySystemInAction() const;
    virtual void SetAllSystemsState(float externalTime, unsigned int state);
    virtual bool ProcessStimulus(unsigned int systemID, unsigned int stimulus, float externalTime, IContext *ifiringcontext, QueueMode mode);
    virtual bool ProcessStimulus(unsigned int stimulus, float externalTime, IContext *ifiringcontext, QueueMode mode);
    virtual bool Trigger(float externalTime, EventSequencer::IContext *ifiringcontext, QueueMode mode);
    virtual bool FireEventTag(unsigned int, IContext *ifiringcontext);
    virtual void Flush();
    virtual void Stop(float externalTime, bool flush, IContext *ifiringcontext);
    virtual void Complete(float externalTime, bool flush, IContext *ifiringcontext);
    virtual void Pause(float externalTime, IContext *ifiringcontext);
    virtual void Resume(float externalTime, IContext *ifiringcontext);
    virtual void Reset(float externalTime);
    virtual void SetVerbose(bool verbose);
};

struct System {
    // total size: 0x40
    enum {
        kQueueLength = 4,
    };
    // TODO it's EventSequencer::Engine
    struct Engine *mEngine;               // offset 0x0, size 0x4
    const struct EventSeqSystem *mSystem; // offset 0x4, size 0x4
    const struct EventSeqState *mState;   // offset 0x8, size 0x4
    const struct EventSeqAction *mAction; // offset 0xC, size 0x4
    float mActionRate;                    // offset 0x10, size 0x4
    float mActionTime;                    // offset 0x14, size 0x4
    float mActionLast;                    // offset 0x18, size 0x4
    float mPausedAt;                      // offset 0x1C, size 0x4
    unsigned int mCurrentState;           // offset 0x20, size 0x4
    unsigned int mEndState;               // offset 0x24, size 0x4
    unsigned int mQueueEndState;          // offset 0x28, size 0x4
    float mQueueDuration;                 // offset 0x2C, size 0x4
    unsigned int mQueuedStimuli[4];       // offset 0x30, size 0x10
};

}; // namespace EventSequencer

#endif
