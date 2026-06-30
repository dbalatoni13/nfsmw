#ifndef ACTIONQUEUE_H
#define ACTIONQUEUE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ActionData.h"
#include "ActionRef.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Input/InputDefParser.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UQueue.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

static const int MAX_ACTIONQ = 20;

// total size: 0x294
class ActionQueue : public UTL::Collections::Listable<ActionQueue, MAX_ACTIONQ> {
public:
    enum eState {
        AQS_DISABLED = 0,
        AQS_ENABLED = 1,
    };

    // static void operator delete(void *mem, void *ptr) {}

    ActionQueue(bool required);
    ActionQueue(int port, unsigned int config, const char *queue_name, bool required);
    ~ActionQueue();

    const char *GetName() {
        if (mQueueName == nullptr)
            return "";
        return mQueueName;
    }
    bool IsRequired() const { return mRequired; }
    void SetRequired(bool b) { mRequired = b; }


    bool IsEmpty();

    const ActionRef GetAction();
    void PopAction();
    void Flush();
    void SetConfig(unsigned int config, const char *queue_name);
    bool IsValid() const {
        return this->mMappings != nullptr;
    }
    void SetPort(int port);
    // int GetPort() const {}

    bool IsActive() const {
        return this->IsValid();
    }

    bool IsEnabled() const;
    void Enable(bool b);
    const ActionRef operator[](int i);
    // int Size() {}
    // Timer LastActionTime() const {}
    // Timer ActivationTime() const {}
    // static Timer LastAnyActionTime() {}

    USE_FASTALLOC(ActionQueue)

    // void *operator new(std::size_t size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    // bool IsActive() const {}

    bool IsConnected() const;

    bool ReceiveAction(ActionData &action);

    unsigned int GetConfig() const {
        return mConfig;
    }

private:
    static ActionQueue::List _mTable;

    UCircularQueue<ActionData, 50> fQueue; // offset 0x4, size 0x268

    void IO_SetConnected(bool plugged);
    void IO_Flush();
    void IO_UpdateFromDevice();


    void Init(int port, unsigned int config);

    void PrintHeader();

    void FetchCurrentValues(InputDevice *device);

    static ActionQueue *FindActionQueue(int id);


    static int AssignUniqueID();


    static Attrib::Key ConvertFromChar(char *key, int numBytes);

    int mPort;                             // offset 0x26C, size 0x4
    eState mState;                         // offset 0x270, size 0x4
    InputMapping *mMappings;               // offset 0x274, size 0x4
    unsigned int mConfig;                  // offset 0x278, size 0x4
    const char *mQueueName;                // offset 0x27C, size 0x4
    int mUniqueID;                         // offset 0x280, size 0x4
    bool mConnected;                       // offset 0x284, size 0x1
    bool mRequired;                        // offset 0x288, size 0x1

    static bool sInJoylogFrame;

    Timer mActionTime;                     // offset 0x28C, size 0x4
    Timer mActivationTime;                 // offset 0x290, size 0x4

    static Timer mLastAnyActionTime;

    void OnActivationChange();
};

#endif
