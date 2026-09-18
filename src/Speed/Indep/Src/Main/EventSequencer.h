#ifndef MAIN_EVENTSEQUENCER_H
#define MAIN_EVENTSEQUENCER_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UTL.h"

#include "Speed/Indep/Src/Main/EventDynamicData.h"

namespace EventSequencer {

class System;
class Engine;

class IContext : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IContext);

    virtual bool SetDynamicData(const System *system, EventDynamicData *data) = 0;
};

DECLAREHANDLE(HENGINE);

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

    virtual void Release() = 0;
    virtual const char *Name() const = 0;
    virtual void Relocate(unsigned int deltaAddress) = 0;
    virtual void Unload() = 0;
    virtual IContext *GetContext() const = 0;
    virtual void SetContext(IContext *context) = 0;
    virtual unsigned int NumSystems() const = 0;
    virtual unsigned int GetSystemID(unsigned int index) const = 0;
    virtual System *GetSystemByIndex(unsigned int index) const = 0;
    virtual System *FindSystem(unsigned int systemID) const = 0;
    virtual bool AnySystemInAction() const = 0;
    virtual void SetAllSystemsState(float externalTime, unsigned int state) = 0;
    virtual bool ProcessStimulus(unsigned int systemID, unsigned int stimulus, float externalTime, IContext *ifiringcontext, QueueMode mode) = 0;
    virtual bool ProcessStimulus(unsigned int stimulus, float externalTime, IContext *ifiringcontext, QueueMode mode) = 0;
    virtual bool Trigger(float externalTime, IContext *ifiringcontext, QueueMode mode) = 0;
    virtual bool FireEventTag(unsigned int, IContext *ifiringcontext) = 0;
    virtual void Flush() = 0;
    virtual void Stop(float externalTime, bool flush, IContext *ifiringcontext) = 0;
    virtual void Complete(float externalTime, bool flush, IContext *ifiringcontext) = 0;
    virtual void Pause(float externalTime, IContext *ifiringcontext) = 0;
    virtual void Resume(float externalTime, IContext *ifiringcontext) = 0;
    virtual void Reset(float externalTime) = 0;
    virtual void SetVerbose(bool verbose) = 0;
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
    friend class Engine;

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

void UpdateDelta(float deltaTime);
void Init(float externalTime);
void Reset(float externalTime);
IEngine *Create(UTL::COM::Object *baseObject, IContext *context, UCrc32 name, float externalTime, float rate);

}; // namespace EventSequencer


namespace EventSequencer {

class Engine : public UTL::COM::Object, public IEngine {
public:
    USE_FASTALLOC(Engine)

    Engine(
        UTL::COM::Object *baseObj,
        IContext *context,
        const CARP::EventSeqEngine *engineData,
        float externalTime,
        float rate
    ) : UTL::COM::Object(0), IEngine(baseObj != NULL ? baseObj : this), mEngine(const_cast<CARP::EventSeqEngine *>(engineData)), mContext(context), mNumSystems(engineData->mNumSystems), mVerbose(false) {
        const CARP::EventSeqSystem *const *sysData = engineData->GetSystems();

        void *block = gFastMem.Alloc(this->mNumSystems * sizeof(System), "EventSequencerSystems");

        this->mSystems = new (block) System(this, sysData[0], externalTime, rate);

        for (unsigned int i = 1; i < this->mNumSystems; i++) {
            new (&this->mSystems[i]) System(this, sysData[i], externalTime, rate);
        }
    }

    ~Engine() override;

    void Release() override;
    const char *Name() const override;
    void Relocate(unsigned int deltaAddress) override;
    void Unload() override;
    IContext *GetContext() const override;
    void SetContext(IContext *context) override;
    unsigned int NumSystems() const override;
    unsigned int GetSystemID(unsigned int index) const override;
    System *GetSystemByIndex(unsigned int index) const override;
    System *FindSystem(unsigned int systemID) const override;
    bool AnySystemInAction() const override;
    void SetAllSystemsState(float externalTime, unsigned int state) override;
    bool ProcessStimulus(unsigned int systemID, unsigned int stimulus, float externalTime, IContext *ifiringcontext, QueueMode mode) override;
    bool ProcessStimulus(unsigned int stimulus, float externalTime, IContext *ifiringcontext, QueueMode mode) override;
    bool Trigger(float externalTime, IContext *ifiringcontext, QueueMode mode) override;
    bool FireEventTag(unsigned int tag, IContext *ifiringcontext) override;
    void Flush() override;
    void Stop(float externalTime, bool flush, IContext *ifiringcontext) override;
    void Complete(float externalTime, bool flush, IContext *ifiringcontext) override;
    void Pause(float externalTime, IContext *ifiringcontext) override;
    void Resume(float externalTime, IContext *ifiringcontext) override;
    void Reset(float externalTime) override;
    void SetVerbose(bool verbose) override;

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

}; // namespace EventSequencer

#endif /* MAIN_EVENTSEQUENCER_H */
