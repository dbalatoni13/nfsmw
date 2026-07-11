#ifndef MAIN_EVENTSEQUENCER_H
#define MAIN_EVENTSEQUENCER_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

// TODO move?
// total size: 0x64
struct EventDynamicData {
    void Clear() {
        bMemSet(this, 0, sizeof(EventDynamicData));
    }

    UMath::Vector4 fPosition;        // offset 0x0, size 0x10
    UMath::Vector4 fVector;          // offset 0x10, size 0x10
    UMath::Vector4 fVelocity;        // offset 0x20, size 0x10
    UMath::Vector4 fAngularVelocity; // offset 0x30, size 0x10
    struct WTrigger *fTrigger;       // offset 0x40, size 0x4
    int fTriggerStimulus;            // offset 0x44, size 0x4
    uintptr_t fhSimable;             // offset 0x48, size 0x4
    uintptr_t fhActivity;            // offset 0x4C, size 0x4
    unsigned int fWorldID;           // offset 0x50, size 0x4
    uintptr_t fhModel;               // offset 0x54, size 0x4
    unsigned int fEventSeqEngine;    // offset 0x58, size 0x4
    unsigned int fEventSeqSystem;    // offset 0x5C, size 0x4
    unsigned int fEventSeqState;     // offset 0x60, size 0x4
};

extern EventDynamicData gEventDynamicData;

namespace EventSequencer {

class System;
class Engine;

class IContext : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IContext);

    virtual bool SetDynamicData(const System *system, EventDynamicData *data);
};

// TODO DECLAREHANDLE in new UTL.h
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
    enum { InstanceLimit = 434 };

    DECL_INTERFACE(IEngine);

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
    virtual bool Trigger(float externalTime, IContext *ifiringcontext, QueueMode mode);
    virtual bool FireEventTag(unsigned int, IContext *ifiringcontext);
    virtual void Flush();
    virtual void Stop(float externalTime, bool flush, IContext *ifiringcontext);
    virtual void Complete(float externalTime, bool flush, IContext *ifiringcontext);
    virtual void Pause(float externalTime, IContext *ifiringcontext);
    virtual void Resume(float externalTime, IContext *ifiringcontext);
    virtual void Reset(float externalTime);
    virtual void SetVerbose(bool verbose);
};

// total size: 0x40
class System {
  public:
    enum {
        kQueueLength = 4,
    };

    System(Engine *engine, const CARP::EventSeqSystem *system, float externalTime, float rate);

    friend void Update(float externalTime);

    unsigned int ID() const;


    IContext *GetContext() const;
    IEngine *GetEngine() const;

    unsigned int GetState() const;

    void SetState(float externalTime, unsigned int state);

    bool IsInAction() const;
    bool IsPaused() const;

    float GetActionRate() const;
    float GetActionTime() const;
    float GetActionDuration() const;

    float GetQueuedDuration() const;

    void SetActionRate(float rate);

    void Flush();
    void Stop(float externalTime, bool flush, IContext *ifiringcontext);
    void Complete(float externalTime, bool flush, IContext *ifiringcontext);
    void Pause(float externalTime, IContext *ifiringcontext);
    void Resume(float externalTime, IContext *ifiringcontext);
    void Reset(float externalTime, float rate, IContext *ifiringcontext);

    bool ProcessStimulus(unsigned int stimulus, float externalTime, IContext *ifiringcontext, EventSequencer::QueueMode mode);
    bool Trigger(float externalTime, IContext *ifiringcontext, EventSequencer::QueueMode mode);

    bool FireEventTag(unsigned int tag, IContext *ifiringcontext) const;

  private:
    ~System();

    bool Update(unsigned int index, float externalTime);

    bool TerminateAction(unsigned int tag, unsigned int index, float externalTime, bool flushQueue, IContext *ifiringcontext);
    bool InvokeStimulus(unsigned int stimulus, float externalTime, IContext *ifiringcontext);
    void FireActionEventList(bool tagIndex, unsigned int index, struct IContext *ifiringcontext) const;
    void FireTimedEvents(float startActionTime, float endActionTime) const;

    void Relocate(unsigned int deltaAddress);

    void Unload();

    void InternalReset(float externalTime, float rate);

    unsigned int GetActiveIndex() const;

    unsigned int ExecuteFilter(const CARP::EventSeqState *state, unsigned int stimulus, IContext *ifiringcontext) const;

    Engine *mEngine;                // offset 0x0, size 0x4
    CARP::EventSeqSystem *mSystem;  // offset 0x4, size 0x4
    CARP::EventSeqState *mState;    // offset 0x8, size 0x4
    CARP::EventSeqAction *mAction;  // offset 0xC, size 0x4
    float mActionRate;              // offset 0x10, size 0x4
    float mActionTime;              // offset 0x14, size 0x4
    float mActionLast;              // offset 0x18, size 0x4
    float mPausedAt;                // offset 0x1C, size 0x4
    unsigned int mCurrentState;     // offset 0x20, size 0x4
    unsigned int mEndState;         // offset 0x24, size 0x4
    unsigned int mQueueEndState;    // offset 0x28, size 0x4
    float mQueueDuration;           // offset 0x2C, size 0x4
    unsigned int mQueuedStimuli[4]; // offset 0x30, size 0x10
};

class Engine : public UTL::COM::Object, public IEngine {
public:
    USE_FASTALLOC(Engine)

    Engine(
        UTL::COM::Object *baseObj,
        IContext *context,
        const CARP::EventSeqEngine *engineData,
        float externalTime,
        float rate
    ) : UTL::COM::Object(*baseObj), IEngine(baseObj) {
        void *block;
        const CARP::EventSeqSystem *const *sysData = engineData->GetSystems();

        this->mContext = context;
        this->mEngine = const_cast<CARP::EventSeqEngine *>(engineData);
        this->mVerbose = false;
        this->mNumSystems = engineData->mNumSystems;

        new (this->mEngine) System(this, sysData[engineData->mNumSystems], externalTime, rate);

        for (unsigned int i = 0; i < engineData->mNumSystems; i++) {
            new (&this->mSystems[i]) System(this, sysData[i], externalTime, rate);
        }
    }

    // ~Engine() override {}

    // void Release() override {}

    // const char *Name() const override {}

    // void Relocate(unsigned int deltaAddress) override {}

    // void Unload() override {}

    // IContext *GetContext() const override {}

    // void SetContext(IContext *context) override {}

    // unsigned int NumSystems() const override {}

    IContext *Context() const {
        return this->mContext;
    }

    System *GetSystems() const {
        return this->mSystems;
    }

    bool IsVerbose() const {
        return this->mVerbose;
    }

private:
    CARP::EventSeqEngine *mEngine;    // offset 0x1C, size 0x4
    IContext *mContext;         // offset 0x20, size 0x4
    System *mSystems;           // offset 0x24, size 0x4
    unsigned int mNumSystems;   // offset 0x28, size 0x4
    bool mVerbose;              // offset 0x2C, size 0x1
};

void UpdateDelta(float deltaTime);
void Init(float externalTime);
void Reset(float externalTime);
IEngine *Create(UTL::COM::Object *baseObject, IContext *context, UCrc32 name, float externalTime, float rate);

}; // namespace EventSequencer

#endif
