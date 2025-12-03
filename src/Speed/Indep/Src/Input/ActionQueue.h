#ifndef INPUT_ACTION_QUEUE_H
#define INPUT_ACTION_QUEUE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ActionData.h"
#include "ActionRef.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UQueue.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// total size: 0x294
class ActionQueue : public UTL::Collections::Listable<ActionQueue, 20> {
  public:
    enum eState {
        AQS_DISABLED = 0,
        AQS_ENABLED = 1,
    };

    ActionQueue(bool required);
    ~ActionQueue();

    bool IsEmpty();
    const ActionRef GetAction();
    void PopAction();
    void Flush();
    bool IsEnabled() const;
    void Enable(bool b);
    void SetPort(int port);
    void SetConfig(unsigned int config, const char *queue_name);

    // static Timer LastAnyActionTime() {}

    // void *operator new(std::size_t size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    // const char *GetName() {}

    // bool IsRequired() const {}

    // void SetRequired(bool b) {}

    // bool IsValid() const {}

    // int GetPort() const {}

    // bool IsActive() const {}

    // int Size() {}

    // Timer LastActionTime() const {}

    // Timer ActivationTime() const {}

    unsigned int GetConfig() const {
        return mConfig;
    }

  private:
    UCircularQueue<ActionData, 50> fQueue; // offset 0x4, size 0x268
    int mPort;                             // offset 0x26C, size 0x4
    eState mState;                         // offset 0x270, size 0x4
    struct InputMapping *mMappings;        // offset 0x274, size 0x4
    unsigned int mConfig;                  // offset 0x278, size 0x4
    const char *mQueueName;                // offset 0x27C, size 0x4
    int mUniqueID;                         // offset 0x280, size 0x4
    bool mConnected;                       // offset 0x284, size 0x1
    bool mRequired;                        // offset 0x288, size 0x1
    Timer mActionTime;                     // offset 0x28C, size 0x4
    Timer mActivationTime;                 // offset 0x290, size 0x4
};

#endif
